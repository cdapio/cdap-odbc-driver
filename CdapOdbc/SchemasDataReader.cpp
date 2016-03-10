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
#include "SchemasDataReader.h"

bool Cask::CdapOdbc::SchemasDataReader::read() {
  return false;
}

void Cask::CdapOdbc::SchemasDataReader::getColumnValue(const ColumnBinding& binding) const {
}

short Cask::CdapOdbc::SchemasDataReader::getColumnCount() const {
  return 5;
}

std::unique_ptr<Cask::CdapOdbc::ColumnInfo> Cask::CdapOdbc::SchemasDataReader::getColumnInfo(short columnNumber) const {
  return std::unique_ptr<ColumnInfo>();
}

bool Cask::CdapOdbc::SchemasDataReader::canReadFast() const {
  return true;
}
