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

bool Cask::CdapOdbc::QueryDataReader::loadData() {
  if (this->moreData) {
    this->queryResult = this->queryCommand->loadRows(FETCH_SIZE);
    ++this->fetchCount;

    // If number of rows is equal to fetch size
    // assume there are more rows
    this->moreData = (this->queryResult.getSize() == FETCH_SIZE);
    this->currentRowIndex = 0;
    return (this->queryResult.getSize() > 0);
  } else {
    return false;
  }
}

Cask::CdapOdbc::QueryDataReader::QueryDataReader(QueryCommand* command)
  : queryCommand(command)
  , fetchCount(0)
  , currentRowIndex(-1)
  , moreData(true) {
  this->schema = this->queryCommand->loadSchema();
}

bool Cask::CdapOdbc::QueryDataReader::read() {
  if (this->queryCommand->getHasData()) {
    if (this->fetchCount == 0) {
      return this->loadData();
    } else {
      ++this->currentRowIndex;
      if (this->currentRowIndex == this->queryResult.getSize()) {
        return this->loadData();
      } else {
        return true;
      }
    }
  } else {
    return false;
  }
}

void Cask::CdapOdbc::QueryDataReader::getColumnValue(const ColumnBinding& binding) {
  auto& row = this->queryResult.getRows().at(this->currentRowIndex);
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
    if (this->fetchCount == 0) {
      // For the first call of fetch no data yet
      // Needs roundtrip to server
      return false;
    } else {
      if (this->currentRowIndex + 1 == this->queryResult.getSize()) {
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
