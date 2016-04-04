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

#pragma once

#include "SQLStatus.h"
#include "SecureString.h"

namespace Cask {
  namespace CdapOdbc {
    class Environment;
    class ConnectionParams;
    class Connection;

    /**
     * Represents a connection info.
     */
    class ConnectionInfo {

      SQLHDBC_INFO_TOKEN handle;
      Environment* environment;
      SQLStatus sqlStatus;
      std::unique_ptr<ConnectionParams> params;
      bool isAsync;
      bool isFunctionsAsync;

      bool canReuse(const Connection&) const;
      bool perfectlyMatch(const Connection&) const;

      ConnectionInfo(const ConnectionInfo&) = delete;
      void operator=(const ConnectionInfo&) = delete;

    public:

      /**
       * Creates a connection info instance.
       */
      ConnectionInfo(Environment* environment, SQLHDBC_INFO_TOKEN handle);

      /**
       * Destructor.
       */
      ~ConnectionInfo() = default;

      /**
       * Gets SqlStatus storage.
       */
      SQLStatus& getSqlStatus() {
        return this->sqlStatus;
      }

      /**
       * Sets connection string to info.
       */
      void setConnectionString(const SecureString& connectionString);

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
       * Determines reuse rating of an existing connection.
       */
      int rateConnection(const Connection& connection);
    };
  }
}
