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

#include "ConnectionParams.h"
#include "ExploreClient.h"
#include "SQLStatus.h"

namespace Cask {
  namespace CdapOdbc {
    class Environment;
	class SQLStatus;

    /**
     * Represents a connection to a database.
     */
    class Connection {
      Environment* environment;
      SQLHDBC handle;
      std::mutex mutex;
      bool isOpen;
      std::unique_ptr<ConnectionParams> params;
      std::unique_ptr<ExploreClient> exploreClient;
	  SQLStatus sqlStatus;

      web::http::uri resolveUri() const;

      Connection(const Connection&) = delete;
      void operator=(const Connection&) = delete;

    public:

      /**
       * Creates a connection instance.
       */
      Connection(Environment* environment, SQLHDBC handle);

      /**
       * Destructor.
       */
      ~Connection() = default;

      /**
       * Gets a connection handle.
       */
      SQLHDBC getHandle() const {
        return this->handle;
      }

      /** 
       * Gets an explore client instance.
       */
      ExploreClient& getExploreClient() const {
        return *this->exploreClient;
      }

      /**
       * Gets parent environment.
       */
      Environment* getEnvironment() const {
        return this->environment;
      }

	  /**
	   * Gets SqlStatus storage
	   */
	  SQLStatus& getSqlStatus() {
		  return this->sqlStatus;
	  }

      /**
       * Opens a connection to explore REST service.
       *
       * Connection string has a form "PARAM1=VALUE1;PARAM2=VALUE2". 
       * There are the following parameters:
       *   HOST - server name
       *   PORT - port number (default 10000)
       *   AUTH_TOKEN - authentication token (default "")
       *   NAMESPACE - namespace name (default "default")
       *   SSL_ENABLED - SSL enabled/disabled (default false)
       *   VERIFY_SSL_CERT - server certificate verification (default true)
       */
      void open(const std::wstring& connectionString);
      
      /*
       * Closes a connection.
       */
      void close();
    };
  }
}
