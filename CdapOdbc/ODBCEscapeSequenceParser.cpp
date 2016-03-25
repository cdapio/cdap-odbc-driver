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
#include "ODBCEscapeSequenceParser.h"
#include "CdapException.h"

#define REGEX_FUNCTION        L"^\\{fn ([^\\(]+)([^\\{\\}]+)\\}" // TODO: change inside to [.*]\\}_end_of_multiline_string_
#define REGEX_FUNCTION_START  L"^\\{fn "
#define REGEX_DATE            L"^\\{d ('[^']*')\\}"
#define REGEX_TIMESTAMP       L"^\\{ts ('[^']*')\\}"
#define REGEX_INTERVAL        L"^\\{INTERVAL ([+-] )*'[^']+' ([^\\}]+)\\}"
#define REGEX_GUID            L"^\\{guid '[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}'\\}"

Cask::CdapOdbc::ODBCEscapeSequenceParser::ODBCEscapeSequenceParser(
  const std::wstring& query)
  : query(query)
  , regexFunction(REGEX_FUNCTION)
  , regexFunctionBeginning(REGEX_FUNCTION_START)
  , regexDate(REGEX_DATE)
  , regexGuid(REGEX_GUID)
  , regexInterval(REGEX_INTERVAL)
  , regexTimestamp(REGEX_TIMESTAMP)
{
}

size_t Cask::CdapOdbc::ODBCEscapeSequenceParser::resolveFunction(std::wstring& query, std::wsmatch match, size_t pos_start, size_t pos_end) {
  return 0;
}

size_t Cask::CdapOdbc::ODBCEscapeSequenceParser::resolveTimestamp(std::wstring& query, std::wsmatch match, size_t pos_start) {
  size_t resolveLen = match[1].length() + 9 + 2; // TIMESTAMP(<literal>)
  query.replace(pos_start, std::wstring::npos, match.format(L"TIMESTAMP($1)") + match.suffix().str());
  return pos_start + resolveLen;
}

size_t Cask::CdapOdbc::ODBCEscapeSequenceParser::resolveDate(std::wstring& query, std::wsmatch match, size_t pos_start) {
  size_t resolveLen = match[1].length() + 4 + 2; // date(<literal>)
  query.replace(pos_start, std::wstring::npos, match.format(L"DATE($1)") + match.suffix().str());
  return pos_start + resolveLen;
}

size_t Cask::CdapOdbc::ODBCEscapeSequenceParser::resolveInterval(std::wstring& query, std::wsmatch match, size_t pos_start) {
  throw CdapException(L"Escape sequence {INTERVAL ...} is not supported.\n");
  return 0;
}

size_t Cask::CdapOdbc::ODBCEscapeSequenceParser::resolveGuid(std::wstring& query, std::wsmatch match, size_t pos_start) {
  throw CdapException(L"Escape sequence {GUID ...} is not supported.\n");
  return 0;
}

bool Cask::CdapOdbc::ODBCEscapeSequenceParser::isEscapedQuote(std::wstring query, size_t pos) {
  return ((pos > 0) && query[pos] == L'\"' && query[pos - 1] == L'\\');
}

bool Cask::CdapOdbc::ODBCEscapeSequenceParser::isQuote(std::wstring query, size_t pos) {
  return (query[pos] == L'\'' || query[pos] == L'\"');
}

std::wstring Cask::CdapOdbc::ODBCEscapeSequenceParser::toString() {
  std::wstring result = query;
  std::wstring subresult;
  std::wsmatch match;
  size_t i = 0, resolvedLength;
  bool insideLiteral = false;
  wchar_t currentQuote;
  std::vector<size_t> fn_openings = std::vector<size_t>();

  while (i < result.length()) {
    /* Check current position w.r.t. literal */
    if (insideLiteral) {
      if ((result[i] == currentQuote) && !isEscapedQuote(result, i)) {
        // exit literal
        insideLiteral = false;
      }
      i++;
      continue;
    }
    else if (!insideLiteral && isQuote(result, i) && !isEscapedQuote(result, i)) {
      // enter literal
      insideLiteral = true;
      currentQuote = result[i];
      i++;
      continue;
    }
    /* Check for escape sequences */
    if (result[i] == L'{') {
      subresult = result.substr(i);
      /* At first try fixed-length non-nested  */
      if (std::regex_search(subresult, match, this->regexGuid)) {
        i = this->resolveGuid(result, match, i); // <-- This throws exception, since not supported.
      }
      else if (std::regex_search(subresult, match, this->regexInterval)) {
        i = this->resolveInterval(result, match, i); // <-- This throws exception, since not supported.
      }
      else if (std::regex_search(subresult, match, this->regexDate)) {
        i = this->resolveDate(result, match, i);
      }
      else if (std::regex_search(subresult, match, this->regexTimestamp)) {
        i = this->resolveTimestamp(result, match, i);
      }
      else if (std::regex_search(subresult, match, this->regexFunctionBeginning)) {
        fn_openings.push_back(i);
        i += 4;
      }
    }
    else if (result[i] == L'}') {
      if (fn_openings.size() == 0) {
        throw CdapException(L"Unmatched } in the query at position " + std::to_wstring(i) + L".");
      }
      i = this->resolveFunction(subresult, match, fn_openings.back(), i);
      fn_openings.pop_back();
    }
    else {
      i++;
    }
  } // while(i < result.length())

  return result;
}
