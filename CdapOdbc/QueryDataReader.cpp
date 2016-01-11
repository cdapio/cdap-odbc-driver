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
#include "QueryDataReader.h"
#include "QueryCommand.h"

////namespace {
////  const std::wstring* convertPattern(const std::wstring* pattern) {
////    // '%' means all in ODBC and NULL means all in Explore
////    // Convert '%' to NULL
////    if (pattern && *pattern == L"%") {
////      return nullptr;
////    } else {
////      return pattern;
////    }
////  }
////}

bool Cask::CdapOdbc::QueryDataReader::loadData() {
  //if (this->moreData) {
  //  this->queryResult = this->connection->getExploreClient().getQueryResult(this->queryHandle, this->fetchSize);
  //  this->moreData = (this->queryResult.getSize() == this->fetchSize);
  //  this->currentRowIndex = 0;
  //  // True if there are some records 
  //  return (this->queryResult.getSize() > 0);
  //} else {
  //  return false;
  //}
  return false;
}

Cask::CdapOdbc::QueryDataReader::QueryDataReader(QueryCommand* command) {
}

bool Cask::CdapOdbc::QueryDataReader::read() {
  ////bool result = false;
  ////bool dataLoaded = (this->queryResult.getSize() > 0);
  ////if (dataLoaded) {
  ////  ++this->currentRowIndex;
  ////  if (this->currentRowIndex == this->queryResult.getSize()) {
  ////    // Need more data
  ////    result = this->loadData();
  ////  } else {
  ////    result = true;
  ////  }
  ////} else {
  ////  // Get 1st row - no data yet
  ////  result = this->loadData();
  ////}

  return false;
}

void Cask::CdapOdbc::QueryDataReader::getColumnValue(const ColumnBinding& binding) {
}

short Cask::CdapOdbc::QueryDataReader::getColumnCount() const {
  return static_cast<short>(this->queryCommand->getColumnCount());
}
