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
#include "QueryCommand.h"
#include "Connection.h"
#include "QueryDataReader.h"
#include "QuerySELECT1DataReader.h"
#include "CdapException.h"
#include "QueryBuilder.h"

Cask::CdapOdbc::QueryCommand::QueryCommand(Connection* connection, const std::wstring& query)
  : Command(connection)
  , query(query) {
}

std::unique_ptr<Cask::CdapOdbc::DataReader> Cask::CdapOdbc::QueryCommand::executeReader() {
  /* 'SELECT 1' query is executed locally */
  if (_wcsicmp(this->query.c_str(), L"select 1") == 0) {
    return std::make_unique<QuerySELECT1DataReader>();
  }

  if (this->queryHandle.size() == 0) {
    QueryBuilder queryBuilder(this->query);
    this->queryHandle = this->getConnection()->getExploreClient().execute(queryBuilder.toString());
  }

  auto status = this->getConnection()->getExploreClient().getQueryStatus(this->queryHandle);

  // Synchronous query execution
  while (status.getOperationStatus() == OperationStatus::INITIALIZED ||
    status.getOperationStatus() == OperationStatus::PENDING ||
    status.getOperationStatus() == OperationStatus::RUNNING) {
    status = this->getConnection()->getExploreClient().getQueryStatus(this->queryHandle);
  }

  switch (status.getOperationStatus()) {
    case OperationStatus::FINISHED:
      this->hasData = status.hasResults();
      break;
    case OperationStatus::UNKNOWN:
    case OperationStatus::ERROR:
      throw CdapException(L"An error occured during query execution.");
    case OperationStatus::CANCELED:
      throw CdapException(L"Query canceled.");
    case OperationStatus::CLOSED:
      throw CdapException(L"Query closed.");
  }

  return std::make_unique<QueryDataReader>(this);
}

Cask::CdapOdbc::QueryResult Cask::CdapOdbc::QueryCommand::loadRows(int rows) {
  return this->getConnection()->getExploreClient().getQueryResult(this->queryHandle, rows);
}

std::vector<Cask::CdapOdbc::ColumnDesc> Cask::CdapOdbc::QueryCommand::loadSchema() {
  return this->getConnection()->getExploreClient().getQuerySchema(this->queryHandle);
}
