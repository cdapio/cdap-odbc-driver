/*
* Copyright � 2016 Cask Data, Inc.
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
       * Tests for SQLTables/SQLFetch/SQLBindCol functions.
       */
      TEST_CLASS(SQLSyncTests) {
        auto createConnection() {
          int result = SQL_SUCCESS;
          SQLHANDLE env = 0;
          SQLHANDLE con = 0;
          SQLHANDLE stmt = 0;

          result = SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &env);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &con);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLSetConnectAttrW(con, SQL_ATTR_ASYNC_ENABLE, reinterpret_cast<SQLPOINTER>(SQL_ASYNC_ENABLE_OFF), 0);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLDriverConnectW(con, HWND_DESKTOP, L"Driver=CDAP ODBC;Host=localhost", SQL_NTS, nullptr, 0, nullptr, SQL_DRIVER_NOPROMPT);
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
        TEST_METHOD(PrepareAndFetchSyncSucceeds) {
          int result = SQL_SUCCESS;

          auto handles = this->createConnection();

          SQLHANDLE stmt = std::get<2>(handles);

          result = SQLPrepareW(stmt, L"SELECT * FROM stream_items", SQL_NTS);
          Assert::AreEqual(SQL_SUCCESS, result);

          double value = -100.0;
          SQLLEN ind = 0;
          result = SQLBindCol(stmt, 3, SQL_DOUBLE, &value, 0, &ind);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFetch(stmt);
          Assert::AreEqual(SQL_SUCCESS, result);

          Assert::IsTrue(value >= 0.0);

          this->freeConnection(handles);
        }

        /**
         * Checks Statement.getColumns().
         */
        TEST_METHOD(ColumnsAndFetchSyncSucceeds) {
          int result = SQL_SUCCESS;
          auto handles = this->createConnection();
          SQLHANDLE stmt = std::get<2>(handles);

          result = SQLColumnsW(stmt, nullptr, SQL_NTS, nullptr, SQL_NTS, L"stream_items", SQL_NTS, nullptr, SQL_NTS);
          Assert::AreEqual(SQL_SUCCESS, result);

          char value[128];
          SQLLEN ind = 0;
          result = SQLBindCol(stmt, 4, SQL_CHAR, value, 128, &ind);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFetch(stmt);
          Assert::AreEqual(SQL_SUCCESS, result);

          Assert::IsTrue(std::strlen(value) > 0);

          this->freeConnection(handles);
        }

        /**
         * Checks Statement.getTables().
         */
        TEST_METHOD(TablesAndFetchSyncSucceeds) {
          int result = SQL_SUCCESS;
          auto handles = this->createConnection();
          SQLHANDLE stmt = std::get<2>(handles);

          result = SQLTablesW(stmt, nullptr, SQL_NTS, nullptr, SQL_NTS, nullptr, SQL_NTS, L"TABLE", SQL_NTS);
          Assert::AreEqual(SQL_SUCCESS, result);

          char value[128];
          SQLLEN ind = 0;
          result = SQLBindCol(stmt, 3, SQL_CHAR, value, 128, &ind);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFetch(stmt);
          Assert::AreEqual(SQL_SUCCESS, result);

          Assert::IsTrue(std::strlen(value) > 0);

          this->freeConnection(handles);
        }
      };
    }
  }
}