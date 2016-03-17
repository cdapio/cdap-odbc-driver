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
#define FUNCTIONS_REGEX L"\\{\\s*[Ff][Nn]\\s+([^\\(]+)([^\\{\\}]+)\\s*\\}"
#define DATETIME_REGEX L"\\{([Tt][Ss]|[Dd]) ('[^']*')\\}"

Cask::CdapOdbc::QueryBuilder::QueryBuilder(
  const std::wstring& query)
  : functionsRegex(FUNCTIONS_REGEX)
  , datetimeRegex(DATETIME_REGEX)
  , query(query) {
}

std::wstring Cask::CdapOdbc::QueryBuilder::toString() {
  std::wstring result = this->query;
  std::wsmatch match;
  std::wstring found;

  // Replaces escape sequence {d ...} and {ts ...}
  while (std::regex_search(result, match, this->datetimeRegex)) {
    found = std::wstring(match[1].str().c_str());
    if (_wcsicmp(found.c_str(), L"d") == 0) {
      result = std::regex_replace(result, this->datetimeRegex, L"DATE($2)");
    }
    else if (_wcsicmp(found.c_str(), L"ts") == 0) {
      result = std::regex_replace(result, this->datetimeRegex, L"TIMESTAMP($2)");
    }
  }

  // Replaces found escape sequence {fn ...} entirely
  // That's why search starts again from the beginning of the string
  while (std::regex_search(result, match, this->functionsRegex)) {
    found = std::wstring(match[1].str().c_str());
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
		  || _wcsicmp(found.c_str(), L"tan") == 0
		  )
	  {
        result = std::regex_replace(result, this->functionsRegex, L"$1$2");
	  }
    else if (_wcsicmp(found.c_str(), L"mod") == 0) {
      result = std::regex_replace(result, this->functionsRegex, L"pmod$2");
    }
    else if (_wcsicmp(found.c_str(), L"cot") == 0) {
		  result = std::regex_replace(result, this->functionsRegex, L"CAST(1.0/(tan$2) as DOUBLE)");
	  }
    else if (_wcsicmp(found.c_str(), L"now") == 0) {
      result = std::regex_replace(result, this->functionsRegex, L"from_unixtime(unix_timestamp())");
    }
    else {
      throw CdapException(L"Not supported escape sequence.");
    }
  }

  return result;
}
