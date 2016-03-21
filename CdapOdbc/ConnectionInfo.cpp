/*
* Copyright © 2016 Cask Data, Inc.
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
#include "ConnectionInfo.h"
#include "Environment.h"
#include "ConnectionParams.h"
#include "Connection.h"

using namespace Cask::CdapOdbc;

bool Cask::CdapOdbc::ConnectionInfo::canReuse(const Connection& connection) const {
  return (_wcsicmp(connection.getParams().getHost().c_str(), this->params->getHost().c_str()) == 0 &&
          connection.getParams().getPort() == this->params->getPort());
}

bool Cask::CdapOdbc::ConnectionInfo::perfectlyMatch(const Connection& connection) const {
  return (connection.getParams() == *this->params &&
          connection.getIsAsync() == this->isAsync &&
          connection.getIsFunctionsAsync() == this->isFunctionsAsync);
}

Cask::CdapOdbc::ConnectionInfo::ConnectionInfo(Environment* environment, SQLHDBC_INFO_TOKEN handle)
  : handle(handle)
  , environment(environment) {
}

void Cask::CdapOdbc::ConnectionInfo::setConnectionString(const std::wstring& connectionString) {
  this->params = std::make_unique<ConnectionParams>(connectionString);
}

int Cask::CdapOdbc::ConnectionInfo::rateConnection(const Connection& connection) {
  if (!this->canReuse(connection)) {
    return 0;
  }

  if (this->perfectlyMatch(connection)) {
    return 100;
  }

  return 50;
}
