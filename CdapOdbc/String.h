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
     * Utility class for manipulating strings.
     */
    class String {
      String() = delete;

    public:

      /**
       * Splits a string to tokens separated by delimiter character.
       */
      static void split(const std::wstring& str, wchar_t delim, std::vector<std::wstring>& tokens);

      /**
       * Removes whitespaces from the start and the end of a string.
       */
      static std::wstring trim(const std::wstring& str);

      /**
       * Makes stream name from table name.
       */
      static std::wstring makeStreamName(const std::wstring& streamName);

      /**
       * Makes table name from stream name.
       */
      static std::wstring makeTableName(const std::wstring& streamName);

      /**
       * Converts double to string with specified precision.
       */
      static std::wstring fromDouble(double value, int precision);
    };
  }
}