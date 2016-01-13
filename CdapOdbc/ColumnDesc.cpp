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
#include "ColumnDesc.h"

using namespace Cask::CdapOdbc;

Cask::CdapOdbc::ColumnDesc::ColumnDesc()
  : position(0) {
}

Cask::CdapOdbc::ColumnDesc::ColumnDesc(web::json::value value) {
  this->name = value.at(L"name").as_string();
  this->type = value.at(L"type").as_string();
  std::transform(this->type.begin(), this->type.end(), this->type.begin(), std::towlower);
  this->position = value.at(L"position").as_integer();
  if (value.has_field(L"comment")) {
    this->comment = value.at(L"comment").as_string();
  }
}
