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
#include "SchemaProperties.h"

void Cask::CdapOdbc::TablesDataReader::filterDatasets() {
  std::vector<web::json::value> newRows;
  auto& rows = this->datasets.getRows();
  for (auto& row : rows) {
    // TODO: once CDAP 3.4 is issued, schema retreival must be updated:
    // All filesets created by HYDRATOR plugin define schema as dataset property (CDAP-5322)
    // For now we use heuristic approach: "schema" or "explore.table.property.avro.schema.literal"
    // See CDAP-5516
    auto& props = row.at(L"properties");
    if (props.has_field(PROP_SCHEMA) || props.has_field(PROP_SCHEMA_ALT)) {
      newRows.push_back(row);
    }
  }
 
  this->datasets = QueryResult(web::json::value::array(newRows));
}

std::wstring Cask::CdapOdbc::TablesDataReader::getTableName() const {
  std::wstring originalName;
  std::wstring hiveName;

  if (this->currentRowIndex < this->streams.getSize()) {
    // get stream name
    originalName = this->streams.getRows().at(this->currentRowIndex).at(L"name").as_string();
    hiveName = L"stream_" + originalName;
  } else {
    // get dataset name
    int rowIndex = this->currentRowIndex - this->streams.getSize();
    originalName = this->datasets.getRows().at(rowIndex).at(L"name").as_string();
    hiveName = L"dataset_" + originalName;
  }

  std::replace(hiveName.begin(), hiveName.end(), '-', '_');
  this->tableNames->insert_or_assign(hiveName, originalName);
  return hiveName;
}

Cask::CdapOdbc::TablesDataReader::TablesDataReader(const QueryResult& streams, const QueryResult& datasets, std::map<std::wstring, std::wstring>& tableNames)
  : streams(streams)
  , datasets(datasets)
  , currentRowIndex(-1)
  , tableNames(&tableNames) {
  this->filterDatasets();
}

bool Cask::CdapOdbc::TablesDataReader::read() {
  ++this->currentRowIndex;
  return this->currentRowIndex < (streams.getSize() + datasets.getSize());
}

void Cask::CdapOdbc::TablesDataReader::getColumnValue(const ColumnBinding& binding) const {
  std::wstring name;
  switch (binding.getColumnNumber()) {
    case 1: // TABLE_CAT 
    case 2: // TABLE_SCHEM 
    case 5: // REMARKS 
      this->fetchNull(binding);
      break;
    case 3: // TABLE_NAME 
      name = this->getTableName();
      if (binding.getTargetType() == SQL_WCHAR) {
        this->fetchWVarchar(name.c_str(), binding);
      } else {
        this->fetchVarchar(name.c_str(), binding);
      }
      break;
    case 4: // TABLE_TYPE
      if (binding.getTargetType() == SQL_WCHAR) {
        this->fetchWVarchar(L"TABLE", binding);
      } else {
        this->fetchVarchar(L"TABLE", binding);
      }
      break;
  }
}

short Cask::CdapOdbc::TablesDataReader::getColumnCount() const {
  return 5;
}

std::unique_ptr<Cask::CdapOdbc::ColumnInfo> Cask::CdapOdbc::TablesDataReader::getColumnInfo(short columnNumber) const {
  return std::unique_ptr<ColumnInfo>();
}

bool Cask::CdapOdbc::TablesDataReader::canReadFast() const {
  return true;
}
