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
#include "String.h"

namespace {

  std::wstring doubleToWString(double value, const Cask::CdapOdbc::ColumnBinding& binding) {
    int maxLen = static_cast<int>(binding.getBufferLength()) - 1;
    auto result = Cask::CdapOdbc::String::fromDouble(value, maxLen - 1);
    if (result.size() > maxLen) {
      maxLen -= (static_cast<int>(result.size()) - maxLen);
      result = Cask::CdapOdbc::String::fromDouble(value, maxLen - 1);
    }

    return result;
  }

  std::wstring toWString(const web::json::value& value, const Cask::CdapOdbc::ColumnBinding& binding) {
    if (value.is_string()) {
      return value.as_string();
    } else if (value.is_number()) {
      const auto& number = value.as_number();
      if (number.is_int64()) {
        return std::to_wstring(number.to_int64());
      } else if (number.is_int32()) {
        return std::to_wstring(number.to_int32());
      } else {
        return doubleToWString(number.to_double(), binding);
      }
    } else if (value.is_double()) {
      return doubleToWString(value.as_double(), binding);
    } else if (value.is_boolean()) {
      return std::to_wstring(value.as_bool() ? 1 : 0);
    } else if (value.is_array()) {
      std::wstring result;
      for (auto& item : value.as_array()) {
        if (item.is_number()) {
          result.push_back(static_cast<wchar_t>(item.as_number().to_uint32()));
        }
      }

      return result;
    } else {
      return std::wstring();
    }
  }
}

void Cask::CdapOdbc::DataReader::fetchNull(const ColumnBinding& binding) {
  if (binding.getStrLenOrInd()) {
    *binding.getStrLenOrInd() = SQL_NULL_DATA;
  }
}

void Cask::CdapOdbc::DataReader::fetchVarchar(const wchar_t* str, const ColumnBinding& binding) {
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

void Cask::CdapOdbc::DataReader::fetchWVarchar(const wchar_t* str, const ColumnBinding& binding) {
  assert(binding.getTargetType() == SQL_WCHAR || binding.getTargetType() == SQL_DEFAULT);
  if (str) {
    std::wstring uniStr = str;
    size_t maxLength = static_cast<size_t>(binding.getBufferLength()) - 1;
    size_t size = (uniStr.size() < maxLength) ? uniStr.size() : maxLength;
    if (binding.getTargetValuePtr()) {
      wchar_t* outString = static_cast<wchar_t*>(binding.getTargetValuePtr());
      auto it = stdext::make_checked_array_iterator<wchar_t*>(outString, size);
      std::copy(uniStr.begin(), uniStr.begin() + size, it);
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
      wchar_t* outString = static_cast<wchar_t*>(binding.getTargetValuePtr());
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

void Cask::CdapOdbc::DataReader::fetchDouble(SQLDOUBLE value, const ColumnBinding& binding) {
  assert(binding.getTargetType() == SQL_DOUBLE || binding.getTargetType() == SQL_DEFAULT);
  *(reinterpret_cast<SQLDOUBLE*>(binding.getTargetValuePtr())) = value;
}

void Cask::CdapOdbc::DataReader::fetchValue(const web::json::value& value, const ColumnBinding& binding) {
  std::wstring strValue;
  SQLDOUBLE dblValue = NAN;
  switch (binding.getTargetType()) {
    case SQL_CHAR:
      strValue = toWString(value, binding);
      this->fetchVarchar(strValue.c_str(), binding);
      break;
    case SQL_WCHAR:
      strValue = toWString(value, binding);
      this->fetchWVarchar(strValue.c_str(), binding);
      break;
    case SQL_DOUBLE:
      if (value.is_string()) {
        dblValue = std::wcstod(value.as_string().c_str(), nullptr);
      } else if (value.is_integer()) {
        dblValue = value.as_integer();
      } else if (value.is_double()) {
        dblValue = value.as_double();
      } else if (value.is_number()) {
        dblValue = value.as_number().to_double();
      } else if (value.is_boolean()) {
        dblValue = value.as_bool() ? 1.0 : 0.0;
      }

      this->fetchDouble(dblValue, binding);
      break;
  }
}

void Cask::CdapOdbc::DataReader::fetchInt(SQLINTEGER value, const ColumnBinding& binding) {
  assert(binding.getTargetType() == SQL_C_SLONG || binding.getTargetType() == SQL_DEFAULT);
  *(reinterpret_cast<SQLINTEGER*>(binding.getTargetValuePtr())) = value;
}
