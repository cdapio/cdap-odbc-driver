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
#include "String.h"

namespace {
  std::wstring makeStreamName(const std::wstring& streamName) {
    return streamName.substr(7);
  }

  std::wstring makeDatasetName(const std::wstring& datasetName) {
    return datasetName.substr(8);
  }

  bool isStream(const std::wstring& name) {
    return name.find(L"stream_") == 0;
  }
}

Cask::CdapOdbc::ColumnsCommand::ColumnsCommand(Connection* connection, const std::wstring& tableName)
  : Command(connection)
  , tableName(tableName) {
}

std::unique_ptr<Cask::CdapOdbc::DataReader> Cask::CdapOdbc::ColumnsCommand::executeReader() {
  if (isStream(this->tableName)) {
    auto streamName = makeStreamName(this->tableName);
    auto queryResult = this->getConnection()->getExploreClient().getStreamFields(streamName);
    bool noSchema = (queryResult.getSize() == 1) && (queryResult.getRows().at(0).at(L"name").as_string() == L"body");
    if (noSchema) {
      return std::make_unique<NoSchemaColumnsDataReader>(this->tableName, queryResult);
    } else {
      return std::make_unique<ColumnsDataReader>(this->tableName, queryResult);
    }
  } else {
    auto datasetName = makeDatasetName(this->tableName);
    auto queryResult = this->getConnection()->getExploreClient().getDatasetFields(datasetName);
    return std::make_unique<ColumnsDataReader>(this->tableName, queryResult);
  }
}
