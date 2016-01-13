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

#include "Command.h"
#include "ExploreClient.h"

namespace Cask {
  namespace CdapOdbc {

    /**
     * Gets a SQL query data from a database.
     */
    class QueryCommand : public Command {

      std::wstring query;
      QueryHandle queryHandle;
      bool hasData;

    public:

      /**
       * Creates a command instance.
       */
      QueryCommand(Connection* connection, const std::wstring& query);

      /**
       * Gets if query has some data.
       */
      bool getHasData() const {
        return this->hasData;
      }

      /**
       * Executes a command and builds a data reader.
       */
      virtual std::unique_ptr<DataReader> executeReader() override;

      /**
       * Loads next N rows.
       */
      QueryResult loadRows(int rows);

      /**
       * Loads query schema.
       */
      std::vector<ColumnDesc> loadSchema();
    };
  }
}
