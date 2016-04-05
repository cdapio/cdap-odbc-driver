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
#include "ArgumentsParser.h"

#define REGEX_FUNCTION        L"^\\{fn ([^\\(]+)\\((.*)\\)\\}$"
#define REGEX_FUNCTION_START  L"^\\{fn "
#define REGEX_DATE            L"^\\{d ('[^']*')\\}"
#define REGEX_TIMESTAMP       L"^\\{ts ('[^']*')\\}"
#define REGEX_INTERVAL        L"^\\{INTERVAL ([+-] )*'[^']+' ([^\\}]+)\\}"
#define REGEX_GUID            L"^\\{guid '[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}'\\}"

Cask::CdapOdbc::ODBCEscapeSequenceParser::ODBCEscapeSequenceParser(
  const std::wstring& query)
  : query(query)
  , regexFunction(REGEX_FUNCTION)
  , regexFunctionBeginning(REGEX_FUNCTION_START)
  , regexDate(REGEX_DATE)
  , regexGuid(REGEX_GUID)
  , regexInterval(REGEX_INTERVAL)
  , regexTimestamp(REGEX_TIMESTAMP) {
}

size_t Cask::CdapOdbc::ODBCEscapeSequenceParser::resolveFunction(std::wstring& query, size_t pos_start, size_t pos_end) {
  std::wstring matched = query.substr(pos_start, pos_end - pos_start + 1);
  std::wsmatch match;
  ArgumentsParser argParser(match[2].str());
  std::vector<std::wstring> arguments;

  if (std::regex_search(matched, match, regexFunction)) {
    std::wstring found = std::wstring(match[1].str().c_str());
    if (
      /* Datetime functions */
      _wcsicmp(found.c_str(), L"year") == 0
      || _wcsicmp(found.c_str(), L"month") == 0
      || _wcsicmp(found.c_str(), L"day") == 0
      || _wcsicmp(found.c_str(), L"hour") == 0
      || _wcsicmp(found.c_str(), L"minute") == 0
      || _wcsicmp(found.c_str(), L"second") == 0
      || _wcsicmp(found.c_str(), L"dayofmonth") == 0

      /* String functions */
      || _wcsicmp(found.c_str(), L"ascii") == 0
      || _wcsicmp(found.c_str(), L"concat") == 0
      || _wcsicmp(found.c_str(), L"lcase") == 0
      || _wcsicmp(found.c_str(), L"length") == 0
      || _wcsicmp(found.c_str(), L"locate") == 0
      || _wcsicmp(found.c_str(), L"ltrim") == 0
      || _wcsicmp(found.c_str(), L"repeat") == 0
      || _wcsicmp(found.c_str(), L"rtrim") == 0
      || _wcsicmp(found.c_str(), L"space") == 0
      || _wcsicmp(found.c_str(), L"substring") == 0
      || _wcsicmp(found.c_str(), L"ucase") == 0

      /* Numeric functions */
      || _wcsicmp(found.c_str(), L"abs") == 0
      || _wcsicmp(found.c_str(), L"acos") == 0
      || _wcsicmp(found.c_str(), L"asin") == 0
      || _wcsicmp(found.c_str(), L"atan") == 0
      || _wcsicmp(found.c_str(), L"ceiling") == 0
      || _wcsicmp(found.c_str(), L"cos") == 0
      || _wcsicmp(found.c_str(), L"degrees") == 0
      || _wcsicmp(found.c_str(), L"exp") == 0
      || _wcsicmp(found.c_str(), L"floor") == 0
      || _wcsicmp(found.c_str(), L"log10") == 0
      || _wcsicmp(found.c_str(), L"pi") == 0
      || _wcsicmp(found.c_str(), L"power") == 0
      || _wcsicmp(found.c_str(), L"radians") == 0
      || _wcsicmp(found.c_str(), L"rand") == 0
      || _wcsicmp(found.c_str(), L"round") == 0
      || _wcsicmp(found.c_str(), L"sign") == 0
      || _wcsicmp(found.c_str(), L"sin") == 0
      || _wcsicmp(found.c_str(), L"sqrt") == 0
      || _wcsicmp(found.c_str(), L"tan") == 0) {
      matched = std::regex_replace(matched, regexFunction, L"$1($2)");
    } else if (_wcsicmp(found.c_str(), L"mod") == 0) {
      matched = std::regex_replace(matched, regexFunction, L"pmod($2)");
    } else if (_wcsicmp(found.c_str(), L"replace") == 0) {
      matched = std::regex_replace(matched, regexFunction, L"translate($2)");
    } else if (_wcsicmp(found.c_str(), L"cot") == 0) {
      matched = std::regex_replace(matched, regexFunction, L"CAST(1.0/(tan$2) as DOUBLE)");
    } else if (_wcsicmp(found.c_str(), L"atan2") == 0) {
      arguments = argParser.GetArguments();
      if (arguments.size() != 2) {
        throw CdapException(L"Unable to parse function arguments: " + std::wstring(found.c_str()) + L".");
      }
      matched = L"atan((" + arguments[1] + L") / (" + arguments[0] + L"))";
    } else if (_wcsicmp(found.c_str(), L"now") == 0) {
      matched = std::regex_replace(matched, regexFunction, L"from_unixtime(unix_timestamp())");
    } else if (_wcsicmp(found.c_str(), L"left") == 0) {
      arguments = argParser.GetArguments();
      if (arguments.size() != 2) {
        throw CdapException(L"Unable to parse function arguments: " + std::wstring(found.c_str()) + L".");
      }

      matched = L"substr(" + arguments[0] + L", 0, " + arguments[1] + L")";
    } else if (_wcsicmp(found.c_str(), L"left") == 0) {
      arguments = argParser.GetArguments();
      if (arguments.size() != 2) {
        throw CdapException(L"Unable to parse function arguments: " + std::wstring(found.c_str()) + L".");
      }
      matched = L"substr(" + arguments[0] + L", 0, " + arguments[1] + L")";
    } else {
      throw CdapException(L"Not supported function in escape sequence: " + std::wstring(found.c_str())
                          + L".");
    }
  } else {
    throw CdapException(L"Encountered malformed scalar function escape sequence:\n" + matched);
  }

  query = query.replace(pos_start, pos_end - pos_start + 1, matched);
  return pos_start + matched.length();
}

size_t Cask::CdapOdbc::ODBCEscapeSequenceParser::resolveTimestamp(std::wstring& query, std::wsmatch match, size_t pos_start) {
  size_t resolveLen = match[1].length() + 9 + 2; // + TIMESTAMP(...)
  query.replace(pos_start, std::wstring::npos, match.format(L"TIMESTAMP($1)") + match.suffix().str());
  return pos_start + resolveLen;
}

size_t Cask::CdapOdbc::ODBCEscapeSequenceParser::resolveDate(std::wstring& query, std::wsmatch match, size_t pos_start) {
  size_t resolveLen = match[1].length() + 4 + 2; // + DATE(...)
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
  size_t i = 0;
  bool insideLiteral = false;
  wchar_t currentQuote;
  std::vector<size_t> fnOpenings = std::vector<size_t>();

  while (i < result.length()) {
    /* Check current position w.r.t. literal */
    if (insideLiteral) {
      if ((result[i] == currentQuote) && !isEscapedQuote(result, i)) {
        /* Exit literal */
        insideLiteral = false;
      }

      i++;
      continue;
    } else if (!insideLiteral && isQuote(result, i) && !isEscapedQuote(result, i)) {
      /* Enter literal */
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
      } else if (std::regex_search(subresult, match, this->regexInterval)) {
        i = this->resolveInterval(result, match, i); // <-- This throws exception, since not supported.
      } else if (std::regex_search(subresult, match, this->regexDate)) {
        i = this->resolveDate(result, match, i);
      } else if (std::regex_search(subresult, match, this->regexTimestamp)) {
        i = this->resolveTimestamp(result, match, i);
      } else if (std::regex_search(subresult, match, this->regexFunctionBeginning)) {
        fnOpenings.push_back(i);
        i += 4;
      } else {
        throw CdapException(L"Unrecognised ODBC escape sequence.");
      }
    } else if (result[i] == L'}') {
      if (fnOpenings.size() == 0) {
        throw CdapException(L"Unmatched } in the query at position " + std::to_wstring(i) + L".");
      }

      i = this->resolveFunction(result, fnOpenings.back(), i);
      fnOpenings.pop_back();
    } else {
      i++;
    }
  } // while(i < result.length())

  return result;
}
