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
#include "DataReader.h"
#include "Encoding.h"

void Cask::CdapOdbc::DataReader::fetchNull(const ColumnBinding& binding) {
  if (binding.getStrLenOrInd()) {
    *binding.getStrLenOrInd() = SQL_NULL_DATA;
  }
}

void Cask::CdapOdbc::DataReader::fetchString(const wchar_t* str, const ColumnBinding& binding) {
  assert(binding.getTargetType() == SQL_CHAR || binding.getTargetType() == SQL_DEFAULT);
  if (str) {
    std::string ansiStr = Encoding::toUtf8(str);
    size_t maxLength = static_cast<size_t>(binding.getBufferLength()) - 1;
    size_t size = (ansiStr.size() < maxLength) ? ansiStr.size() : maxLength;
    if (binding.getTargetValuePtr()) {
      char* outString = static_cast<char*>(binding.getTargetValuePtr());
      auto it = stdext::make_checked_array_iterator<char*>(outString, size);
      std::copy(ansiStr.begin(), ansiStr.begin() + size, it);
      outString[size] = 0;
    }

    if (binding.getStrLenOrInd()) {
      if (size <= maxLength) {
        *binding.getStrLenOrInd() = size;
      } else {
        *binding.getStrLenOrInd() = SQL_NO_TOTAL;
      }
    }
  } else {
    if (binding.getTargetValuePtr() && binding.getBufferLength() > 0) {
      char* outString = static_cast<char*>(binding.getTargetValuePtr());
      outString[0] = 0;
    }

    if (binding.getStrLenOrInd()) {
      *binding.getStrLenOrInd() = SQL_NULL_DATA;
    }
  }
}

void Cask::CdapOdbc::DataReader::fetchSmallint(SQLSMALLINT value, const ColumnBinding& binding) {
  assert(binding.getTargetType() == SQL_C_SSHORT || binding.getTargetType() == SQL_DEFAULT);
  *(reinterpret_cast<SQLSMALLINT*>(binding.getTargetValuePtr())) = value;
}

void Cask::CdapOdbc::DataReader::fetchInt(SQLINTEGER value, const ColumnBinding& binding) {
  assert(binding.getTargetType() == SQL_C_SLONG || binding.getTargetType() == SQL_DEFAULT);
  *(reinterpret_cast<SQLINTEGER*>(binding.getTargetValuePtr())) = value;
}
