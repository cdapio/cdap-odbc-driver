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

        void testResolving(const wchar_t* escapeSeq, const wchar_t* result) {
          ODBCEscapeSequenceParser parser(escapeSeq);
          auto actual = parser.toString();
          Assert::AreEqual(result, parser.toString().c_str());
        }

      public:
        
        /**
         * Checks that date escape sequence succeeds.
         */
        TEST_METHOD(ResolveDateSucceeds) {
          this->testResolving(
            L"SELECT {d '2007 - 01 - 01'}", 
            L"SELECT DATE('2007 - 01 - 01')");
        }

        /**
         * Checks that timestamp escape sequence succeeds.
         */
        TEST_METHOD(ResolveTimestampSucceeds) {
          this->testResolving(
            L"SELECT {ts '1998-05-02 01:23:56.123'}", 
            L"SELECT TIMESTAMP('1998-05-02 01:23:56.123')");
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

        /**
         * Checks that function ASCII escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionASCIISucceeds) {
          this->testResolving(
            L"SELECT {fn ASCII(35)}",
            L"SELECT ASCII(35)");
        }

        /**
         * Checks that function CONCAT escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionCONCATSucceeds) {
          this->testResolving(
            L"SELECT {fn CONCAT(firstname, lastname)}",
            L"SELECT CONCAT(firstname, lastname)");
        }

        /**
         * Checks that function LCASE escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionLCASESucceeds) {
          this->testResolving(
            L"SELECT {fn LCASE(name)}",
            L"SELECT LCASE(name)");
        }

        /**
         * Checks that function LEFT escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionLEFTSucceeds) {
          this->testResolving(
            L"SELECT {fn LEFT(firstname, 10)}",
            L"SELECT substr(firstname, 0,  10)");
        }

        /**
         * Checks that function LENGTH escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionLENGTHSucceeds) {
          this->testResolving(
            L"SELECT {fn LENGTH(name)}",
            L"SELECT LENGTH(name)");
        }

        /**
         * Checks that function LOCATE escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionLOCATESucceeds) {
          this->testResolving(
            L"SELECT {fn LOCATE(name, 'a')}",
            L"SELECT LOCATE(name, 'a')");
        }

        /**
         * Checks that function LTRIM escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionLTRIMSucceeds) {
          this->testResolving(
            L"SELECT {fn LTRIM(name)}",
            L"SELECT LTRIM(name)");
        }

        /**
         * Checks that function REPEAT escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionREPEATSucceeds) {
          this->testResolving(
            L"SELECT {fn REPEAT('1', 10)}",
            L"SELECT REPEAT('1', 10)");
        }

        /**
         * Checks that function REPLACE escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionREPLACESucceeds) {
          this->testResolving(
            L"SELECT {fn REPLACE(name, 'a', 'b')}",
            L"SELECT translate(name, 'a', 'b')");
        }

        /**
         * Checks that function RTRIM escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionRTRIMSucceeds) {
          this->testResolving(
            L"SELECT {fn RTRIM(name)}",
            L"SELECT RTRIM(name)");
        }

        /**
         * Checks that function SPACE escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionSPACESucceeds) {
          this->testResolving(
            L"SELECT {fn SPACE(10)}",
            L"SELECT SPACE(10)");
        }

        /**
         * Checks that function SUBSTRING escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionSUBSTRINGSucceeds) {
          this->testResolving(
            L"SELECT {fn SUBSTRING(name, 10, 5)}",
            L"SELECT SUBSTRING(name, 10, 5)");
        }

        /**
         * Checks that function UCASE escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionUCASESucceeds) {
          this->testResolving(
            L"SELECT {fn UCASE(Name)}",
            L"SELECT UCASE(Name)");
        }

        /**
         * Checks that function ABS escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionABSSucceeds) {
          this->testResolving(
            L"SELECT {fn ABS(-10)}",
            L"SELECT ABS(-10)");
        }

        /**
         * Checks that function ACOS escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionACOSSucceeds) {
          this->testResolving(
            L"SELECT {fn ACOS(angle)}",
            L"SELECT ACOS(angle)");
        }

        /**
         * Checks that function ASIN escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionASINSucceeds) {
          this->testResolving(
            L"SELECT {fn ASIN(angle)}",
            L"SELECT ASIN(angle)");
        }

        /**
         * Checks that function ATAN escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionATANSucceeds) {
          this->testResolving(
            L"SELECT {fn ATAN(angle)}",
            L"SELECT ATAN(angle)");
        }

        /**
         * Checks that function CEILING escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionCEILINGSucceeds) {
          this->testResolving(
            L"SELECT {fn CEILING(value)}",
            L"SELECT CEILING(value)");
        }

        /**
         * Checks that function COS escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionCOSSucceeds) {
          this->testResolving(
            L"SELECT {fn COS(value)}",
            L"SELECT COS(value)");
        }

        /**
         * Checks that function COT escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionCOTSucceeds) {
          this->testResolving(
            L"SELECT {fn COT(value)}",
            L"SELECT CAST(1.0/tan(value) as DOUBLE)");
        }

        /**
         * Checks that function DEGREES escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionDEGREESSucceeds) {
          this->testResolving(
            L"SELECT {fn DEGREES(value)}",
            L"SELECT DEGREES(value)");
        }

        /**
         * Checks that function EXP escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionEXPSucceeds) {
          this->testResolving(
            L"SELECT {fn EXP(value)}",
            L"SELECT EXP(value)");
        }

        /**
         * Checks that function FLOOR escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionFLOORSucceeds) {
          this->testResolving(
            L"SELECT {fn FLOOR(value)}",
            L"SELECT FLOOR(value)");
        }

        /**
         * Checks that function LOG10 escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionLOG10Succeeds) {
          this->testResolving(
            L"SELECT {fn LOG10(value)}",
            L"SELECT LOG10(value)");
        }

        /**
         * Checks that function PI escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionPISucceeds) {
          this->testResolving(
            L"SELECT {fn PI(value)}",
            L"SELECT PI(value)");
        }

        /**
         * Checks that function POWER escape sequence succeeds.
         */
        TEST_METHOD(ResolveFunctionPOWERSucceeds) {
          this->testResolving(
            L"SELECT {fn POWER(value)}",
            L"SELECT POWER(value)");
        }
      };
    }
  }
}