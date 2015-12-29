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
       * Tests for SQLTables/SQLFetch/SQLBindCol functions.
       */
      TEST_CLASS(SQLTablesTests) {

        std::tuple<SQLHENV, SQLHDBC, SQLHSTMT> allocStatement() {
          SQLHENV env = nullptr;
          SQLRETURN result = SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &env);
          Assert::AreEqual(static_cast<SQLRETURN>(SQL_SUCCESS), result);
          Assert::IsNotNull(env);

          SQLHDBC dbc = nullptr;
          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
          Assert::AreEqual(static_cast<SQLRETURN>(SQL_SUCCESS), result);
          Assert::IsNotNull(dbc);

          auto connectionStringW = this->getBaseConnectionStringW();
          result = SQLDriverConnectW(
            dbc,
            nullptr,
            const_cast<wchar_t*>(connectionStringW.c_str()),
            static_cast<SQLSMALLINT>(connectionStringW.size()),
            nullptr,
            0,
            nullptr,
            0);
          Assert::AreEqual(static_cast<SQLRETURN>(SQL_SUCCESS), result);

          SQLHSTMT stmt = nullptr;
          result = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
          Assert::AreEqual(static_cast<SQLRETURN>(SQL_SUCCESS), result);

          return std::make_tuple(env, dbc, stmt);
        }

        void freeStatement(std::tuple<SQLHENV, SQLHDBC, SQLHSTMT> stmt) {
          SQLRETURN result = SQLFreeHandle(SQL_HANDLE_STMT, std::get<2>(stmt));
          Assert::AreEqual(static_cast<SQLRETURN>(SQL_SUCCESS), result);

          result = SQLDisconnect(std::get<1>(stmt));
          Assert::AreEqual(static_cast<SQLRETURN>(SQL_SUCCESS), result);

          result = SQLFreeHandle(SQL_HANDLE_DBC, std::get<1>(stmt));
          Assert::AreEqual(static_cast<SQLRETURN>(SQL_SUCCESS), result);

          result = SQLFreeHandle(SQL_HANDLE_ENV, std::get<0>(stmt));
          Assert::AreEqual(static_cast<SQLRETURN>(SQL_SUCCESS), result);
        }

        std::wstring getBaseConnectionStringW() {
          return L"Host=localhost;Port=10000";
        }

      public:

        static const int StrLen = 1024;

        struct DataBinding {
          char TargetValuePtr[StrLen];
          SQLLEN StrLen_or_Ind;
        };

        static const int NumCols = 5;

        /**
        * Checks SQLTables succeeds for catalogs.
        */
        TEST_METHOD(GetCatalogsSucceeds) {
          auto stmtInfo = this->allocStatement();
          auto stmt = std::get<2>(stmtInfo);
          SQLRETURN result;
          DataBinding data[NumCols] = { 0 };
          
          for (int i = 0; i < NumCols; i++) {
            result = SQLBindCol(stmt, (SQLUSMALLINT)i + 1, SQL_C_CHAR, (SQLPOINTER)(data[i].TargetValuePtr), StrLen, &(data[i].StrLen_or_Ind));
            Assert::AreEqual(static_cast<SQLRETURN>(SQL_SUCCESS), result);
          }
          
          result = SQLTablesW(stmt, (SQLWCHAR*)SQL_ALL_CATALOGS, SQL_NTS, L"", SQL_NTS, L"", SQL_NTS, L"", SQL_NTS);
          Assert::AreEqual(static_cast<SQLRETURN>(SQL_SUCCESS), result);
          
          for (result = SQLFetch(stmt); result == SQL_SUCCESS; result = SQLFetch(stmt)) {
          }

          this->freeStatement(stmtInfo);
        }
      };
    }
  }
}