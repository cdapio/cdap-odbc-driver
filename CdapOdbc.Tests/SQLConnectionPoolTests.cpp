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
       * Tests for SPI functions.
       * https://msdn.microsoft.com/en-us/library/hh405036(v=vs.85).aspx
       */
      TEST_CLASS(SQLConnectionPoolTests) {

      public:

        /**
         * Emulates Driver Manager calls for matching connections.
         */
        TEST_METHOD(CallsForMatchingConnectionsSucceed) {
          int result;
          SQLHENV env;
          SQLHDBC con;
          SQLHDBC_INFO_TOKEN infoToken;
          POOLID poolID = 0;
          SQLConnPoolRating rating = 0;

          // Open connection
          result = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &con);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLDriverConnectW(con, HWND_DESKTOP, TEST_CONNECTION_STRING, SQL_NTS, nullptr, 0, nullptr, SQL_DRIVER_NOPROMPT);
          Assert::AreEqual(SQL_SUCCESS, result);

          // Connection opened 
          // Lets' emulate pool-related calls
          result = SQLAllocHandle(SQL_HANDLE_DBC_INFO_TOKEN, SQL_NULL_HANDLE, &infoToken);
          Assert::AreEqual(SQL_SUCCESS, result);
          Assert::IsNotNull(infoToken);

          result = SQLSetConnectAttrForDbcInfoW(infoToken, SQL_ATTR_ASYNC_ENABLE, SQL_ASYNC_ENABLE_OFF, 0);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLSetConnectAttrForDbcInfoW(infoToken, SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE, SQL_ASYNC_DBC_ENABLE_OFF, 0);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLSetDriverConnectInfoW(infoToken, TEST_CONNECTION_STRING, SQL_NTS);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLGetPoolID(infoToken, &poolID);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLRateConnection(infoToken, con, SQL_FALSE, 0, &rating);
          Assert::AreEqual(SQL_SUCCESS, result);
          Assert::AreEqual(100, static_cast<int>(rating));

          // As connection similar Driver Manager would reuse it as is at this point

          // Cleanup
          result = SQLDisconnect(con);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFreeHandle(SQL_HANDLE_DBC_INFO_TOKEN, infoToken);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFreeHandle(SQL_HANDLE_DBC, con);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFreeHandle(SQL_HANDLE_ENV, env);
          Assert::AreEqual(SQL_SUCCESS, result);
        }

        /**
        * Emulates Driver Manager calls for PARTIALLY matching connections.
        */
        TEST_METHOD(CallsForPartiallyMatchingConnectionsSucceed) {
          int result;
          SQLHENV env;
          SQLHDBC con;
          SQLHDBC_INFO_TOKEN infoToken;
          POOLID poolID = 0;
          SQLConnPoolRating rating = 0;

          // Open connection
          result = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &con);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLDriverConnectW(con, HWND_DESKTOP, TEST_CONNECTION_STRING, SQL_NTS, nullptr, 0, nullptr, SQL_DRIVER_NOPROMPT);
          Assert::AreEqual(SQL_SUCCESS, result);

          // Connection opened 
          // Lets' emulate pool-related calls
          result = SQLAllocHandle(SQL_HANDLE_DBC_INFO_TOKEN, SQL_NULL_HANDLE, &infoToken);
          Assert::AreEqual(SQL_SUCCESS, result);
          Assert::IsNotNull(infoToken);

          // Set this attribute different
          result = SQLSetConnectAttrForDbcInfoW(infoToken, SQL_ATTR_ASYNC_ENABLE, reinterpret_cast<SQLPOINTER>(SQL_ASYNC_ENABLE_ON), 0);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLSetConnectAttrForDbcInfoW(infoToken, SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE, SQL_ASYNC_DBC_ENABLE_OFF, 0);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLSetDriverConnectInfoW(infoToken, TEST_CONNECTION_STRING, SQL_NTS);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLGetPoolID(infoToken, &poolID);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLRateConnection(infoToken, con, SQL_FALSE, 0, &rating);
          Assert::AreEqual(SQL_SUCCESS, result);
          Assert::AreEqual(50, static_cast<int>(rating));

          // As connection partially similar Driver Manager would reset connection
          result = SQLSetConnectAttrW(con, SQL_ATTR_DBC_INFO_TOKEN, infoToken, 0);
          Assert::AreEqual(SQL_SUCCESS, result);

          // Cleanup
          result = SQLDisconnect(con);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFreeHandle(SQL_HANDLE_DBC_INFO_TOKEN, infoToken);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFreeHandle(SQL_HANDLE_DBC, con);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFreeHandle(SQL_HANDLE_ENV, env);
          Assert::AreEqual(SQL_SUCCESS, result);
        }

        /**
         * Emulates Driver Manager calls for NOT matching connections.
         */
        TEST_METHOD(CallsForNotMatchingConnectionsSucceed) {
          int result;
          SQLHENV env;
          SQLHDBC con;
          SQLHDBC conNew;
          SQLHDBC_INFO_TOKEN infoToken;
          POOLID poolID = 0;
          SQLConnPoolRating rating = 0;

          // Open connection
          result = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &con);
          Assert::AreEqual(SQL_SUCCESS, result);

          // Check even if localhost REST service missing
          result = SQLDriverConnectW(con, HWND_DESKTOP, TEST_CONNECTION_STRING2, SQL_NTS, nullptr, 0, nullptr, SQL_DRIVER_NOPROMPT);
          Assert::AreEqual(SQL_SUCCESS, result);

          // Connection opened 
          // Let's emulate pool-related calls
          result = SQLAllocHandle(SQL_HANDLE_DBC_INFO_TOKEN, SQL_NULL_HANDLE, &infoToken);
          Assert::AreEqual(SQL_SUCCESS, result);
          Assert::IsNotNull(infoToken);

          result = SQLSetConnectAttrForDbcInfoW(infoToken, SQL_ATTR_ASYNC_ENABLE, SQL_ASYNC_ENABLE_OFF, 0);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLSetConnectAttrForDbcInfoW(infoToken, SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE, SQL_ASYNC_DBC_ENABLE_OFF, 0);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLSetDriverConnectInfoW(infoToken, TEST_CONNECTION_STRING, SQL_NTS);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLGetPoolID(infoToken, &poolID);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLRateConnection(infoToken, con, SQL_FALSE, 0, &rating);
          Assert::AreEqual(SQL_SUCCESS, result);
          Assert::AreEqual(0, static_cast<int>(rating));

          // As connection doesn't match Driver Manager would open new
          result = SQLAllocHandle(SQL_HANDLE_DBC, env, &conNew);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLPoolConnectW(conNew, infoToken, nullptr, 0, 0);
          Assert::AreEqual(SQL_SUCCESS, result);

          // Cleanup
          result = SQLDisconnect(conNew);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLDisconnect(con);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFreeHandle(SQL_HANDLE_DBC, conNew);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFreeHandle(SQL_HANDLE_DBC_INFO_TOKEN, infoToken);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFreeHandle(SQL_HANDLE_DBC, con);
          Assert::AreEqual(SQL_SUCCESS, result);

          result = SQLFreeHandle(SQL_HANDLE_ENV, env);
          Assert::AreEqual(SQL_SUCCESS, result);
        }
      };
    }
  }
}