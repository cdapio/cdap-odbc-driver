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

#pragma once

namespace Cask {
  namespace CdapOdbc {

    /**
      Utility class for converting arguments from and to SQL formats.
    */
    class Argument {
      Argument() = delete;

    public:

      /**
        Converts SQL string to std::string.
    Used for input arguments.
      */
      static std::string toStdString(SQLCHAR* string, SQLSMALLINT length);

      /**
        Converts SQL wide string to std::string.
      Used for input arguments.
      */
      static std::string toStdString(SQLWCHAR* string, SQLSMALLINT length);

      /**
      Copies std::string to SQL string.
      Used for output arguments.
      */
      static void fromStdString(const std::string& input, SQLCHAR* outConnectionString, SQLSMALLINT bufferLength, SQLSMALLINT* stringLengthPtr);

      /**
      Copies std::string to SQL wide string.
      Used for output arguments.
      */
      static void fromStdString(const std::string& input, SQLWCHAR* outConnectionString, SQLSMALLINT bufferLength, SQLSMALLINT* stringLengthPtr);

      /**
      Copies std::wstring to SQL wide string.
      Used for output arguments.
      */
      static void fromStdString(const std::wstring& input, SQLWCHAR* outConnectionString, SQLSMALLINT bufferLength, SQLSMALLINT* stringLengthPtr);
    };
  }
}