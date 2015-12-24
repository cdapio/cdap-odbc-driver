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

#include "ColumnBinding.h"
#include "ExploreClient.h"

namespace Cask {
  namespace CdapOdbc {
    class Connection;

    /**
    * Represents a SQL statement which can be executed to return some data.
    */
    class Statement {
      Connection* connection;
      SQLHSTMT handle;
      std::vector<ColumnBinding> columnBindings;
      QueryHandle queryHandle;
      int fetchSize;
      int currentRowIndex;
      bool open;

      void openQuery();
      void getNextResults();
      void fetchRow();

      Statement(const Statement&) = delete;
      void operator=(const Statement&) = delete;

    public:

      /**
      * Creates a statement instance.
      */
      Statement(Connection* connection, SQLHSTMT handle);

      /**
      * Destructor.
      */
      ~Statement() = default;

      /**
      * Gets ODBC statement handle.
      */
      SQLHSTMT getHandle() const {
        return this->handle;
      }

      /**
       * Gets a fetch size.
       */
      int getFetchSize() const {
        return this->fetchSize;
      }

      /**
       * Sets a fetch size.
       */
      void setFetchSize(int value) {
        this->fetchSize = value;
      }

      /**
       * Adds a column binding information to a statement.
       */
      void addColumnBinding(const ColumnBinding& binding);

      /**
       * Removes a column binding information from a statement.
       */
      void removeColumnBinding(SQLUSMALLINT columnNumber);

      /**
       * Retrieves catalogs from a database.
       */
      void getCatalogs();

      /**
       * Retrieves the next row for the current statement 
       * and updates column bindings.
       */
      void fetch();

      /**
       * Closes the statement.
       */
      void close();
    };
  }
}
