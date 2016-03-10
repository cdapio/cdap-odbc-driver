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
#include "CdapException.h"

namespace {

  std::wstring doubleToWString(double value, const Cask::CdapOdbc::ColumnBinding& binding) {
    int maxLen = static_cast<int>(binding.getBufferLength()) - 1;
    auto result = Cask::CdapOdbc::String::fromDouble(value, maxLen);
    //if (result.size() > maxLen) {
    //  maxLen -= (static_cast<int>(result.size()) - maxLen);
    //  result = Cask::CdapOdbc::String::fromDouble(value, maxLen - 1);
    //}

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

  double intToFrac(int value) {
    double temp = value;
    while (temp >= 1.0) {
      temp /= 10;
    }

    return temp;
  }

  void parseTimestamp(const std::wstring& value, SQL_TIMESTAMP_STRUCT& ts) {
    const wchar_t* formatString = L"%4d-%2d-%2d %2d:%2d:%2d.%3d";
    ts = { 0 };
    int fraction = 0;
    auto n = swscanf_s(
      value.c_str(), 
      formatString,
      &ts.year,
      &ts.month,
      &ts.day,
      &ts.hour,
      &ts.minute,
      &ts.second,
      &fraction);
    if (!(n == 3 || n == 7)) {
      throw Cask::CdapOdbc::CdapException(L"Cannot convert string '" + value + L"'to TIMESTAMP.");
    }

    if (fraction > 0) {
      ts.fraction = static_cast<SQLUINTEGER>(1000000000 * intToFrac(fraction));
    }
  }

  void parseDate(const std::wstring& value, SQL_DATE_STRUCT& ts) {
    const wchar_t* formatString = L"%4d-%2d-%2d";
    ts = { 0 };
    auto n = swscanf_s(
      value.c_str(),
      formatString,
      &ts.year,
      &ts.month,
      &ts.day);
    if (!(n == 3)) {
      throw Cask::CdapOdbc::CdapException(L"Cannot convert string '" + value + L"' to DATE.");
}
  }
}

void Cask::CdapOdbc::DataReader::fetchNull(const ColumnBinding& binding) const {
  if (binding.getStrLenOrInd()) {
    *binding.getStrLenOrInd() = SQL_NULL_DATA;
  }
}

void Cask::CdapOdbc::DataReader::fetchVarchar(const wchar_t* str, const ColumnBinding& binding) const {
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

void Cask::CdapOdbc::DataReader::fetchWVarchar(const wchar_t* str, const ColumnBinding& binding) const {
  assert(binding.getTargetType() == SQL_WCHAR || binding.getTargetType() == SQL_DEFAULT);
  if (str) {
    std::wstring uniStr = str;
    size_t maxLength = (static_cast<size_t>(binding.getBufferLength()) / sizeof(wchar_t)) - 1;
    size_t size = (uniStr.size() < maxLength) ? uniStr.size() : maxLength;
    if (binding.getTargetValuePtr()) {
      wchar_t* outString = static_cast<wchar_t*>(binding.getTargetValuePtr());
      auto it = stdext::make_checked_array_iterator<wchar_t*>(outString, size);
      std::copy(uniStr.begin(), uniStr.begin() + size, it);
      outString[size] = 0;
    }

    if (binding.getStrLenOrInd()) {
      if (size <= maxLength) {
        *binding.getStrLenOrInd() = size * sizeof(wchar_t);
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

void Cask::CdapOdbc::DataReader::fetchTinyint(SQLCHAR value, const ColumnBinding& binding) const {
  assert(binding.getTargetType() == SQL_TINYINT ||
         binding.getTargetType() == SQL_C_STINYINT ||
         binding.getTargetType() == SQL_DEFAULT);
  *(reinterpret_cast<SQLCHAR*>(binding.getTargetValuePtr())) = value;
}

void Cask::CdapOdbc::DataReader::fetchSmallint(SQLSMALLINT value, const ColumnBinding& binding) const {
  assert(binding.getTargetType() == SQL_C_SSHORT || binding.getTargetType() == SQL_DEFAULT);
  *(reinterpret_cast<SQLSMALLINT*>(binding.getTargetValuePtr())) = value;
}

void Cask::CdapOdbc::DataReader::fetchDouble(SQLDOUBLE value, const ColumnBinding& binding) const {
  assert(binding.getTargetType() == SQL_DOUBLE || binding.getTargetType() == SQL_DEFAULT);
  *(reinterpret_cast<SQLDOUBLE*>(binding.getTargetValuePtr())) = value;
}

void Cask::CdapOdbc::DataReader::fetchTimestamp(const SQL_TIMESTAMP_STRUCT& value, const ColumnBinding & binding) const {
  assert(binding.getTargetType() == SQL_C_TIMESTAMP ||
         binding.getTargetType() == SQL_C_TYPE_TIMESTAMP || /* == SQL_TYPE_TIMESTAMP */
         binding.getTargetType() == SQL_C_DEFAULT);
  *(reinterpret_cast<SQL_TIMESTAMP_STRUCT*>(binding.getTargetValuePtr())) = value;
}

void Cask::CdapOdbc::DataReader::fetchDate(const SQL_DATE_STRUCT& value, const ColumnBinding& binding) const {
  assert(binding.getTargetType() == SQL_C_DATE ||
    binding.getTargetType() == SQL_C_TYPE_DATE || /* == SQL_TYPE_DATA */
    binding.getTargetType() == SQL_C_DEFAULT);
  *(reinterpret_cast<SQL_DATE_STRUCT*>(binding.getTargetValuePtr())) = value;
}

void Cask::CdapOdbc::DataReader::fetchUnsignedLong(SQLUBIGINT value, const ColumnBinding& binding) const {
  assert(binding.getTargetType() == SQL_C_ULONG || binding.getTargetType() == SQL_DEFAULT);
  *(reinterpret_cast<SQLUBIGINT*>(binding.getTargetValuePtr())) = value;
}

void Cask::CdapOdbc::DataReader::fetchSignedLong(SQLBIGINT value, const ColumnBinding& binding) const {
  assert(binding.getTargetType() == SQL_C_SBIGINT || binding.getTargetType() == SQL_DEFAULT);
  *(reinterpret_cast<SQLBIGINT*>(binding.getTargetValuePtr())) = value;
}

void Cask::CdapOdbc::DataReader::fetchValue(const web::json::value& value, const ColumnBinding& binding) const {
  std::wstring strValue;
  std::wstring message;
  SQLDOUBLE dblValue = NAN;
  SQLUBIGINT ubintValue = 0ULL;
  SQLBIGINT sbintValue = 0LL;
  SQLCHAR sintValue = 0;
  SQL_TIMESTAMP_STRUCT ts = { 0 };
  SQL_DATE_STRUCT dt = { 0 };

  switch (binding.getTargetType()) {
    case SQL_BIT:
    case SQL_TINYINT:
      if (value.is_boolean()) {
        sintValue = static_cast<SQLCHAR>(value.as_bool() ? 1 : 0);
      } else if (value.is_integer()) {
        sintValue = static_cast<SQLCHAR>(value.as_integer());
      } else {
        throw CdapException(L"Cannot convert value '" + value.as_string() + L"'to BIT/TINYINT.");
      }

      this->fetchTinyint(sintValue, binding);
      break;
    case SQL_C_CHAR: /* SQL_CHAR */
      strValue = toWString(value, binding);
      this->fetchVarchar(strValue.c_str(), binding);
      break;
    case SQL_WCHAR:
      strValue = toWString(value, binding);
      this->fetchWVarchar(strValue.c_str(), binding);
      break;
    case SQL_C_DOUBLE: /* SQL_DOUBLE*/
      if (value.is_string()) {
        dblValue = std::wcstod(value.as_string().c_str(), nullptr);
      } else if (value.is_number()) {
        dblValue = value.as_number().to_double();
      } else if (value.is_boolean()) {
        dblValue = value.as_bool() ? 1.0 : 0.0;
      } else {
        throw CdapException(L"Cannot convert value '" + value.as_string() + L"'to DOUBLE.");
      }

      this->fetchDouble(dblValue, binding);
      break;
    case SQL_C_ULONG:
      if (value.is_string()) {
        ubintValue = std::wcstoul(value.as_string().c_str(), nullptr, 0);
      } else if (value.is_number()) {
        ubintValue = value.as_number().to_uint64();
      } else if (value.is_boolean()) {
        ubintValue = value.as_bool() ? 1UL : 0UL;
      } else {
        throw CdapException(L"Cannot convert value '" + value.as_string() + L"'to ULONG.");
      }

      this->fetchUnsignedLong(ubintValue, binding);
      break;

    case SQL_C_SBIGINT:
      if (value.is_string()) {
        sbintValue = std::wcstol(value.as_string().c_str(), nullptr, 0);
      } else if (value.is_number()) {
        sbintValue = value.as_number().to_int64();
      } else if (value.is_boolean()) {
        sbintValue = value.as_bool() ? 1L : 0L;
      } else {
        throw CdapException(L"Cannot convert value '" + value.as_string() + L"'to SBIGINT.");
      }

      this->fetchSignedLong(sbintValue, binding);
      break;
    case SQL_C_TYPE_TIMESTAMP:
    case SQL_C_TIMESTAMP:
      if (value.is_string()) {
        strValue = value.as_string();
        parseTimestamp(strValue, ts);
        this->fetchTimestamp(ts, binding);
      } else {
        throw CdapException(L"Cannot convert value '" + strValue + L"' to TIMESTAMP.");
      }

      break;
    case SQL_C_TYPE_DATE:
    case SQL_C_DATE:
      if (value.is_string()) {
        strValue = value.as_string();
        parseDate(strValue, dt);
        this->fetchDate(dt, binding);
      }
      else {
        throw CdapException(L"Cannot convert value '" + strValue + L"' to DATE.");
      }

      break;
    default:
      throw CdapException(L"Target type '" + std::to_wstring(binding.getTargetType()) + L"' is not supported.");
  }
}

void Cask::CdapOdbc::DataReader::fetchInt(SQLINTEGER value, const ColumnBinding& binding) const {
  assert(binding.getTargetType() == SQL_C_SLONG || binding.getTargetType() == SQL_DEFAULT);
  *(reinterpret_cast<SQLINTEGER*>(binding.getTargetValuePtr())) = value;
}
