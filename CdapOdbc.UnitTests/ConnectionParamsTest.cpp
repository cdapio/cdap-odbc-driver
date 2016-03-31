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
#include "ConnectionParams.h"
#include "SecureString.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Cask {
  namespace CdapOdbc {
    namespace Tests {

      /**
       * Tests for ConnectionParams class.
       */
      TEST_CLASS(ConnectionParamsTest) {
      public:
        
        /**
         * Checks that parsing connection string with most default params succeeds.
         */
        TEST_METHOD(ParseDefaultSucceeds) {
          SecureString connectionString(L"Driver=(Cask CDAP ODBC Driver);Host=localhost");
          ConnectionParams params(connectionString);
          Assert::AreEqual(L"(Cask CDAP ODBC Driver)", params.getDriver().c_str());
          Assert::AreEqual(L"", params.getAuthToken().c_str());
          Assert::AreEqual(L"", params.getDsn().c_str());
          Assert::AreEqual(L"localhost", params.getHost().c_str());
          Assert::AreEqual(L"default", params.getNamespace().c_str());
          Assert::AreEqual(10000, params.getPort());
          Assert::AreEqual(false, params.getSslEnabled());
          Assert::AreEqual(true, params.getVerifySslCert());
        }

        /**
         * Checks that parsing connection string succeeds.
         */
        TEST_METHOD(ParseSucceeds) {
          SecureString connectionString(L"Driver=(Cask CDAP ODBC Driver);Host=127.0.0.1;Port=999;Namespace=testspace;SSL_Enabled=true;Verify_Ssl_Cert=false;Auth_Token=0000FFF");
          ConnectionParams params(connectionString);

          Assert::AreEqual(L"(Cask CDAP ODBC Driver)", params.getDriver().c_str());
          Assert::AreEqual(L"0000FFF", params.getAuthToken().c_str());
          Assert::AreEqual(L"", params.getDsn().c_str());
          Assert::AreEqual(L"127.0.0.1", params.getHost().c_str());
          Assert::AreEqual(L"testspace", params.getNamespace().c_str());
          Assert::AreEqual(999, params.getPort());
          Assert::AreEqual(true, params.getSslEnabled());
          Assert::AreEqual(false, params.getVerifySslCert());
        }

        /**
        * Checks that set connection string succeeds.
        */
        TEST_METHOD(SetParamsSucceeds) {
          ConnectionParams params;
          params.setDriver(L"(Cask CDAP ODBC Driver)");
          params.setAuthToken(L"0000FFF");
          params.setHost(L"127.0.0.1");
          params.setNamespace(L"testspace");
          params.setPort(999);
          params.setSslEnabled(true);
          params.setVerifySslCert(false);

          Assert::AreEqual(L"(Cask CDAP ODBC Driver)", params.getDriver().c_str());
          Assert::AreEqual(L"0000FFF", params.getAuthToken().c_str());
          Assert::AreEqual(L"", params.getDsn().c_str());
          Assert::AreEqual(L"127.0.0.1", params.getHost().c_str());
          Assert::AreEqual(L"testspace", params.getNamespace().c_str());
          Assert::AreEqual(999, params.getPort());
          Assert::AreEqual(true, params.getSslEnabled());
          Assert::AreEqual(false, params.getVerifySslCert());
          Assert::AreEqual(L"Driver=(Cask CDAP ODBC Driver);Host=127.0.0.1;Port=999;Auth_Token=0000FFF;Namespace=testspace;SSL_Enabled=True;Verify_SSL_Cert=False;", params.getFullConnectionString().c_str());
        }
      };
    }
  }
}