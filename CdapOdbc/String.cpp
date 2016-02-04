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
#include "String.h"

void Cask::CdapOdbc::String::split(const std::wstring & str, wchar_t delim, std::vector<std::wstring>& tokens) {
  std::wstringstream stream(str);
  std::wstring item;
  while (std::getline(stream, item, delim)) {
    tokens.push_back(item);
  }
}

std::wstring Cask::CdapOdbc::String::trim(const std::wstring & str) {
  std::wstring result = str;

  // Trim trailing spaces
  size_t endPos = result.find_last_not_of(L" \t");
  if (std::wstring::npos != endPos) {
    result = result.substr(0, endPos + 1);
  }

  // Trim leading spaces
  size_t startPos = result.find_first_not_of(L" \t");
  if (std::wstring::npos != startPos) {
    result = result.substr(startPos);
  }

  return result;
}

std::wstring Cask::CdapOdbc::String::makeStreamName(const std::wstring& streamName) {
  return streamName.substr(7);
}

std::wstring Cask::CdapOdbc::String::makeTableName(const std::wstring& streamName) {
  return L"stream_" + streamName;
}

std::wstring Cask::CdapOdbc::String::fromDouble(double value, int precision) {
  std::wstringstream s;
  s << std::setprecision(precision) << value;
  return s.str();
}
