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
#include "QueryStatus.h"

using namespace Cask::CdapOdbc;

OperationStatus Cask::CdapOdbc::QueryStatus::parseStatus(const utility::string_t& value) {
  if (value == L"INITIALIZED") {
    return OperationStatus::INITIALIZED;
  } else if (value == L"RUNNING") {
    return OperationStatus::RUNNING;
  } else if (value == L"FINISHED") {
    return OperationStatus::FINISHED;
  } else if (value == L"CANCELED") {
    return OperationStatus::CANCELED;
  } else if (value == L"CLOSED") {
    return OperationStatus::CLOSED;
  } else if (value == L"ERROR") {
    return OperationStatus::ERROR;
  } else if (value == L"UNKNOWN") {
    return OperationStatus::UNKNOWN;
  } else if (value == L"PENDING") {
    return OperationStatus::PENDING;
  }

  throw std::invalid_argument("value");
}

Cask::CdapOdbc::QueryStatus::QueryStatus()
  : opStatus(OperationStatus::UNKNOWN)
  , results(false) {
}

Cask::CdapOdbc::QueryStatus::QueryStatus(web::json::value value) {
  this->opStatus = this->parseStatus(value.at(L"status").as_string());
  this->results = value.at(L"hasResults").as_bool();
}
