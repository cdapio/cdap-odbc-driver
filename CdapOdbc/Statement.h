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
#include "ColumnMapper.h"

namespace Cask {
  namespace CdapOdbc {
    class Connection;

    /**
    * Represents a SQL statement which can be executed to return some data.
    */
    class Statement {
  
      enum class State {
        INITIAL,
        OPEN,
        FETCH,
        CLOSED,
      };

      enum class RequestType {
        UNKNOWN,
        CATALOGS,
        SCHEMAS,
        TABLES,
        TYPES,
        COLUMNS,
        SPECIAL_COLUMNS,
        DATA
      };

      std::vector<ColumnBinding> columnBindings;
      QueryHandle queryHandle;
      QueryResult queryResult;
      State state;
      Connection* connection;
      SQLHSTMT handle;
      int fetchSize;
      int currentRowIndex;
      bool moreData;
      std::unique_ptr<ColumnMapper> mapper;
      RequestType requestType;
      std::wstring tableName;

      void throwStateError();
      void openQuery();
      bool loadData();
      bool getNextResults();
      void fetchRow();
      void setupMetadataMapper();
      void setupSimpleMapper();

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
       * Gets parent connection.
       */
      Connection* getConnection() const {
        return this->connection;
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
       * Retrieves schemas from a database.
       * Empty string means "" catalog or schema, NULL means all available.
       */
      void getSchemas(const std::wstring* catalog, const std::wstring* schemaPattern);

      /**
       * Retrieves tables from a database.
       * Empty string means "" catalog or schema, NULL means all available.
       */
      void getTables(
        const std::wstring* catalog,
        const std::wstring* schemaPattern,
        const std::wstring* tableNamePattern,
        const std::wstring* tableTypes);

      /**
       * Retrieves data type information.
       */
      void getDataTypes();

      /**
       * Retrieves column information for a table.
       */
      void getColumns(const std::wstring& tableName);

      /**
       * Retrieves information about special columns for a table.
       */
      void getSpecialColumns();

      /**
       * Retrieves the next row for the current statement 
       * and updates column bindings.
       */
      void fetch();

      /**
       * Resets statement to initial state.
       */
      void reset();

      /**
       * Unbinds all column bindings.
       */
      void unbindColumns();

      /**
       * Resets parameters.
       */
      void resetParameters();
    };
  }
}
