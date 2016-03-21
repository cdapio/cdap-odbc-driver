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
    class ConnectionInfo;

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
      bool isAsync;
      bool isFunctionsAsync;
      std::unique_ptr<pplx::task<void>> openTask;

      web::http::uri resolveUri() const;
      void internalOpen(const std::wstring& connectionString);

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
       * Gets SqlStatus storage.
       */
      SQLStatus& getSqlStatus() {
        return this->sqlStatus;
      }

      /**
       * Gets connection async mode. 
       */
      bool getIsAsync() const {
        return this->isAsync;
      }

      /**
       * Gets connection functions async mode.
       */
      bool getIsFunctionsAsync() const {
        return this->isFunctionsAsync;
      }

      /**
       * Gets connection params.
       */
      const ConnectionParams& getParams() const {
        return *this->params;
      }

      /**
       * Sets connection async mode. 
       */
      void setAsync(bool value) {
        this->isAsync = value;
      }

      /**
       * Sets connection async mode.
       */
      void setFunctionsAsync(bool value) {
        this->isFunctionsAsync = value;
      }

      /**
       * Locks the mutex.
       */
      void lock() {
        this->mutex.lock();
      }

      /**
       * Unlocks the mutex
       */
      void unlock() {
        this->mutex.unlock();
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

      /**
      * Opens a connection to explore REST service.
      */
      bool openAsync(const std::wstring& connectionString);

      /*
       * Closes a connection.
       */
      void close();

      /**
       * Sets connection info to connection.
       */
      void setInfo(const ConnectionInfo& info);
    };
  }
}
