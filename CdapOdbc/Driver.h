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

#include "DataType.h"

namespace Cask {
  namespace CdapOdbc {
    class Environment;
    class Connection;
    class Statement;
    class Descriptor;

    /**
     * Represents root object of ODBC driver.
     * Implemented as a singleton.
     */
    class Driver {
      static std::unique_ptr<Driver> instance;
      static std::atomic_int lastHandleIndex;

      std::map<SQLHENV, std::unique_ptr<Environment>> environments;
      std::map<SQLHDBC, std::unique_ptr<Connection>> connections;
      std::map<SQLHSTMT, std::unique_ptr<Statement>> statements;
      std::map<SQLHDESC, std::unique_ptr<Descriptor>> descriptors;

      std::mutex mutex;

      std::map<std::wstring, DataType> dataTypes;

      static SQLHANDLE generateNewHandle();
      Environment* findEnvironment(SQLHENV env);
      Connection* findConnection(SQLHDBC dbc);
      void freeConnections(const Environment& env);
      void freeStatements(const Connection& dbc);
      void freeDescriptors(const Connection& dbc);
      void initializeDataTypes();

      Driver(const Driver&) = delete;
      void operator=(const Driver&) = delete;

    public:

      /**
       * Creates a driver instance.
       */
      Driver();

      /**
       * Destructor.
       */
      ~Driver();

      /**
       * Gets a driver instance.
       */
      static Driver& getInstance();

      /**
      * Gets an environment by handle.
      */
      Environment& getEnvironment(SQLHENV env);

      /**
      * Gets a connection by handle.
      */
      Connection& getConnection(SQLHDBC dbc);

      /**
      * Gets a statement by handle.
      */
      Statement& getStatement(SQLHSTMT stmt);

      /**
      * Gets a descriptor by handle.
      */
      Descriptor& getDescriptor(SQLHDESC desc);

      /**
      * Gets a data type by name.
      */
      const DataType& getDataType(const std::wstring& name) const {
        return this->dataTypes.at(name);
      }

      /**
      * Allocates a new environment and returns its handle.
      */
      SQLHENV allocEnvironment();

      /**
      * Allocates a new connection and returns its handle.
      */
      SQLHDBC allocConnection(SQLHENV env);

      /**
      * Allocates a new statement and returns its handle.
      */
      SQLHSTMT allocStatement(SQLHDBC dbc);

      /**
      * Allocates a new descriptor and returns its handle.
      */
      SQLHDESC allocDescriptor(SQLHDBC dbc);
      
      /**
       * Destroys an environment freeing all allocated resources.
       */
      void freeEnvironment(SQLHENV env);

      /**
      * Destroys a connection freeing all allocated resources.
      */
      void freeConnection(SQLHDBC dbc);

      /**
      * Destroys a statement freeing all allocated resources.
      */
      void freeStatement(SQLHSTMT stmt);

      /**
      * Destroys a descriptor freeing all allocated resources.
      */
      void freeDescriptor(SQLHDESC desc);

      /**
       * Setups supported functions.
       */
      void setupSupportedFunctions(SQLUSMALLINT* bitset);

      /**
       * Adds new DSN.
       */
      void addDataSource(HWND parentWindow, const std::wstring& driver, const std::wstring& attrs);

      /**
       * Modifies DSN.
       */
      void modifyDataSource(HWND parentWindow, const std::wstring& driver, const std::wstring& attrs);
    
      /**
       * Removes DSN.
       */
      void deleteDataSource(const std::wstring& driver, const std::wstring& attrs);
    };
  }
}
