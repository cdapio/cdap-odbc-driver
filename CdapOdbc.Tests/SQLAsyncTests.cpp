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

#include "stdafx.h"
#include "CppUnitTest.h"

#define TEST_TABLE L"stream_contacts"
#define TEST_QUERY L"SELECT * FROM stream_contacts"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Cask {
  namespace CdapOdbc {
    namespace Tests {
      /**
       * Tests for SQLTables/SQLFetch/SQLBindCol functions.
       */
      TEST_CLASS(SQLAsyncTests) {

        auto createConnection() {
          int result = SQL_SUCCESS;
          SQLHANDLE env = 0;
          SQLHANDLE con = 0;
          SQLHANDLE stmt = 0;

          result = SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &env);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &con);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLSetConnectAttrW(con, SQL_ATTR_ASYNC_ENABLE, reinterpret_cast<SQLPOINTER>(SQL_ASYNC_ENABLE_ON), 0);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLDriverConnectW(con, HWND_DESKTOP, TEST_CONNECTION_STRING, SQL_NTS, nullptr, 0, nullptr, SQL_DRIVER_NOPROMPT);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLAllocHandle(SQL_HANDLE_STMT, con, &stmt);
          Assert::AreEqual(SQL_SUCCESS, result);

          return std::make_tuple(env, con, stmt);
        }

        void freeConnection(std::tuple<SQLHANDLE, SQLHANDLE, SQLHANDLE> handles) {
          SQLDisconnect(std::get<1>(handles));
          SQLFreeHandle(SQL_HANDLE_STMT, std::get<2>(handles));
          SQLFreeHandle(SQL_HANDLE_DBC, std::get<1>(handles));
          SQLFreeHandle(SQL_HANDLE_ENV, std::get<0>(handles));
        }
      public:

        /**
         * Checks Statement.executeAsync().
         */
        TEST_METHOD(PrepareAndFetchAsyncSucceeds) {
          int result = SQL_SUCCESS;
          auto handles = this->createConnection();
          SQLHANDLE stmt = std::get<2>(handles);

          result = SQLPrepareW(stmt, TEST_QUERY, SQL_NTS);
          do {
            result = SQLPrepareW(stmt, TEST_QUERY, SQL_NTS);
          } while (result == SQL_STILL_EXECUTING);
          Assert::AreEqual(SQL_SUCCESS, result);

          double value = -100.0;
          SQLLEN ind = 0;
          result = SQLBindCol(stmt, 3, SQL_DOUBLE, &value, 0, &ind);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFetch(stmt);
          do {
            result = SQLFetch(stmt);
          } while (result == SQL_STILL_EXECUTING);
          Assert::AreEqual(SQL_SUCCESS, result);

          Assert::IsTrue(value >= 0.0);

          this->freeConnection(handles);
        }

        /**
         * Checks Statement.getColumnsAsync().
         */
        TEST_METHOD(ColumnsAndFetchAsyncSucceeds) {
          int result = SQL_SUCCESS;
          auto handles = this->createConnection();
          SQLHANDLE stmt = std::get<2>(handles);

          result = SQLColumnsW(stmt, nullptr, SQL_NTS, nullptr, SQL_NTS, TEST_TABLE, SQL_NTS, nullptr, SQL_NTS);
          do {
            result = SQLColumnsW(stmt, nullptr, SQL_NTS, nullptr, SQL_NTS, TEST_TABLE, SQL_NTS, nullptr, SQL_NTS);
          } while (result == SQL_STILL_EXECUTING);
          Assert::AreEqual(SQL_SUCCESS, result);

          char value[128];
          SQLLEN ind = 0;
          result = SQLBindCol(stmt, 1, SQL_CHAR, value, 128, &ind);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFetch(stmt);
          do {
            result = SQLFetch(stmt);
          } while (result == SQL_STILL_EXECUTING);
          Assert::AreEqual(SQL_SUCCESS, result);

          Assert::IsTrue(std::strlen(value) > 0);

          this->freeConnection(handles);
        }

        /**
         * Checks Statement.getTablesAsync().
         */
        TEST_METHOD(TablesAndFetchAsyncSucceeds) {
          int result = SQL_SUCCESS;
          auto handles = this->createConnection();
          SQLHANDLE stmt = std::get<2>(handles);

          result = SQLTablesW(stmt, nullptr, SQL_NTS, nullptr, SQL_NTS, nullptr, SQL_NTS, L"TABLE", SQL_NTS);
          do {
            result = SQLTablesW(stmt, nullptr, SQL_NTS, nullptr, SQL_NTS, nullptr, SQL_NTS, L"TABLE", SQL_NTS);
          } while (result == SQL_STILL_EXECUTING);
          Assert::AreEqual(SQL_SUCCESS, result);

          char value[128];
          SQLLEN ind = 0;
          result = SQLBindCol(stmt, 3, SQL_CHAR, value, 128, &ind);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFetch(stmt);
          do {
            result = SQLFetch(stmt);
          } while (result == SQL_STILL_EXECUTING);
          Assert::AreEqual(SQL_SUCCESS, result);

          Assert::IsTrue(std::strlen(value) > 0);

          this->freeConnection(handles);
        }

        /**
         * Checks Connection.openAsync().
         */
        TEST_METHOD(ConnectionOpenAsyncSucceeds) {
          int result = SQL_SUCCESS;
          SQLHANDLE env = 0;
          SQLHANDLE con = 0;

          result = SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &env);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &con);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLSetConnectAttrW(con, SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE, reinterpret_cast<SQLPOINTER>(SQL_ASYNC_ENABLE_ON), 0);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLDriverConnectW(con, HWND_DESKTOP, TEST_CONNECTION_STRING, SQL_NTS, nullptr, 0, nullptr, SQL_DRIVER_NOPROMPT);
          do {
            result = SQLDriverConnectW(con, HWND_DESKTOP, TEST_CONNECTION_STRING, SQL_NTS, nullptr, 0, nullptr, SQL_DRIVER_NOPROMPT);
          } while (result == SQL_STILL_EXECUTING);
          Assert::AreEqual(SQL_SUCCESS, result);
          
          SQLDisconnect(con);
          SQLFreeHandle(SQL_HANDLE_DBC, con);
          SQLFreeHandle(SQL_HANDLE_ENV, env);
        }
      };
    }
  }
}