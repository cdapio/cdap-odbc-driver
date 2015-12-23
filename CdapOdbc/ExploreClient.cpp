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
#include "ExploreClient.h"

using namespace Cask::CdapOdbc;

web::json::value Cask::CdapOdbc::ExploreClient::doRequest(web::http::method mhd, const utility::string_t& path) {
  using namespace web::http;
  uri_builder requestUri;
  requestUri.append_path(path);
  http_request request(mhd);
  request.set_request_uri(requestUri.to_uri());
  //request.headers().add(header_names::authorization, L"Bearer " + this->authToken);
  auto task = this->httpClient->request(request)
    .then([](web::http::http_response response) {
      if (response.status_code() == web::http::status_codes::OK) {
        return response.extract_json();
      } else {
        throw std::exception("Cannot get the response.");
      }
    });
  task.wait();
  return task.get();
}

web::json::value Cask::CdapOdbc::ExploreClient::doGet(const utility::string_t& path) {
  return this->doRequest(web::http::methods::GET, path);
}

web::json::value Cask::CdapOdbc::ExploreClient::doPost(const utility::string_t & path) {
  return this->doRequest(web::http::methods::POST, path);
}

web::json::value Cask::CdapOdbc::ExploreClient::doDelete(const utility::string_t & path) {
  return this->doRequest(web::http::methods::DEL, path);
}

Cask::CdapOdbc::ExploreClient::ExploreClient(const web::http::uri& baseUri) {
  this->httpClient = std::make_unique<web::http::client::http_client>(baseUri);
}

bool Cask::CdapOdbc::ExploreClient::isAvailable() {
  try {
    return (this->doGet(L"explore/status").at(L"status").as_string() == L"OK");
  } catch (std::exception) {
    return false;
  }
}

QueryStatus Cask::CdapOdbc::ExploreClient::getQueryStatus(const QueryHandle& handle) {
  return QueryStatus();
}

void Cask::CdapOdbc::ExploreClient::closeQuery(const QueryHandle& handle) {
  this->doDelete(L"data/explore/queries/" + handle);
}

QueryHandle Cask::CdapOdbc::ExploreClient::getCatalogs() {
  return this->doPost(L"data/explore/jdbc/catalogs").at(L"handle").as_string();
}
