/*
* Copyright © 2015 Cask Data, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License"); you may not
* use this file except in compliance with the License. You may obtain a copy of
* the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations under
* the License.
*/

#include "stdafx.h"
#include "QueryDataReader.h"
#include "QueryCommand.h"
#include "Driver.h"

void Cask::CdapOdbc::QueryDataReader::adjustFetchSize(std::int64_t responseSize) {
  auto oneRow = responseSize / this->fetchSize;
  auto newFetchSize = OPTIMAL_RESPONSE_SIZE_IN_BYTES / oneRow;
  this->fetchSize = static_cast<int>(newFetchSize);
  TRACE(L"FETCH SIZE adjusted to: %d\n", this->fetchSize);
}

void Cask::CdapOdbc::QueryDataReader::checkTasksForExceptions() {
  auto it = this->tasks.begin();
  while (it != this->tasks.end()) {
    if (it->is_done()) {
      // Check for exceptions
      it->get();
      it = this->tasks.erase(it);
    } else {
      ++it;
    }
  }
}

bool Cask::CdapOdbc::QueryDataReader::loadDataContinue() {
  // Discard fetched frame
  this->frameCache.pop();
  TRACE(L"Popped frame. Frame count: %d\n", this->frameCache.size());

  this->currentRowIndex = 0;

  if (this->moreFrames) {
    // Start loading next async frame
    auto& task = this->loadFrameAsync();

    // At this point we must know if there are more rows in query
    // If data fetched quickier than loading cache can be empty
    // In this case we have to wait for next frame
    if (this->frameCache.empty()) {
      task.wait();
      
      // As task done check immediately for exceptions
      task.get();
    }
  }

  if (!this->frameCache.empty() && this->getFrame().getSize() > 0) {
    return true;
  } else {
    return false;
  }
}

bool Cask::CdapOdbc::QueryDataReader::loadDataBegin() {
  if (this->moreFrames && this->loadFrame()) {
    this->currentRowIndex = 0;
    // After first frame loaded we could try to load a few more
    this->tryLoadFrameAsync();
    return true;
  } else {
    return false;
  }
}

bool Cask::CdapOdbc::QueryDataReader::loadFrame() {
  auto frame = this->queryCommand->loadRows(this->fetchSize);
  if (frame.getSize() > 0) {
    this->frameCache.push(frame);
    TRACE(L"Pushed frame. Frame count: %d\n", this->frameCache.size());
    // We assume there is more data 
    // if actual frame size is equal to requested fetch size
    this->moreFrames = (frame.getSize() == this->fetchSize);
    if (this->moreFrames) {
      this->adjustFetchSize(frame.getSizeInBytes());
    }

    return true;
  } else {
    return false;
  }
}

void Cask::CdapOdbc::QueryDataReader::tryLoadFrameAsync() {
  if (this->moreFrames) {
    this->loadFrameAsync();
  }
}

pplx::task<bool>& Cask::CdapOdbc::QueryDataReader::loadFrameAsync() {
  auto task = pplx::task<bool>([this]() {
    TRACE(L"Started loading new frame\n");
    return this->loadFrame();
  });

  this->tasks.emplace_back(std::move(task));
  return this->tasks.back();
}

Cask::CdapOdbc::QueryDataReader::QueryDataReader(QueryCommand* command)
  : queryCommand(command)
  , currentRowIndex(-1)
  , moreFrames(true)
  , firstLoad(true)
  , fetchSize(FETCH_SIZE) {
  this->schema = this->queryCommand->loadSchema();
}

bool Cask::CdapOdbc::QueryDataReader::read() {
  bool result = false;
  if (this->queryCommand->getHasData()) {
    if (this->firstLoad) {
      this->firstLoad = false;
      result = this->loadDataBegin();
    } else {
      this->checkTasksForExceptions();
      ++this->currentRowIndex;
      if (this->isFrameFetched()) {
        result = this->loadDataContinue();
      } else {
        result = true;
      }
    }
  }

  // Yield some time to other threads 
  pplx::wait(0);
  return result;
}

void Cask::CdapOdbc::QueryDataReader::getColumnValue(const ColumnBinding& binding) const {
  const auto& row = this->getFrame().getRows().at(this->currentRowIndex);
  auto& value = row.at(L"columns").as_array().at(binding.getColumnNumber() - 1);
  
  if (value.is_null()) {
    this->fetchNull(binding);
  } else {
    this->fetchValue(value, binding);
  }
}

short Cask::CdapOdbc::QueryDataReader::getColumnCount() const {
  return static_cast<short>(this->schema.size());
}

std::unique_ptr<Cask::CdapOdbc::ColumnInfo> Cask::CdapOdbc::QueryDataReader::getColumnInfo(short columnNumber) const {
  auto& columnDesc = this->schema[columnNumber - 1];
  return std::make_unique<ColumnInfo>(columnDesc);
}

bool Cask::CdapOdbc::QueryDataReader::canReadFast() const {
  if (this->queryCommand->getHasData()) {
    if (this->firstLoad) {
      // For the first call of fetch no data yet
      // Needs roundtrip to server
      return false;
    } else {
      if (this->currentRowIndex + 1 == this->getFrame().getSize()) {
        // End of batch - needs the next one
        return false;
      } else {
        return true;
      }
    }
  } else {
    return true;
  }
}
