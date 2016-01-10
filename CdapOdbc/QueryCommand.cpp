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
#include "QueryDataReader.h"

Cask::CdapOdbc::QueryCommand::QueryCommand(Connection* connection)
  : Command(connection) {
}

std::unique_ptr<Cask::CdapOdbc::DataReader> Cask::CdapOdbc::QueryCommand::executeReader() {

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

  return std::make_unique<QueryDataReader>();
}
