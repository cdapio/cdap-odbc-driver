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
#include "ColumnsCommand.h"
#include "Connection.h"
#include "ColumnsDataReader.h"
#include "NoSchemaColumnsDataReader.h"
#include "SchemaProperties.h"

namespace {
  bool isStream(const std::wstring& name) {
    return name.find(L"stream_") == 0;
  }
}

Cask::CdapOdbc::ColumnsCommand::ColumnsCommand(Connection* connection, const std::wstring& realTableName, const std::wstring& hiveTableName)
  : Command(connection)
  , realTableName(realTableName)
  , hiveTableName(hiveTableName) {
}

std::unique_ptr<Cask::CdapOdbc::DataReader> Cask::CdapOdbc::ColumnsCommand::executeReader() {
  if (isStream(this->hiveTableName)) {
    auto root = this->getConnection()->getExploreClient().getStreamFields(this->realTableName);
    QueryResult queryResult(root.getValue().at(L"format").at(L"schema").at(L"fields"));
    bool noSchema = (queryResult.getSize() == 1) && (queryResult.getRows().at(0).at(L"name").as_string() == L"body");
    if (noSchema) {
      return std::make_unique<NoSchemaColumnsDataReader>(this->hiveTableName, queryResult);
    } else {
      return std::make_unique<ColumnsDataReader>(this->hiveTableName, queryResult);
    }
  } else {
    auto root = this->getConnection()->getExploreClient().getDatasetFields(this->realTableName);
    const auto& props = root.getValue().at(L"spec").at(L"properties");
    web::json::value value;
    if (props.has_field(PROP_SCHEMA)) {
      value = props.at(PROP_SCHEMA);
    } else if (props.has_field(PROP_SCHEMA_ALT)) {
      value = props.at(PROP_SCHEMA_ALT);
    }
    
    std::error_code error;
    auto schema = web::json::value::parse(value.as_string(), error);
    if (error) {
      throw std::system_error(error);
    }

    QueryResult queryResult(schema.at(L"fields"));
    return std::make_unique<ColumnsDataReader>(this->hiveTableName, queryResult);
  }
}