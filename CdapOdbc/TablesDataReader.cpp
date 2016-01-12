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
#include "TablesDataReader.h"
#include "String.h"

Cask::CdapOdbc::TablesDataReader::TablesDataReader(const QueryResult& queryResult)
  : queryResult(queryResult)
  , currentRowIndex(-1) {
}

bool Cask::CdapOdbc::TablesDataReader::read() {
  ++this->currentRowIndex;
  return this->currentRowIndex < queryResult.getSize();
}

void Cask::CdapOdbc::TablesDataReader::getColumnValue(const ColumnBinding& binding) {
  std::wstring name;
  switch (binding.getColumnNumber()) {
    case 1: // TABLE_CAT 
    case 2: // TABLE_SCHEM 
    case 5: // REMARKS 
      this->fetchNull(binding);
      break;
    case 3: // TABLE_NAME 
      name = String::makeTableName(this->queryResult.getRows().at(this->currentRowIndex).at(L"name").as_string());
      this->fetchString(name.c_str(), binding);
      break;
    case 4: // TABLE_TYPE 
      this->fetchString(L"TABLE", binding);
      break;
  }
}

short Cask::CdapOdbc::TablesDataReader::getColumnCount() const {
  return 5;
}
