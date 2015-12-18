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
#include <array>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Cask {
  namespace CdapOdbc {
    namespace Tests {
      std::array<LPCSTR, 37> functions =
      {
        "SQLAllocHandle",
        "SQLConnectA",
        "SQLConnectW",
        "SQLGetTypeInfoA",
        "SQLGetTypeInfoW",
        "SQLDataSourcesA",
        "SQLDataSourcesW",
        "SQLPrepareA",
        "SQLPrepareW",
        "SQLBindParameter",
        "SQLExecute",
        "SQLExecDirectA",
        "SQLExecDirectW",
        "SQLNumParams",
        "SQLParamData",
        "SQLPutData",
        "SQLNumResultCols",
        "SQLDescribeColA",
        "SQLDescribeColW",
        "SQLColAttributeA",
        "SQLColAttributeW",
        "SQLBindCol",
        "SQLFetch",
        "SQLGetData",
        "SQLSetPos",
        "SQLMoreResults",
        "SQLColumnsA",
        "SQLColumnsW",
        "SQLPrimaryKeysA",
        "SQLPrimaryKeysW",
        "SQLTablesA",
        "SQLTablesW",
        "SQLFreeStmt",
        "SQLCloseCursor",
        "SQLCancel",
        "SQLDisconnect",
        "SQLFreeHandle",
      };

      /**
       * Tests for validating library exports.
       */
      TEST_CLASS(LibraryTests) {
      public:

        /**
         * Ensures that library can be successfully loaded.
         */
        TEST_METHOD(LoadLibrarySucceeds) {
          HMODULE libraryHandle = LoadLibraryW(L"CdapOdbc.dll");
          Assert::IsNotNull(libraryHandle);
          FreeLibrary(libraryHandle);
        }

        /**
         * Ensures that every exported function can be loaded.
         */
        TEST_METHOD(GetFunctionsSucceeds) {
          HMODULE libraryHandle = LoadLibraryW(L"CdapOdbc.dll");
          Assert::IsNotNull(libraryHandle);

          FARPROC proc = nullptr;

          for (auto& item : functions) {
            proc = GetProcAddress(libraryHandle, item);
            Assert::IsNotNull(proc);
          }

          FreeLibrary(libraryHandle);
        }
      };
    }
  }
}