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
#include "Argument.h"
#include "Encoding.h"

std::wstring Cask::CdapOdbc::Argument::toStdString(SQLWCHAR* string, SQLSMALLINT length) {
  if (length == SQL_NTS) {
    std::wstring temp(reinterpret_cast<const wchar_t*>(string));
    return temp;
  } else {
    std::wstring temp(reinterpret_cast<const wchar_t*>(string), static_cast<size_t>(length));
    return temp;
  }
}

void Cask::CdapOdbc::Argument::fromStdString(const std::wstring & input, SQLWCHAR * outString, SQLSMALLINT bufferLength, SQLSMALLINT * stringLengthPtr) {
  // Determine max output string length
  size_t maxLength = static_cast<size_t>(bufferLength) - 1;
  size_t size = (input.size() < maxLength) ? input.size() : maxLength;

  // Copy string to output buffer
  if (outString != nullptr) {
    auto it = stdext::make_checked_array_iterator<wchar_t*>(reinterpret_cast<wchar_t*>(outString), size);
    std::copy(input.begin(), input.begin() + size, it);
    outString[size] = 0;
  }

  // Fill stringLengthPtr even if outConnectionString is nullptr.
  if (stringLengthPtr != nullptr) {
    *stringLengthPtr = static_cast<SQLSMALLINT>(size);
  }
}
