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
#include "ColumnsDataReader.h"

namespace {
  SQLSMALLINT getDataType(const web::json::value& value) {
    std::wstring typeName;
    if (value.is_string()) {
      typeName = value.as_string();
    } else if (value.is_array()) {
      typeName = value.as_array().at(0).as_string();
    }

    if (typeName == L"boolean") {
      return SQL_BIT;
    } else if (typeName == L"bytes") {
      return SQL_BINARY;
    } else if (typeName == L"double") {
      return SQL_DOUBLE;
    } else if (typeName == L"float") {
      return SQL_REAL;
    } else if (typeName == L"int") {
      return SQL_INTEGER;
    } else if (typeName == L"long") {
      return SQL_BIGINT;
    } else if (typeName == L"string") {
      return SQL_CHAR;
    } else {
      return SQL_UNKNOWN_TYPE;
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

  SQLINTEGER getColumnSize(const web::json::value& value) {
    switch (getDataType(value)) {
      case SQL_CHAR:
      case SQL_BINARY:
        return std::numeric_limits<std::int32_t>::max();
      default:
        return 0;
    }
  }

  SQLINTEGER getBufferLength(const web::json::value& value) {
    switch (getDataType(value)) {
      case SQL_BIT:
        return sizeof(char);
      case SQL_BINARY:
      case SQL_CHAR:
        return std::numeric_limits<std::int32_t>::max();
      case SQL_DOUBLE:
        return sizeof(double);
      case SQL_REAL:
        return sizeof(float);
      case SQL_INTEGER:
        return sizeof(std::int32_t);
      case SQL_BIGINT:
        return sizeof(std::int64_t);
      default:
        return 0;
    }
  }

  SQLSMALLINT getRadix(const web::json::value& value) {
    switch (getDataType(value)) {
      case SQL_INTEGER:
        return std::numeric_limits<std::int32_t>::radix;
      case SQL_DOUBLE:
        return std::numeric_limits<double>::radix;
      default:
        return 0;
    }
  }

  SQLSMALLINT getIsNull(const web::json::value& value) {
    if (value.is_array() && value.as_array().at(1).as_string() == L"null") {
      return SQL_NULLABLE;
    } else {
      return SQL_NO_NULLS;
    }
  }
}

Cask::CdapOdbc::ColumnsDataReader::ColumnsDataReader(const std::wstring& tableName, const QueryResult& queryResult)
  : tableName(tableName)
  , queryResult(queryResult)
  , currentRowIndex(-1) {
}

bool Cask::CdapOdbc::ColumnsDataReader::read() {
  ++this->currentRowIndex;
  return this->currentRowIndex < queryResult.getSize();
}

void Cask::CdapOdbc::ColumnsDataReader::getColumnValue(const ColumnBinding& binding) {
  auto& record = this->queryResult.getRows().at(this->currentRowIndex);
  std::wstring name;
  std::wstring typeName;
  std::wstring no = L"NO";
  std::wstring yes = L"YES";
  SQLSMALLINT radix = 0;

  switch (binding.getColumnNumber()) {
    case 1: // TABLE_CAT 
    case 2: // TABLE_SCHEM 
    case 9: // DECIMAL_DIGITS 
    case 12: // REMARKS 
    case 13: // COLUMN_DEF 
    case 15: // SQL_DATETIME_SUB 
    case 16: // CHAR_OCTET_LENGTH 
      this->fetchNull(binding);
      break;
    case 3: // TABLE_NAME 
      this->fetchVarchar(this->tableName.c_str(), binding);
      break;
    case 4: // COLUMN_NAME 
      name = record.at(L"name").as_string();
      this->fetchVarchar(name.c_str(), binding);
      break;
    case 5: // DATA_TYPE
    case 14: // SQL_DATA_TYPE 
      this->fetchSmallint(getDataType(record.at(L"type")), binding);
      break;
    case 6: // TYPE_NAME 
      typeName = getTypeName(record.at(L"type"));
      this->fetchVarchar(typeName.c_str(), binding);
      break;
    case 7: // COLUMN_SIZE 
      this->fetchInt(getColumnSize(record.at(L"type")), binding);
      break;
    case 8: // BUFFER_LENGTH 
      this->fetchInt(getBufferLength(record.at(L"type")), binding);
      break;
    case 10: // NUM_PREC_RADIX 
      radix = getRadix(record.at(L"type"));
      if (radix > 0) {
        this->fetchSmallint(radix, binding);
      } else {
        this->fetchNull(binding);
      }

      break;
    case 11: // NULLABLE
      this->fetchSmallint(getIsNull(record.at(L"type")), binding);
      break;
    case 17: // ORDINAL_POSITION 
      this->fetchInt(this->currentRowIndex + 1, binding);
      break;
    case 18: // IS_NULLABLE 
      this->fetchVarchar((getIsNull(record.at(L"type")) == SQL_NO_NULLS) ? L"NO" : L"YES", binding);
      break;
  }
}

short Cask::CdapOdbc::ColumnsDataReader::getColumnCount() const {
  return 18;
}

std::unique_ptr<Cask::CdapOdbc::ColumnInfo> Cask::CdapOdbc::ColumnsDataReader::getColumnInfo(short columnNumber) const {
  return std::unique_ptr<ColumnInfo>();
}
