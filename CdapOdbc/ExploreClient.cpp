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
#include "BadRequestException.h"
#include "CdapException.h"

using namespace Cask::CdapOdbc;

namespace {
  web::json::value toJson(const utility::string_t* str) {
    return (str != nullptr) ? web::json::value::string(*str) : web::json::value::null();
  }

  utility::string_t toUriPath(const utility::string_t* str) {
    if (str == nullptr) {
      return L"null";
    } else {
      return *str;
    }
  }
}

web::json::value Cask::CdapOdbc::ExploreClient::doRequest(web::http::http_request& request) {
  auto task = this->httpClient->request(request)
    .then([](web::http::http_response response) {
    if (response.status_code() == web::http::status_codes::OK) {
      return response.extract_json();
    } else if (response.status_code() == web::http::status_codes::BadRequest) {
      auto task = response.extract_utf16string();
      task.wait();
      throw BadRequestException(task.get());
    } else {
      throw CdapException(L"Cannot get the response.");
    }
  });
  task.wait();
  return task.get();
}

web::json::value Cask::CdapOdbc::ExploreClient::doRequest(web::http::method mhd, const utility::string_t& path) {
  web::http::uri_builder requestUri;
  web::http::http_request request(mhd);
  request.set_request_uri(requestUri.append_path(path).to_uri());
  //request.headers().add(header_names::authorization, L"Bearer " + this->authToken);
  return this->doRequest(request);
}

web::json::value Cask::CdapOdbc::ExploreClient::doRequest(web::http::method mhd, const utility::string_t & path, web::json::value body) {
  web::http::uri_builder requestUri;
  web::http::http_request request(mhd);
  request.set_body(body);
  request.set_request_uri(requestUri.append_path(path).to_uri());
  //request.headers().add(header_names::authorization, L"Bearer " + this->authToken);
  return this->doRequest(request);
}

web::json::value Cask::CdapOdbc::ExploreClient::doGet(const utility::string_t& path) {
  return this->doRequest(web::http::methods::GET, path);
}

web::json::value Cask::CdapOdbc::ExploreClient::doPost(const utility::string_t & path) {
  return this->doRequest(web::http::methods::POST, path);
}

web::json::value Cask::CdapOdbc::ExploreClient::doPost(const utility::string_t & path, web::json::value body) {
  return this->doRequest(web::http::methods::POST, path, body);
}

web::json::value Cask::CdapOdbc::ExploreClient::doDelete(const utility::string_t & path) {
  return this->doRequest(web::http::methods::DEL, path);
}

Cask::CdapOdbc::ExploreClient::ExploreClient(const web::http::uri& baseUri, const std::wstring& namespace_)
  : namespace_(namespace_) {
  this->httpClient = std::make_unique<web::http::client::http_client>(baseUri);
}

bool Cask::CdapOdbc::ExploreClient::isAvailable() {
  try {
    return (this->doGet(L"explore/status").at(L"status").as_string() == L"OK");
  } catch (std::exception&) {
    return false;
  }
}

QueryStatus Cask::CdapOdbc::ExploreClient::getQueryStatus(const QueryHandle& handle) {
  auto value = this->doGet(L"data/explore/queries/" + handle + L"/status");
  return QueryStatus(value);
}

std::vector<ColumnDesc> Cask::CdapOdbc::ExploreClient::getQuerySchema(const QueryHandle& handle) {
  std::vector<ColumnDesc> result;
  auto value = this->doGet(L"data/explore/queries/" + handle + L"/schema");
  auto columns = value.as_array();
  for (auto& item : columns) {
    result.push_back(ColumnDesc(item));
  }

  return result;
}

QueryResult Cask::CdapOdbc::ExploreClient::getQueryResult(const QueryHandle& handle, int rows) {
  web::json::value size;
  size[L"size"] = web::json::value::number(rows);
  auto value = this->doPost(L"data/explore/queries/" + handle + L"/next", size);
  return QueryResult(value);
}

void Cask::CdapOdbc::ExploreClient::closeQuery(const QueryHandle& handle) {
  this->doDelete(L"data/explore/queries/" + handle);
}

QueryHandle Cask::CdapOdbc::ExploreClient::getCatalogs() {
  return this->doPost(L"data/explore/jdbc/catalogs").at(L"handle").as_string();
}

QueryHandle Cask::CdapOdbc::ExploreClient::getSchemas(const std::wstring* catalog, const std::wstring* schemaPattern) {
  web::json::value schemaArgs;
  schemaArgs[L"catalog"] = toJson(catalog);
  schemaArgs[L"schemaPattern"] = toJson(schemaPattern);

  auto value = this->doPost(L"namespaces/" + toUriPath(schemaPattern) + L"/data/explore/jdbc/schemas", schemaArgs);
  return value.at(L"handle").as_string();
}

QueryHandle Cask::CdapOdbc::ExploreClient::getTables(const std::wstring* catalog, const std::wstring* schemaPattern, const std::wstring* tableNamePattern, const std::vector<std::wstring>* tableTypes) {
  web::json::value tablesArgs;

  tablesArgs[L"catalog"] = toJson(catalog);
  tablesArgs[L"schemaPattern"] = toJson(schemaPattern);
  tablesArgs[L"tableNamePattern"] = toJson(tableNamePattern);

  if (tableTypes) {
    std::vector<web::json::value> tableTypeArgs;

    for (auto& item : *tableTypes) {
      tableTypeArgs.push_back(web::json::value::string(item));
    }

    tablesArgs[L"tableTypes"] = web::json::value::array(tableTypeArgs);
  } else {
    tablesArgs[L"tableTypes"] = web::json::value::null();
  }

  auto value = this->doPost(L"namespaces/" + toUriPath(schemaPattern) + L"/data/explore/jdbc/tables", tablesArgs);
  return value.at(L"handle").as_string();
}

QueryResult Cask::CdapOdbc::ExploreClient::getStreams() {
  auto value = this->doGet(L"namespaces/" + this->namespace_ + L"/streams");
  return QueryResult(value);
}

QueryResult Cask::CdapOdbc::ExploreClient::getDatasets() {
  auto value = this->doGet(L"namespaces/" + this->namespace_ + L"/data/datasets");
  return QueryResult(value);
}

QueryResult Cask::CdapOdbc::ExploreClient::getStreamFields(const std::wstring& streamName) {
  auto value = this->doGet(L"namespaces/" + this->namespace_ + L"/streams/" + streamName).at(L"format").at(L"schema").at(L"fields");
  return QueryResult(value);
}

QueryResult Cask::CdapOdbc::ExploreClient::getDatasetFields(const std::wstring& datasetName) {
  auto value = this->doGet(L"namespaces/" + this->namespace_ + L"/data/datasets/" + datasetName).at(L"spec").at(L"properties").at(L"schema");
  std::error_code error;
  auto schema = web::json::value::parse(value.as_string(), error);
  if (error) {
    throw std::system_error(error);
  }

  return QueryResult(schema.at(L"fields"));
}

QueryHandle Cask::CdapOdbc::ExploreClient::execute(const std::wstring& statement) {
  web::json::value query;
  std::wstring stmt_preproc = statement;
	// TODO: Remove the wrapping SELECT properly
	// if (stmt_preproc.find(L"\"") != std::wstring::npos) {
	//   stmt_preproc = stmt_preproc.substr(18, statement.length() - 21 - 18);
  // }
  query[L"query"] = toJson(&stmt_preproc);
  auto value = this->doPost(L"namespaces/" + this->namespace_ + L"/data/explore/queries", query);
  return value.at(L"handle").as_string();
}
