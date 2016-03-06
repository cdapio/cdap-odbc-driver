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
#include "TypesDataReader.h"

Cask::CdapOdbc::TypesDataReader::TypesDataReader()
  : currentRowIndex(-1) {
}

bool Cask::CdapOdbc::TypesDataReader::read() {
  ++this->currentRowIndex;
  return this->currentRowIndex < 1;
}

void Cask::CdapOdbc::TypesDataReader::getColumnValue(const ColumnBinding& binding) {
  switch (binding.getColumnNumber()) {
    case 1: // TYPE_NAME
      this->fetchVarchar(L"string", binding);
      break;
    case 2: // DATA_TYPE 
    case 16: // SQL_DATA_TYPE   
      this->fetchSmallint(SQL_VARCHAR, binding);
      break;
    case 3: // COLUMN_SIZE  
      this->fetchSmallint(2000, binding);
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
      this->fetchNull(binding);
      break;
    case 6: // CREATE_PARAMS 
      this->fetchVarchar(L"length", binding);
      break;
    case 7: // NULLABLE  
      this->fetchSmallint(SQL_NULLABLE, binding);
      break;
    case 8: // CASE_SENSITIVE
      this->fetchSmallint(SQL_TRUE, binding);
      break;
    case 9: // SEARCHABLE 
      this->fetchSmallint(SQL_SEARCHABLE, binding);
      break;
    case 11: // FIXED_PREC_SCALE   
      this->fetchSmallint(SQL_FALSE, binding);
      break;
  }
}

short Cask::CdapOdbc::TypesDataReader::getColumnCount() const {
  return 19;
}

std::unique_ptr<Cask::CdapOdbc::ColumnInfo> Cask::CdapOdbc::TypesDataReader::getColumnInfo(short columnNumber) const {
  return std::unique_ptr<ColumnInfo>();
}

bool Cask::CdapOdbc::TypesDataReader::canReadFast() const {
  return true;
}
