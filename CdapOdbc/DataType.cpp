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
#include "DataType.h"

Cask::CdapOdbc::DataType::DataType(
  const std::wstring& name, 
  SQLSMALLINT sqlType, 
  SQLULEN size /* = 0U */, 
  SQLLEN displaySize /* = 0 */, 
  SQLSMALLINT decimalDigits /* = 0 */, 
  SQLSMALLINT nullable /* = SQL_NULLABLE */)
  : name(name)
  , size(size)
  , displaySize(displaySize)
  , sqlType(sqlType)
  , decimalDigits(decimalDigits)
  , nullable(nullable) {
}
