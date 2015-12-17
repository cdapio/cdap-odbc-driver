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

namespace Cask {
  namespace CdapOdbc {

    /**
     * The methods of this class call the HTTP APIs exposed by explore 
     * and return the raw information
     * contained in their JSON responses.
     */
    class ExploreClient {
      std::unique_ptr<web::http::client::http_client> httpClient;
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
    };
  }
}
