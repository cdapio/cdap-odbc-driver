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

#include "String.h"

namespace Cask {
  namespace CdapOdbc {

    /**
     * Utility class for converting arguments from and to SQL formats.
     */
    class Argument {
      Argument() = delete;

    public:

      /**
       * Converts SQL wide string to std::string.
       * Used for input arguments.
       */
      template <typename T>
      inline static std::unique_ptr<T> toStdString(SQLWCHAR* string, SQLSMALLINT length) {
        if (string == nullptr) {
          return nullptr;
        } else if (length == SQL_NTS) {
          return std::make_unique<T>(reinterpret_cast<const wchar_t*>(string));
        } else {
          return std::make_unique<T>(reinterpret_cast<const wchar_t*>(string), static_cast<size_t>(length));
        }
      }

      /**
       * Copies std::wstring to SQL wide string.
       * Used for output arguments.
       */
      static void fromStdString(const std::wstring& input, SQLWCHAR* outString, SQLSMALLINT bufferLength, SQLSMALLINT* stringLengthPtr);

      /**
       * Copies SecureString to SQL wide string.
       * Used for output arguments.
       */
      static void fromSecureString(const SecureString& input, SQLWCHAR* outString, SQLSMALLINT bufferLength, SQLSMALLINT* stringLengthPtr);
    };
  }
}