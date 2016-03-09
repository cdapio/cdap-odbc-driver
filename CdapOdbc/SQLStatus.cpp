/*
* Copyright © 2016 Cask Data, Inc.
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
#include "SQLStatus.h"
#include "Encoding.h"
#include "CdapException.h"

using namespace Cask::CdapOdbc;

Cask::CdapOdbc::SQLStatus::SQLStatus() {
}

void Cask::CdapOdbc::SQLStatus::addMsg(
  const std::wstring& code,
  const std::wstring& msg,
  void* binError,
  size_t buffLength) {
  statusItems.push_front(SQLStatusElement(code, msg, binError, buffLength));
}

void Cask::CdapOdbc::SQLStatus::addError(const std::exception& ex) {
  const CdapException* cdapEx = dynamic_cast<const CdapException*>(&ex);
  if (cdapEx) {
    // Driver-specific errors 
    statusItems.push_front(SQLStatusElement(cdapEx->getSqlStatusCode(), cdapEx->getMessage()));
  } else {
    // Other exceptions
    auto msg = Encoding::toUtf16(ex.what());
    statusItems.push_front(SQLStatusElement(L"HY000", msg));
  }
}

void Cask::CdapOdbc::SQLStatus::clear() {
  statusItems.clear();
}

const std::wstring& Cask::CdapOdbc::SQLStatus::getCode(SQLSMALLINT recNum) {

  if (statusItems.size() >= recNum && recNum >= 1) {
    auto& it = statusItems.begin();
    std::advance(it, recNum - 1);
    return it->code;
  }

  return dummy;
}

const std::wstring& Cask::CdapOdbc::SQLStatus::getMessage(SQLSMALLINT recNum) {
  if (statusItems.size() >= recNum && recNum >= 1) {
    auto& it = statusItems.begin();
    std::advance(it, recNum - 1);
    return it->msg;
  }

  return dummy;
}

SQLSMALLINT Cask::CdapOdbc::SQLStatus::getRecCount() {
  return static_cast<SQLSMALLINT>(statusItems.size());
}