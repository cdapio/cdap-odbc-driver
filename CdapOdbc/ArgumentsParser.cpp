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
#include "ArgumentsParser.h"
#include "CdapException.h"
#include "SecureString.h"

Cask::CdapOdbc::ArgumentsParser::ArgumentsParser(
  const std::wstring& arguments)
  : arguments(arguments) {
}

bool Cask::CdapOdbc::ArgumentsParser::isEscapedQuote(std::wstring query, size_t pos) {
  return ((pos > 0) && query[pos] == L'\"' && query[pos - 1] == L'\\');
}

bool Cask::CdapOdbc::ArgumentsParser::isQuote(std::wstring query, size_t pos) {
  return (query[pos] == L'\'' || query[pos] == L'\"');
}

bool Cask::CdapOdbc::ArgumentsParser::isNestedBeginning(std::wstring query, size_t pos) {
  return (query[pos] == L'(');
}

bool Cask::CdapOdbc::ArgumentsParser::isNestedEnding(std::wstring query, size_t pos) {
  return (query[pos] == L')');
}

bool Cask::CdapOdbc::ArgumentsParser::isNestedMatched(wchar_t begin, wchar_t end) {
  return (begin == L'(' && end == L')');
}

std::vector<std::wstring> Cask::CdapOdbc::ArgumentsParser::getArguments() {
  std::vector<std::wstring> result;
  std::wstring buffer = L"";

  std::wstring args = std::wstring(this->arguments);
  args = String::trim(args);

  wchar_t currentQuote;
  std::vector<wchar_t> nestedStack;

  bool insideLiteral = false;
  bool insideNested = false;

  for (size_t i = 0; i < args.length(); i++) {
    /*
     * Check current position w.r.t. literal
     * Everything inside single or double matched quotes is treated as literal -- and skipped
     */
    if (!insideLiteral && !insideNested && args[i] == L',') {
      result.push_back(buffer);
      buffer = std::wstring();
      continue;
    }

    if (insideLiteral) {
      if ((args[i] == currentQuote) && !isEscapedQuote(args, i)) {
        /* Exit literal */
        insideLiteral = false;
      }
    } else if (isQuote(args, i) && !isEscapedQuote(args, i)) {
      /* Enter literal */
      insideLiteral = true;
      currentQuote = args[i];
    } else if (isNestedBeginning(args, i)) {
      nestedStack.push_back(args[i]);
    } else if (isNestedEnding(args, i)) {
      if (!nestedStack.empty() && isNestedMatched(nestedStack.back(), args[i])) {
        nestedStack.pop_back();
      } else {
        throw CdapException(L"Unmatching closing brace ')' at position: "
          + std::to_wstring(i)
          + L".");
      }
    } else if (!nestedStack.empty()) {
      if (isNestedMatched(nestedStack.back(), args[i])) {
        nestedStack.pop_back();
      }
    } else if (isNestedBeginning(args, i)) {
      nestedStack.push_back(args[i]);
    }

    buffer.push_back(args[i]);
  } // for

  if (!buffer.empty()) {
    result.push_back(buffer);
  }

  return result;
}