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
#include "Connection.h"
#include "Environment.h"
#include "Encoding.h"
#include "CommunicationLinkFailure.h"

using namespace Cask::CdapOdbc;

web::http::uri Cask::CdapOdbc::Connection::resolveUri() const {
  web::http::uri_builder uri;
  uri.set_scheme(this->params->getSslEnabled() ? L"https" : L"http");
  uri.set_host(this->params->getHost());
  uri.append_path(L"v3");
  if (this->params->getPort() > 0) {
    uri.set_port(this->params->getPort());
  }

  return uri.to_uri();
}

Cask::CdapOdbc::Connection::Connection(Environment* environment, SQLHDBC handle)
  : environment(environment)
  , handle(handle)
  , isOpen(false) {
  assert(environment);
  assert(handle);
}

void Cask::CdapOdbc::Connection::open(const std::wstring& connectionString) {
  assert(!this->isOpen);
  this->params = std::make_unique<ConnectionParams>(connectionString);
  auto baseUri = this->resolveUri();
  this->exploreClient = std::make_unique<ExploreClient>(baseUri, this->params->getNamespace());
  if (this->exploreClient->isAvailable()) {
    this->isOpen = true;
  } else {
    throw CommunicationLinkFailure("Service unavailable.");
  }
}

void Cask::CdapOdbc::Connection::close() {
  assert(this->isOpen);
  this->exploreClient.reset();
  this->params.reset();
  this->isOpen = false;
}
