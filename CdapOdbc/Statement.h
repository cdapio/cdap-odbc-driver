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
#include "Command.h"
#include "ColumnInfo.h"
#include "SQLStatus.h"

namespace Cask {
  namespace CdapOdbc {
    class Connection;
	class SQLStatus;

    /**
    * Represents a SQL statement which can be executed to return some data.
    */
    class Statement {
  
      enum class State {
        INITIAL,
        PREPARE,
        OPEN,
        FETCH,
        CLOSED,
      };

      State state;
      Connection* connection;
      SQLHSTMT handle;
      std::vector<ColumnBinding> columnBindings;
      std::unique_ptr<Command> command;
      std::unique_ptr<DataReader> dataReader;
	  SQLStatus sqlStatus;

      void throwStateError() const;
      void openQuery();

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
       * Adds a column binding information to a statement.
       */
      void addColumnBinding(const ColumnBinding& binding);

      /**
       * Removes a column binding information from a statement.
       */
      void removeColumnBinding(SQLUSMALLINT columnNumber);

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
      void getColumns(const std::wstring& streamName);

      /**
       * Retrieves information about special columns for a table.
       */
      void getSpecialColumns();

	  /**
	  * Gets SqlStatus storage
	  */
	  SQLStatus& getSqlStatus() {
		  return this->sqlStatus;
	  }

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

      /**
       * Prepares SQL query statement for execution.
       */
      void prepare(const std::wstring& query);

      /**
       * Executes SQL query.
       */
      void execute();

      /**
       * Directly executes SQL query (without prepare first).
       */
      void executeDirect(const std::wstring& query);

      /**
       * Returns number of columns for a statement. 
       */
      SQLSMALLINT getColumnCount() const;

      /**
       * Gets a column info.
       */
      std::unique_ptr<ColumnInfo> getColumnInfo(short columnNumber) const;
    };
  }
}
