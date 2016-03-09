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
#include "TablesCommand.h"
#include "Connection.h"
#include "TablesDataReader.h"

Cask::CdapOdbc::TablesCommand::TablesCommand(Connection* connection, std::map<std::wstring, std::wstring>& tableNames)
  : Command(connection)
  , tableNames(&tableNames) {
}

std::unique_ptr<Cask::CdapOdbc::DataReader> Cask::CdapOdbc::TablesCommand::executeReader() {
  auto streams = this->getConnection()->getExploreClient().getStreams();
  auto datasets = this->getConnection()->getExploreClient().getDatasets();
  return std::make_unique<TablesDataReader>(streams, datasets, *tableNames);
}
