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
    class Environment;
    class Connection;
    class Statement;
    class Descriptor;

    /**
      Represents root object of ODBC driver.
      Implemented as a singleton.
    */
    class Driver {
      static std::unique_ptr<Driver> instance;
      static std::atomic_int lastHandleIndex;

      std::unordered_map<SQLHENV, std::unique_ptr<Environment>> environments;
      std::unordered_map<SQLHDBC, std::unique_ptr<Connection>> connections;
      std::unordered_map<SQLHSTMT, std::unique_ptr<Statement>> statements;
      std::unordered_map<SQLHDESC, std::unique_ptr<Descriptor>> descriptors;

      std::mutex mutex;

      Environment* findEnvironment(SQLHENV env);
      Connection* findConnection(SQLHDBC dbc);

      Driver(const Driver&) = delete;
      void operator=(const Driver&) = delete;

    public:

      Driver() = default;
      ~Driver() = default;

      static Driver& getInstance();
      static SQLHANDLE generateNewHandle();

      Environment& getEnvironment(SQLHENV env);
      Connection& getConnection(SQLHDBC dbc);
      Statement& getStatement(SQLHSTMT stmt);
      Descriptor& getDescriptor(SQLHDESC desc);

      SQLHENV allocEnvironment();
      SQLHDBC allocConnection(SQLHENV env);
      SQLHSTMT allocStatement(SQLHDBC dbc);
      SQLHDESC allocDescriptor(SQLHDBC dbc);
      
      void freeEnvironment(SQLHENV env);
      void freeConnection(SQLHDBC dbc);
      void freeStatement(SQLHSTMT stmt);
      void freeDescriptor(SQLHDESC desc);
    };
  }
}
