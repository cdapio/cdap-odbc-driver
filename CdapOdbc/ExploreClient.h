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

#pragma once

#include "ColumnDesc.h"
#include "QueryStatus.h"
#include "QueryResult.h"

namespace Cask {
  namespace CdapOdbc {

    using QueryHandle = utility::string_t;

    /**
     * The methods of this class call the HTTP APIs exposed by explore 
     * and return the raw information
     * contained in their JSON responses.
     */
    class ExploreClient {
      std::unique_ptr<web::http::client::http_client> httpClient;

      web::json::value doRequest(web::http::http_request& request);
      web::json::value doRequest(web::http::method mhd, const utility::string_t& path);
      web::json::value doRequest(web::http::method mhd, const utility::string_t& path, web::json::value body);
      web::json::value doGet(const utility::string_t& path);
      web::json::value doPost(const utility::string_t& path);
      web::json::value doPost(const utility::string_t& path, web::json::value body);
      web::json::value doDelete(const utility::string_t& path);

      ExploreClient(const ExploreClient&) = delete;
      void operator=(const ExploreClient&) = delete;

    public:

      /**
       * Creates an instance of explore client.
       */
      ExploreClient(const web::http::uri& baseUri);
      
      /**
       * Destructor.
       */
      ~ExploreClient() = default;

      /**
       * Checks if CDAP service available.
       */
      bool isAvailable();

      /**
       * Retrieves a query status.
       */
      QueryStatus getQueryStatus(const QueryHandle& handle);

      /**
       * Retrieves a query schema.
       */
      std::vector<ColumnDesc> getQuerySchema(const QueryHandle& handle);

      /**
       * Retrieves the next query result.
       */
      QueryResult getQueryResult(const QueryHandle& handle, int rows);

      /**
       * Closes a query.
       */
      void closeQuery(const QueryHandle& handle);

      /**
       * Retrieves catalog names available in a database.
       */
      QueryHandle getCatalogs();

      /**
       * Retrieves schema names available in a database.
       */
      QueryHandle getSchemas(const std::wstring* catalog, const std::wstring* schemaPattern);

      /**
       * Retrieves table names available in a database.
       */
      QueryHandle getTables(
        const std::wstring* catalog,
        const std::wstring* schemaPattern,
        const std::wstring* tableNamePattern,
        const std::vector<std::wstring>* tableTypes);

      /**
       * Retrieves a list of streams.
       */
      QueryResult getStreams();

      /**
       * Retrieves a list.
       */
      QueryResult getStreamFields(const std::wstring& streamName);

      /**
      * Executes SQL statement.
      */
      QueryHandle execute(const std::wstring& statement);
    };
  }
}
