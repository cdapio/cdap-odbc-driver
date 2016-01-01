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
#include "Statement.h"
#include "Connection.h"
#include "StillExecutingException.h"
#include "NoDataException.h"
#include "String.h"

using namespace Cask::CdapOdbc;

namespace {
  void split(const std::wstring& str, wchar_t delim, std::vector<std::wstring>& tokens) {
    std::wstringstream stream(str);
    std::wstring item;
    while (std::getline(stream, item, delim)) {
      tokens.push_back(item);
    }
  }
}

void Cask::CdapOdbc::Statement::throwStateError() {
  throw std::logic_error("Wrong statement state.");
}

void Cask::CdapOdbc::Statement::openQuery() {
  auto status = this->connection->getExploreClient().getQueryStatus(this->queryHandle);
  switch (status.getOperationStatus()) {
    case OperationStatus::FINISHED:
      this->querySchema = this->connection->getExploreClient().getQuerySchema(this->queryHandle);
      if (status.hasResults()) {
        this->moreData = true;
      }

      break;
    case OperationStatus::INITIALIZED:
    case OperationStatus::PENDING:
    case OperationStatus::RUNNING:
      throw StillExecutingException("The query is still executing.");
    case OperationStatus::UNKNOWN:
    case OperationStatus::ERROR:
      throw std::exception("An error occured during query execution.");
    case OperationStatus::CANCELED:
      throw std::exception("Query canceled.");
    case OperationStatus::CLOSED:
      throw std::exception("Query closed.");
  }
}

bool Cask::CdapOdbc::Statement::loadData() {
  if (this->moreData) {
    this->queryResult = this->connection->getExploreClient().getQueryResult(this->queryHandle, this->fetchSize);
    this->moreData = (this->queryResult.getSize() == this->fetchSize);
    this->currentRowIndex = 0;
    // True if there are some records 
    return (this->queryResult.getSize() > 0);
  } else {
    return false;
  }
}

bool Cask::CdapOdbc::Statement::getNextResults() {
  bool result = false;
  bool dataLoaded = (this->queryResult.getSize() > 0);
  if (dataLoaded) {
    ++this->currentRowIndex;
    if (this->currentRowIndex == this->queryResult.getSize()) {
      // Need more data
      result = this->loadData();
    } else {
      result = true;
    }
  } else {
    // Get 1st row - no data yet
    result = this->loadData();
  }

  return result;
}

void Cask::CdapOdbc::Statement::fetchRow() {
}

Cask::CdapOdbc::Statement::Statement(Connection* connection, SQLHSTMT handle)
  : state(State::INITIAL)
  , connection(connection)
  , handle(handle)
  , fetchSize(50)
  , currentRowIndex(0)
  , moreData(false) {
  assert(connection);
  assert(handle);
}

void Cask::CdapOdbc::Statement::addColumnBinding(const ColumnBinding& binding) {
  if (this->state != State::INITIAL && this->state != State::OPEN) {
    this->throwStateError();
  }

  auto it = std::find_if(
    this->columnBindings.begin(),
    this->columnBindings.end(),
    [binding](auto& b) { return b.getColumnNumber() == binding.getColumnNumber(); });
  if (it != this->columnBindings.end()) {
    // Replace existing binding. 
    *it = binding;
  } else {
    // Add new binding. 
    this->columnBindings.push_back(binding);
  }
}

void Cask::CdapOdbc::Statement::removeColumnBinding(SQLUSMALLINT columnNumber) {
  if (this->state != State::INITIAL && this->state != State::OPEN) {
    this->throwStateError();
  }

  auto it = std::find_if(
    this->columnBindings.begin(), 
    this->columnBindings.end(), 
    [columnNumber](auto& b) { return b.getColumnNumber() == columnNumber; });
  if (it == this->columnBindings.end()) {
    throw std::invalid_argument("columnNumber");
  }

  this->columnBindings.erase(it);
}

void Cask::CdapOdbc::Statement::getCatalogs() {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->queryHandle = this->connection->getExploreClient().getCatalogs();
  this->state = State::OPEN;
}

void Cask::CdapOdbc::Statement::getSchemas(const std::wstring& catalog, const std::wstring& schemaPattern) {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->queryHandle = this->connection->getExploreClient().getSchemas(catalog, schemaPattern);
  this->state = State::OPEN;
}

void Cask::CdapOdbc::Statement::getTables(
  const std::wstring& catalog, 
  const std::wstring& schemaPattern, 
  const std::wstring& tableNamePattern, 
  const std::wstring& tableTypes) {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  std::vector<std::wstring> tableTypeArgs;
  String::split(tableTypes, L',', tableTypeArgs);
  this->queryHandle = this->connection->getExploreClient().getTables(catalog, schemaPattern, tableNamePattern, tableTypeArgs);
  this->state = State::OPEN;
}

void Cask::CdapOdbc::Statement::fetch() {
  if (this->state != State::OPEN && this->state != State::FETCH) {
    this->throwStateError();
  }

  if (this->state == State::OPEN) {
    this->openQuery();
    this->state = State::FETCH;
  } 
  
  if (this->state == State::FETCH) {
    if (this->getNextResults()) {
      this->fetchRow();
    } else {
      this->state = State::CLOSED;
      throw NoDataException("No more data in the query.");
    }
  }
}

void Cask::CdapOdbc::Statement::reset() {
  if (this->state != State::INITIAL) {
    if (this->state == State::OPEN || this->state == State::FETCH) {
      this->connection->getExploreClient().closeQuery(this->queryHandle);
    }

    this->queryHandle.clear();
    this->currentRowIndex = 0;
    this->moreData = false;
    this->state = State::INITIAL;
  }
}

void Cask::CdapOdbc::Statement::unbindColumns() {
  if (this->state != State::INITIAL && this->state != State::OPEN) {
    this->throwStateError();
  }

  this->columnBindings.clear();
}

void Cask::CdapOdbc::Statement::resetParameters() {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }
}
