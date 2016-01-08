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
#include "MetadataColumnMapper.h"

namespace {
  const wchar_t* getColumnName(int columnNumber) {
    switch (columnNumber) {
      case 1: 
        return L"TABLE_CAT";
      case 2:
        return L"TABLE_SCHEM";
      case 3:
        return L"TABLE_NAME";
      case 4:
        return L"TABLE_TYPE";
      case 5:
        return L"REMARKS";
      default:
        return nullptr;
    }
  }
}

const Cask::CdapOdbc::ColumnDesc* Cask::CdapOdbc::MetadataColumnMapper::getDesc(const ColumnBinding& binding) const {
  const wchar_t* columnName = getColumnName(binding.getColumnNumber());
  if (columnName == nullptr) {
    throw std::invalid_argument("binding");
  }

  for (auto& desc : this->getColumnDescs()) {
    if (desc.getName() == columnName) {
      return &desc;
    }
  }

  // Column not found.
  return nullptr;
}
