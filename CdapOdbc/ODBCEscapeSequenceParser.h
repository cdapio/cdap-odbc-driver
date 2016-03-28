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
    class ODBCEscapeSequenceParser {

      ODBCEscapeSequenceParser(const ODBCEscapeSequenceParser&) = delete;
      void operator=(const ODBCEscapeSequenceParser&) = delete;

      std::wstring query;

      std::wregex regexFunction;
      std::wregex regexFunctionBeginning;

      std::wregex regexTimestamp;
      std::wregex regexDate;
      std::wregex regexInterval;
      std::wregex regexGuid;

      size_t resolveFunction(std::wstring& query, std::wsmatch match, size_t pos_start, size_t pos_end);
      size_t resolveTimestamp(std::wstring& query, std::wsmatch match, size_t pos_start);
      size_t resolveDate(std::wstring& query, std::wsmatch match, size_t pos_start);
      size_t resolveInterval(std::wstring& query, std::wsmatch match, size_t pos_start);
      size_t resolveGuid(std::wstring& query, std::wsmatch match, size_t pos_start);
      bool isEscapedQuote(std::wstring query, size_t pos);
      bool isQuote(std::wstring query, size_t pos);

    public:

      /**
       * Creates an instance of the parser
       */
      ODBCEscapeSequenceParser(const std::wstring& query);

      /**
       * Parses the query and returns it with resolved escape sequences
       */
      std::wstring toString();

    };
  }
}
