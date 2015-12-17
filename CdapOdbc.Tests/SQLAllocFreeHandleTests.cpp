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
       * Tests for SQLAllocHandle and SQLFreeHandle functions.
       */
      TEST_CLASS(SQLAllocFreeHandleTests) {
      public:

        /**
         * Ensures that passing NULL as output handle argument fails with error.
         */
        TEST_METHOD(NullOutputHandlePtrFails) {
          SQLRETURN result = SQLAllocHandle(SQL_HANDLE_ENV, nullptr, nullptr);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_ERROR));
        }

        /**
         * Ensures that SQLAllocHandle fails if handle type is invalid.
         */
        TEST_METHOD(InvalidHandleTypeFails) {
          SQLHANDLE outHandle = nullptr;
          SQLSMALLINT badType = 300;
          SQLRETURN result = SQLAllocHandle(badType, nullptr, &outHandle);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_ERROR));
        }

        /**
         * Ensures that an environment can be successfully allocated and freed.
         */
        TEST_METHOD(AllocAndFreeEnvironmentSucceeds) {
          SQLHENV env = nullptr;
          SQLRETURN result = SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &env);

          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
          Assert::IsNotNull(env);

          result = SQLFreeHandle(SQL_HANDLE_ENV, env);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
        }

        /**
         * Ensures that a connection can be successfully allocated and freed.
         */
        TEST_METHOD(AllocAndFreeConnectionSucceeds) {
          SQLHENV env = nullptr;
          SQLRETURN result = SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &env);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
          Assert::IsNotNull(env);

          SQLHDBC dbc = nullptr;
          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
          Assert::IsNotNull(dbc);

          result = SQLFreeHandle(SQL_HANDLE_DBC, dbc);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));

          result = SQLFreeHandle(SQL_HANDLE_ENV, env);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
        }

        /**
         * Ensures that a statememnt can be successfully allocated and freed.
         */
        TEST_METHOD(AllocAndFreeStatementSucceeds) {
          SQLHENV env = nullptr;
          SQLRETURN result = SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &env);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
          Assert::IsNotNull(env);

          SQLHDBC dbc = nullptr;
          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
          Assert::IsNotNull(dbc);

          SQLHSTMT stmt = nullptr;
          result = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
          Assert::IsNotNull(stmt);

          result = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));

          result = SQLFreeHandle(SQL_HANDLE_DBC, dbc);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));

          result = SQLFreeHandle(SQL_HANDLE_ENV, env);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
        }

        /**
         * Ensures that a descriptor can be successfully allocated and freed.
         */
        TEST_METHOD(AllocAndFreeDescriptorSucceeds) {
          SQLHENV env = nullptr;
          SQLRETURN result = SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &env);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
          Assert::IsNotNull(env);

          SQLHDBC dbc = nullptr;
          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
          Assert::IsNotNull(dbc);

          SQLHDESC desc = nullptr;
          result = SQLAllocHandle(SQL_HANDLE_DESC, dbc, &desc);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
          Assert::IsNotNull(desc);

          result = SQLFreeHandle(SQL_HANDLE_DESC, desc);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));

          result = SQLFreeHandle(SQL_HANDLE_DBC, dbc);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));

          result = SQLFreeHandle(SQL_HANDLE_ENV, env);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_SUCCESS));
        }

        /**
         * Ensures that passing an invalid handle fails with error.
         */
        TEST_METHOD(AllocPassInvalidHandleFails) {
          SQLHANDLE handle = nullptr;
          SQLHANDLE badHandle = reinterpret_cast<void*>(1);
          SQLRETURN result = SQLAllocHandle(SQL_HANDLE_ENV, badHandle, &handle);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_ERROR));

          result = SQLAllocHandle(SQL_HANDLE_DBC, nullptr, &handle);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_ERROR));

          result = SQLAllocHandle(SQL_HANDLE_STMT, nullptr, &handle);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_ERROR));

          result = SQLAllocHandle(SQL_HANDLE_DESC, nullptr, &handle);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_ERROR));
        }

        /**
        * Ensures that passing NULL handle to SQLFreeHandle fails with error.
        */
        TEST_METHOD(FreeNullHandleFails) {
          SQLRETURN result = SQLFreeHandle(SQL_HANDLE_ENV, nullptr);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_INVALID_HANDLE));

          result = SQLFreeHandle(SQL_HANDLE_DBC, nullptr);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_INVALID_HANDLE));

          result = SQLFreeHandle(SQL_HANDLE_STMT, nullptr);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_INVALID_HANDLE));

          result = SQLFreeHandle(SQL_HANDLE_DESC, nullptr);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_INVALID_HANDLE));
        }

        /**
        * Ensures that passing an invalid handle to SQLFreeHandle fails with error.
        */
        TEST_METHOD(FreeInvalidHandleFails) {
          SQLHANDLE badHandle = reinterpret_cast<void*>(1);
          SQLRETURN result = SQLFreeHandle(SQL_HANDLE_ENV, badHandle);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_INVALID_HANDLE));

          result = SQLFreeHandle(SQL_HANDLE_DBC, badHandle);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_INVALID_HANDLE));

          result = SQLFreeHandle(SQL_HANDLE_STMT, badHandle);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_INVALID_HANDLE));

          result = SQLFreeHandle(SQL_HANDLE_DESC, badHandle);
          Assert::AreEqual(result, static_cast<SQLRETURN>(SQL_INVALID_HANDLE));
        }
      };
    }
  }
}