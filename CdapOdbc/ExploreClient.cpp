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

web::json::value Cask::CdapOdbc::ExploreClient::doRequest(web::http::http_request& request, std::int64_t* sizeInBytes) {
#ifdef TRACE_REQUESTS
  TRACE(L"REQUEST: %s\n", request.to_string().c_str());
#endif
  auto requestTask = this->httpClient->request(request);
  {
    PROFILE_FUNCTION(TIMER_QUERY);
    requestTask.wait();
  }

  auto response = requestTask.get();
#ifdef TRACE_REQUESTS
  TRACE(L"RESPONSE: %s\n", response.to_string().c_str());
#endif
//  TRACE(L"RESPONSE SIZE: %d\n", response.headers().content_length());
  if (response.status_code() == web::http::status_codes::OK) {
    auto jsonTask = response.extract_json();
    {
      PROFILE_FUNCTION(TIMER_PARSING);
      jsonTask.wait();
    }

    if (sizeInBytes) {
      *sizeInBytes = response.headers().content_length();
    }

    return jsonTask.get();
  } else if (response.status_code() == web::http::status_codes::BadRequest) {
    auto errorTask = response.extract_utf16string();
    errorTask.wait();
    throw BadRequestException(errorTask.get());
  } else {
    throw CdapException(L"Cannot get the response.");
  }
}

web::json::value Cask::CdapOdbc::ExploreClient::doRequest(web::http::method mhd, const utility::string_t& path, const web::json::value* body /* = nullptr */, std::int64_t* sizeInBytes /* = nullptr */) {
  web::http::uri_builder requestUri;
  web::http::http_request request(mhd);
  
  if (body) {
    request.set_body(*body);
  }

  request.set_request_uri(requestUri.append_path(path).to_uri());
  if (this->authToken.size() > 0) {
    request.headers().add(web::http::header_names::authorization, L"Bearer " + this->authToken);
  }

  return this->doRequest(request, sizeInBytes);
}

Cask::CdapOdbc::ExploreClient::ExploreClient(const web::http::uri& baseUri, const std::wstring& namespace_, const SecureString& authToken)
  : namespace_(namespace_)
  , authToken(authToken) {
  this->httpClient = std::make_unique<web::http::client::http_client>(baseUri);
}

bool Cask::CdapOdbc::ExploreClient::isAvailable() {
  try {
    return (this->doRequest(web::http::methods::GET, L"explore/status").at(L"status").as_string() == L"OK");
  } catch (std::exception&) {
    return false;
  }
}

QueryStatus Cask::CdapOdbc::ExploreClient::getQueryStatus(const QueryHandle& handle) {
  auto value = this->doRequest(web::http::methods::GET, L"data/explore/queries/" + handle + L"/status");
  return QueryStatus(value);
}

std::vector<ColumnDesc> Cask::CdapOdbc::ExploreClient::getQuerySchema(const QueryHandle& handle) {
  std::vector<ColumnDesc> result;
  auto value = this->doRequest(web::http::methods::GET, L"data/explore/queries/" + handle + L"/schema");
  auto columns = value.as_array();
  for (auto& item : columns) {
    result.push_back(ColumnDesc(item));
  }

  return result;
}

QueryResult Cask::CdapOdbc::ExploreClient::getQueryResult(const QueryHandle& handle, int rows) {
  web::json::value size;
  std::int64_t sizeInBytes = 0L;
  size[L"size"] = web::json::value::number(rows);
  auto value = this->doRequest(web::http::methods::POST, L"data/explore/queries/" + handle + L"/next", &size, &sizeInBytes);
  return QueryResult(value, sizeInBytes);
}

void Cask::CdapOdbc::ExploreClient::closeQuery(const QueryHandle& handle) {
  this->doRequest(web::http::methods::DEL, L"data/explore/queries/" + handle);
}

QueryHandle Cask::CdapOdbc::ExploreClient::getCatalogs() {
  return this->doRequest(web::http::methods::POST, L"data/explore/jdbc/catalogs").at(L"handle").as_string();
}

QueryHandle Cask::CdapOdbc::ExploreClient::getSchemas(const std::wstring* catalog, const std::wstring* schemaPattern) {
  web::json::value schemaArgs;
  schemaArgs[L"catalog"] = toJson(catalog);
  schemaArgs[L"schemaPattern"] = toJson(schemaPattern);

  auto value = this->doRequest(web::http::methods::POST, L"namespaces/" + toUriPath(schemaPattern) + L"/data/explore/jdbc/schemas", &schemaArgs);
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

  auto value = this->doRequest(web::http::methods::POST, L"namespaces/" + toUriPath(schemaPattern) + L"/data/explore/jdbc/tables", &tablesArgs);
  return value.at(L"handle").as_string();
}

QueryResult Cask::CdapOdbc::ExploreClient::getStreams() {
  auto value = this->doRequest(web::http::methods::GET, L"namespaces/" + this->namespace_ + L"/streams");
  return QueryResult(value);
}

QueryResult Cask::CdapOdbc::ExploreClient::getDatasets() {
  auto value = this->doRequest(web::http::methods::GET, L"namespaces/" + this->namespace_ + L"/data/datasets");
  return QueryResult(value);
}

QueryResult Cask::CdapOdbc::ExploreClient::getStreamFields(const std::wstring& streamName) {
  auto value = this->doRequest(web::http::methods::GET, L"namespaces/" + this->namespace_ + L"/streams/" + streamName);
  return QueryResult(value);
}

QueryResult Cask::CdapOdbc::ExploreClient::getDatasetFields(const std::wstring& datasetName) {
  auto value = this->doRequest(web::http::methods::GET, L"namespaces/" + this->namespace_ + L"/data/datasets/" + datasetName);
  return QueryResult(value);
}

QueryHandle Cask::CdapOdbc::ExploreClient::execute(const std::wstring& statement) {
  web::json::value query;
  std::wstring stmt_preproc = statement;
  std::wstring::size_type found;

  /* Remove 'HAVING (COUNT(1) > 0) clause' */
  found = stmt_preproc.find(L"HAVING (COUNT(1) > 0)", 0);
  if (found != std::wstring::npos) {
    stmt_preproc.erase(found, 21);
  }

  query[L"query"] = toJson(&stmt_preproc);
  auto value = this->doRequest(web::http::methods::POST, L"namespaces/" + this->namespace_ + L"/data/explore/queries", &query);
  return value.at(L"handle").as_string();
}
