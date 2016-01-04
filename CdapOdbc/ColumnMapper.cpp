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
#include "ColumnMapper.h"

namespace {
  void copyString(const std::wstring* str, SQLPOINTER outStr, SQLLEN bufferLength, SQLLEN* lenOrInd) {
    if (str) {
      size_t maxLength = static_cast<size_t>(bufferLength) - 1;
      size_t size = (str->size() < maxLength) ? str->size() : maxLength;
      if (outStr) {
        wchar_t* outString = static_cast<wchar_t*>(outStr);
        auto it = stdext::make_checked_array_iterator<wchar_t*>(outString, size);
        std::copy(str->begin(), str->begin() + size, it);
        outString[size] = 0;
      }

      if (lenOrInd) {
        if (size <= maxLength) {
          *lenOrInd = size;
        } else {
          *lenOrInd = SQL_NO_TOTAL;
        }
      }
    } else {
      if (outStr && bufferLength > 0) {
        wchar_t* outString = static_cast<wchar_t*>(outStr);
        outString[0] = 0;
      }
      
      if (lenOrInd) {
        *lenOrInd = SQL_NULL_DATA;
      }
    }
  }
}

const Cask::CdapOdbc::ColumnDesc* Cask::CdapOdbc::ColumnMapper::getDesc(const ColumnBinding& binding) const {
  // By default position is an index of column
  return &(*(this->columnDescs.begin() + (binding.getColumnNumber() - 1)));
}

void Cask::CdapOdbc::ColumnMapper::map(const web::json::value& data) {
  assert(data.is_array());
  for (auto& item : this->columnBindings) {
    auto desc = this->getDesc(item);
    if (desc) {
      if (item.getTargetType() == SQL_C_CHAR) {
        auto value = data.at(desc->getPosition());
        std::wstring strValue;

        if (!value.is_null()) {
          strValue = value.as_string();
        }

        copyString(value.is_null() ? nullptr : &strValue, item.getTargetValuePtr(), item.getBufferLength(), item.getStrLenOrInd());
      }
    } else {
      // Column not found
      if (item.getTargetType() == SQL_C_CHAR) {
        copyString(nullptr, item.getTargetValuePtr(), item.getBufferLength(), item.getStrLenOrInd());
      }
    }
  }
}
