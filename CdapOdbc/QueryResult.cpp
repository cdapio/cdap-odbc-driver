/*
* Copyright � 2015 Cask Data, Inc.
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
#include "QueryResult.h"

Cask::CdapOdbc::QueryResult::QueryResult()
  : sizeInBytes(0L) {
}

Cask::CdapOdbc::QueryResult::QueryResult(const web::json::value& value)
  : rows(value)
  , sizeInBytes(0L) {
}

Cask::CdapOdbc::QueryResult::QueryResult(const web::json::value& value, std::int64_t sizeInBytes)
  : rows(value)
  , sizeInBytes(sizeInBytes) {
}
