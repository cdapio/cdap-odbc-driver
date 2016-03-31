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
#include "ODBCEscapeSequenceParser.h"
#include "CdapException.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Cask {
  namespace CdapOdbc {
    namespace Tests {

      /**
       * Tests for OdbcEscapeSequenceParser class.
       */
      TEST_CLASS(OdbcEscapeSequenceParserTest) {
      public:
        
        /**
         * Checks that date escape sequence succeeds.
         */
        TEST_METHOD(ResolveDateSucceeds) {
          auto query = std::wstring(L"SELECT {d '2007 - 01 - 01'}");
          ODBCEscapeSequenceParser parser(query);
          auto actual = parser.toString();
          Assert::AreEqual(L"SELECT DATE('2007 - 01 - 01')", actual.c_str());
        }

        /**
         * Checks that timestamp escape sequence succeeds.
         */
        TEST_METHOD(ResolveTimestampSucceeds) {
          auto query = std::wstring(L"SELECT {ts '1998-05-02 01:23:56.123'}");
          ODBCEscapeSequenceParser parser(query);
          auto actual = parser.toString();
          Assert::AreEqual(L"SELECT TIMESTAMP('1998-05-02 01:23:56.123')", actual.c_str());
        }

        /**
         * Checks that function escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionSucceeds) {
          auto query = std::wstring(L"SELECT {fn UCASE(Name)}");
          ODBCEscapeSequenceParser parser(query);
          auto actual = parser.toString();
          Assert::AreEqual(L"SELECT UCASE(Name)", actual.c_str());
        }

        /**
         * Checks that interval escape sequence succeeds.
         */
        TEST_METHOD(ResolveIntervalSucceeds) {
          auto query = std::wstring(L"SELECT {INTERVAL '163' HOUR(2)}");
          ODBCEscapeSequenceParser parser(query);
          auto func = [&parser] { parser.toString(); };
          Assert::ExpectException<CdapException>(func);
        }

        /**
         * Checks that guid escape sequence succeeds.
         */
        TEST_METHOD(ResolveGuidSucceeds) {
          auto query = std::wstring(L"SELECT {guid '94E53675-0C18-459C-B30B-B270376D6A05'}");
          ODBCEscapeSequenceParser parser(query);
          auto func = [&parser] { parser.toString(); };
          Assert::ExpectException<CdapException>(func);
        }
      };
    }
  }
}