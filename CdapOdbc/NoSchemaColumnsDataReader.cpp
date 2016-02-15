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
#include "NoSchemaColumnsDataReader.h"

Cask::CdapOdbc::NoSchemaColumnsDataReader::NoSchemaColumnsDataReader(const std::wstring& tableName, const QueryResult& queryResult)
  : tableName(tableName)
  , queryResult(queryResult)
  , currentRowIndex(-1) {
}

bool Cask::CdapOdbc::NoSchemaColumnsDataReader::read() {
  ++this->currentRowIndex;
  return this->currentRowIndex < 3;
}

void Cask::CdapOdbc::NoSchemaColumnsDataReader::getColumnValue(const ColumnBinding& binding) {
  static std::array<const wchar_t*, 3> names = { L"ts", L"headers", L"body" };

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
			if (binding.getTargetType() == SQL_WCHAR) {
				this->fetchWVarchar(names[this->currentRowIndex], binding);
			}
			else {
				this->fetchVarchar(names[this->currentRowIndex], binding);
			}
      break;
    case 5: // DATA_TYPE
    case 14: // SQL_DATA_TYPE 
      this->fetchSmallint(SQL_CHAR, binding);
      break;
    case 6: // TYPE_NAME 
      this->fetchVarchar(L"string", binding);
      break;
    case 7: // COLUMN_SIZE 
      this->fetchInt(std::numeric_limits<std::int32_t>::max(), binding);
      break;
    case 8: // BUFFER_LENGTH 
      this->fetchInt(std::numeric_limits<std::int32_t>::max(), binding);
      break;
    case 10: // NUM_PREC_RADIX 
      this->fetchSmallint(0, binding);
      break;
    case 11: // NULLABLE
      this->fetchSmallint(SQL_NULLABLE, binding);
      break;
    case 17: // ORDINAL_POSITION 
      this->fetchInt(this->currentRowIndex + 1, binding);
      break;
    case 18: // IS_NULLABLE 
      this->fetchVarchar(L"YES", binding);
      break;
  }
}

short Cask::CdapOdbc::NoSchemaColumnsDataReader::getColumnCount() const {
  return 18;
}

std::unique_ptr<Cask::CdapOdbc::ColumnInfo> Cask::CdapOdbc::NoSchemaColumnsDataReader::getColumnInfo(short columnNumber) const {
  return std::unique_ptr<ColumnInfo>();
}
