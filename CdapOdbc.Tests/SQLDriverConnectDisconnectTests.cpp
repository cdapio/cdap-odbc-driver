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

#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Cask {
  namespace CdapOdbc {
    namespace Tests {
      /**
       * Tests for SQLDriverConnect/SQLDisconnect functions.
       */
      TEST_CLASS(SQLDriverConnectDisconnectTests) {

        std::tuple<SQLHENV, SQLHDBC> allocConnection() {
          SQLHENV env = nullptr;
          SQLRETURN result = SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &env);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
          Assert::IsNotNull(env);

          SQLHDBC dbc = nullptr;
          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
          Assert::IsNotNull(dbc);

          return std::make_tuple(env, dbc);
        }

        void freeConnection(std::tuple<SQLHENV, SQLHDBC> connection) {
          SQLRETURN result = SQLFreeHandle(SQL_HANDLE_DBC, std::get<1>(connection));
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));

          result = SQLFreeHandle(SQL_HANDLE_ENV, std::get<0>(connection));
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
        }

      public:

        /**
         * Checks that passing NULL in ConnectionHandle 
         * to SQLDriverConnect fails with SQL_INVALID_HANDLE error.
         */
        TEST_METHOD(NullConnectionHandleOnConnectFails) {
          SQLRETURN result = SQLDriverConnectA(nullptr, nullptr, nullptr, 0, nullptr, 0, nullptr, 0);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_INVALID_HANDLE));

          result = SQLDriverConnectW(nullptr, nullptr, nullptr, 0, nullptr, 0, nullptr, 0);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_INVALID_HANDLE));
        }

        /**
        * Checks that passing NULL in ConnectionHandle
        * to SQLDisconnect fails with SQL_INVALID_HANDLE error.
        */
        TEST_METHOD(NullConnectionHandleOnDisconnectFails) {
          SQLRETURN result = SQLDisconnect(nullptr);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_INVALID_HANDLE));
        }

        /**
         * Checks that passing NULL for connection string fails.
         */
        TEST_METHOD(NullConnectionStringFails) {
          auto connectionInfo = this->allocConnection();
          auto connectionHandle = std::get<1>(connectionInfo);

          SQLRETURN result = SQLDriverConnectA(connectionHandle, nullptr, nullptr, 0, nullptr, 0, nullptr, 0);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_ERROR));

          result = SQLDriverConnectW(connectionHandle, nullptr, nullptr, 0, nullptr, 0, nullptr, 0);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_ERROR));

          this->freeConnection(connectionInfo);
        }
      };
    }
  }
}