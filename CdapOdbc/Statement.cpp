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
#include "MetadataColumnMapper.h"
#include "Encoding.h"

using namespace Cask::CdapOdbc;

namespace {
  const std::wstring* convertPattern(const std::wstring* pattern) {
    // '%' means all in ODBC and NULL means all in Explore
    // Convert '%' to NULL
    if (pattern && *pattern == L"%") {
      return nullptr;
    } else {
      return pattern;
    }
  }

  void copyString(const std::wstring* str, const ColumnBinding& binding) {

    if (str) {
      std::string ansiStr = Encoding::toUtf8(*str);
      size_t maxLength = static_cast<size_t>(binding.getBufferLength()) - 1;
      size_t size = (ansiStr.size() < maxLength) ? ansiStr.size() : maxLength;
      if (binding.getTargetValuePtr()) {
        char* outString = static_cast<char*>(binding.getTargetValuePtr());
        auto it = stdext::make_checked_array_iterator<char*>(outString, size);
        std::copy(ansiStr.begin(), ansiStr.begin() + size, it);
        outString[size] = 0;
      }

      if (binding.getStrLenOrInd()) {
        if (size <= maxLength) {
          *binding.getStrLenOrInd() = size;
        } else {
          *binding.getStrLenOrInd() = SQL_NO_TOTAL;
        }
      }
    } else {
      if (binding.getTargetValuePtr() && binding.getBufferLength() > 0) {
        char* outString = static_cast<char*>(binding.getTargetValuePtr());
        outString[0] = 0;
      }

      if (binding.getStrLenOrInd()) {
        *binding.getStrLenOrInd() = SQL_NULL_DATA;
      }
    }
  }

  SQLSMALLINT getDataType(const web::json::value& value) {
    std::wstring typeName;
    if (value.is_string()) {
      typeName = value.as_string();
    } else if (value.is_array()) {
      typeName = value.as_array().at(0).as_string();
    }

    if (typeName == L"string") {
      return SQL_VARCHAR;
    } else if (typeName == L"int") {
      return SQL_INTEGER;
    } else if (typeName == L"double") {
      return SQL_DOUBLE;
    } else {
      return SQL_UNKNOWN_TYPE;
    }
  }

  SQLSMALLINT getIsNull(const web::json::value& value) {
    if (value.is_array() && value.as_array().at(1).as_string() == L"null") {
      return SQL_NULLABLE;
    } else {
      return SQL_NO_NULLS;
    }
  }

  std::wstring getTypeName(const web::json::value& value) {
    std::wstring typeName;
    if (value.is_string()) {
      typeName = value.as_string();
    } else if (value.is_array()) {
      typeName = value.as_array().at(0).as_string();
    }

    return typeName;
  }

  int getColumnSize(const web::json::value& value) {
    switch (getDataType(value)) {
      case SQL_VARCHAR:
        return std::numeric_limits<std::int32_t>::max();
      case SQL_INTEGER:
        return std::numeric_limits<std::int32_t>::digits;
      case SQL_DOUBLE:
        return std::numeric_limits<double>::digits;
      default:
        return 0;
    }
  }

  int getBufferLength(const web::json::value& value) {
    switch (getDataType(value)) {
      case SQL_VARCHAR:
        return std::numeric_limits<std::int32_t>::max();
      case SQL_INTEGER:
        return sizeof(std::int32_t);
      case SQL_DOUBLE:
        return sizeof(double);
      default:
        return 0;
    }
  }

  int getRadix(const web::json::value& value) {
    switch (getDataType(value)) {
      case SQL_INTEGER:
        return std::numeric_limits<std::int32_t>::radix;
      case SQL_DOUBLE:
        return std::numeric_limits<double>::radix;
      default:
        return 0;
    }
  }
}

void Cask::CdapOdbc::Statement::throwStateError() {
  throw std::logic_error("Wrong statement state.");
}

void Cask::CdapOdbc::Statement::openQuery() {
  switch (this->requestType) {
    case RequestType::SCHEMAS:
      this->state = State::CLOSED;
      throw NoDataException("No more data in the query.");
    case RequestType::TABLES:
      this->queryResult = this->connection->getExploreClient().getStreams();
      break;
    case RequestType::TYPES:
      break;
    case RequestType::COLUMNS:
      this->queryResult = this->connection->getExploreClient().getStreamFields(this->tableName);
      break;

    ////case RequestType::DATA:
    ////  auto status = this->connection->getExploreClient().getQueryStatus(this->queryHandle);
    ////  switch (status.getOperationStatus()) {
    ////    case OperationStatus::FINISHED:
    ////      if (status.hasResults()) {
    ////        this->moreData = true;
    ////        auto querySchema = this->connection->getExploreClient().getQuerySchema(this->queryHandle);
    ////        this->mapper->setColumnBindings(this->columnBindings);
    ////        this->mapper->setColumnDescs(querySchema);
    ////      }

    ////      break;
    ////    case OperationStatus::INITIALIZED:
    ////    case OperationStatus::PENDING:
    ////    case OperationStatus::RUNNING:
    ////      throw StillExecutingException("The query is still executing.");
    ////    case OperationStatus::UNKNOWN:
    ////    case OperationStatus::ERROR:
    ////      throw std::exception("An error occured during query execution.");
    ////    case OperationStatus::CANCELED:
    ////      throw std::exception("Query canceled.");
    ////    case OperationStatus::CLOSED:
    ////      throw std::exception("Query closed.");
    ////  }

    ////  break;
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
  ++this->currentRowIndex;
  if (this->requestType == RequestType::TABLES || this->requestType == RequestType::COLUMNS) {
    return (this->currentRowIndex <= this->queryResult.getSize());
  } else if (this->requestType == RequestType::TYPES) {
    return this->currentRowIndex <= 1;
  }

  ////bool result = false;
  ////bool dataLoaded = (this->queryResult.getSize() > 0);
  ////if (dataLoaded) {
  ////  ++this->currentRowIndex;
  ////  if (this->currentRowIndex == this->queryResult.getSize()) {
  ////    // Need more data
  ////    result = this->loadData();
  ////  } else {
  ////    result = true;
  ////  }
  ////} else {
  ////  // Get 1st row - no data yet
  ////  result = this->loadData();
  ////}

  return false;
}

void Cask::CdapOdbc::Statement::fetchRow() {
  ////auto columns = this->queryResult.getRows().at(this->currentRowIndex).at(L"columns");
  ////this->mapper->map(columns);

  if (this->requestType == RequestType::TABLES) {
    std::wstring empty;
    std::wstring name;
    std::wstring type = L"TABLE";
    for (auto& item : this->columnBindings) {
      switch (item.getColumnNumber()) {
        case 1: // TABLE_CAT 
        case 2: // TABLE_SCHEM 
        case 5: // REMARKS 
          copyString(nullptr, item);
          break;
        case 3: // TABLE_NAME 
          name = this->queryResult.getRows().at(this->currentRowIndex - 1).at(L"name").as_string();
          copyString(&name, item);
          break;
        case 4: // TABLE_TYPE 
          copyString(&type, item);
          break;
      }
    }
  } else if (this->requestType == RequestType::TYPES) {
    std::wstring typeName = L"VARCHAR()";
    std::wstring createParams = L"length";
    for (auto& item : this->columnBindings) {
      switch (item.getColumnNumber()) {
        case 1: // TYPE_NAME
          copyString(&typeName, item);
          break;
        case 2: // DATA_TYPE 
        case 16: // SQL_DATA_TYPE   
          *(reinterpret_cast<SQLSMALLINT*>(item.getTargetValuePtr())) = SQL_VARCHAR;
        case 3: // COLUMN_SIZE  
          *(reinterpret_cast<SQLINTEGER*>(item.getTargetValuePtr())) = 2000;
          break;
        case 4: // LITERAL_PREFIX 
        case 5: // LITERAL_SUFFIX
        case 10: // UNSIGNED_ATTRIBUTE
        case 12: // AUTO_UNIQUE_VALUE 
        case 13: // LOCAL_TYPE_NAME 
        case 14: // MINIMUM_SCALE 
        case 15: // MAXIMUM_SCALE 
        case 17: // SQL_DATETIME_SUB 
        case 18: // NUM_PREC_RADIX 
        case 19: // INTERVAL_PRECISION 
          copyString(nullptr, item);
        case 6: // CREATE_PARAMS 
          copyString(&createParams, item);
          break;
        case 7: // NULLABLE  
          *(reinterpret_cast<SQLSMALLINT*>(item.getTargetValuePtr())) = SQL_NULLABLE;
          break;
        case 8: // CASE_SENSITIVE
          *(reinterpret_cast<SQLSMALLINT*>(item.getTargetValuePtr())) = SQL_TRUE;
          break;
        case 9: // SEARCHABLE 
          *(reinterpret_cast<SQLSMALLINT*>(item.getTargetValuePtr())) = SQL_SEARCHABLE;
          break;
        case 11: // FIXED_PREC_SCALE   
          *(reinterpret_cast<SQLSMALLINT*>(item.getTargetValuePtr())) = SQL_FALSE;
          break;
      }
    }
  } else if (this->requestType == RequestType::COLUMNS) {
    auto& record = this->queryResult.getRows().at(this->currentRowIndex - 1);
    std::wstring name;
    std::wstring typeName;
    std::wstring no = L"NO";
    std::wstring yes = L"YES";
    SQLSMALLINT radix = 0;
    for (auto &item : this->columnBindings) {
      switch (item.getColumnNumber()) {
        case 1: // TABLE_CAT 
        case 2: // TABLE_SCHEM 
        case 9: // DECIMAL_DIGITS 
        case 12: // REMARKS 
        case 13: // COLUMN_DEF 
        case 15: // SQL_DATETIME_SUB 
        case 16: // CHAR_OCTET_LENGTH 
          copyString(nullptr, item);
          break;
        case 3: // TABLE_NAME 
          copyString(&this->tableName, item);
          break;
        case 4: // COLUMN_NAME 
          name = record.at(L"name").as_string();
          copyString(&name, item);
          break;
        case 5: // DATA_TYPE
        case 14: // SQL_DATA_TYPE 
          *(reinterpret_cast<SQLSMALLINT*>(item.getTargetValuePtr())) = getDataType(record.at(L"type"));
          break;
        case 6: // TYPE_NAME 
          typeName = getTypeName(record.at(L"type"));
          copyString(&typeName, item);
          break;
        case 7: // COLUMN_SIZE 
          *(reinterpret_cast<SQLINTEGER*>(item.getTargetValuePtr())) = getColumnSize(record.at(L"type"));
          break;
        case 8: // BUFFER_LENGTH 
          *(reinterpret_cast<SQLINTEGER*>(item.getTargetValuePtr())) = getBufferLength(record.at(L"type"));
          break;
        case 10: // NUM_PREC_RADIX 
          radix = getRadix(record.at(L"type"));
          if (radix > 0) {
            *(reinterpret_cast<SQLSMALLINT*>(item.getTargetValuePtr())) = radix;
          } else {
            copyString(nullptr, item);
          }

          break;
        case 11: // NULLABLE
          *(reinterpret_cast<SQLSMALLINT*>(item.getTargetValuePtr())) = getIsNull(record.at(L"type"));
          break;
        case 17: // ORDINAL_POSITION 
          *(reinterpret_cast<SQLINTEGER*>(item.getTargetValuePtr())) = currentRowIndex;
          break;
        case 18: // IS_NULLABLE 
          if (getIsNull(record.at(L"type")) == SQL_NO_NULLS) {
            copyString(&no, item);
          } else {
            copyString(&yes, item);
          }

          break;
      }
    }
  }
}

void Cask::CdapOdbc::Statement::setupMetadataMapper() {
  this->mapper = std::make_unique<MetadataColumnMapper>();
}

void Cask::CdapOdbc::Statement::setupSimpleMapper() {
  this->mapper = std::make_unique<ColumnMapper>();
}

Cask::CdapOdbc::Statement::Statement(Connection* connection, SQLHSTMT handle)
  : state(State::INITIAL)
  , connection(connection)
  , handle(handle)
  , fetchSize(50)
  , currentRowIndex(0)
  , moreData(false)
  , requestType(RequestType::UNKNOWN) {
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

  this->requestType = RequestType::CATALOGS;
  this->state = State::OPEN;
}

void Cask::CdapOdbc::Statement::getSchemas(const std::wstring* catalog, const std::wstring* schemaPattern) {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->requestType = RequestType::SCHEMAS;
  this->state = State::OPEN;
}

void Cask::CdapOdbc::Statement::getTables(
  const std::wstring* catalog,
  const std::wstring* schemaPattern,
  const std::wstring* tableNamePattern,
  const std::wstring* tableTypes) {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->requestType = RequestType::TABLES;
  this->state = State::OPEN;
}

void Cask::CdapOdbc::Statement::getDataTypes() {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->requestType = RequestType::TYPES;
  this->state = State::OPEN;
}

void Cask::CdapOdbc::Statement::getColumns(const std::wstring& tableName) {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->requestType = RequestType::COLUMNS;
  this->tableName = tableName;
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
    this->queryResult = QueryResult();
    this->mapper.reset();
    this->currentRowIndex = 0;
    this->moreData = false;
    this->requestType = RequestType::UNKNOWN;
    this->tableName.clear();
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
