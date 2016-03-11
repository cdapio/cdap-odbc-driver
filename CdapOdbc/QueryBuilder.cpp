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
#include "QueryBuilder.h"
#include "CdapException.h"

// Matches "{fn …}"
#define FUNCTIONS_REGEX L"\\{\\s*[Ff][Nn]\\s+([^\\(]+)([^\\{\\}]+)\\}"

Cask::CdapOdbc::QueryBuilder::QueryBuilder(const std::wstring& query)
  : functionsRegex(FUNCTIONS_REGEX)
  , query(query) {
}

std::wstring Cask::CdapOdbc::QueryBuilder::toString() {
  std::wstring result = this->query;
  std::wsmatch match;

  // Replaces found escape sequence {fn ...} entirely
  // That's why search starts again from the beginning of the string
  while (std::regex_search(result, match, this->functionsRegex)) {
    if (_wcsicmp(match[1].str().c_str(), L"lcase") == 0) {
      result = std::regex_replace(result, this->functionsRegex, L"LOWER$2");
    } else if (_wcsicmp(match[1].str().c_str(), L"ucase")) {
      result = std::regex_replace(result, this->functionsRegex, L"UPPER$2");
    } else {
      throw CdapException(L"Not supported escape sequence.");
    }
  }

  return result;
}
