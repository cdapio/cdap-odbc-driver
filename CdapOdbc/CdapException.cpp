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
#include "CdapException.h"

Cask::CdapOdbc::CdapException::CdapException()
  : std::logic_error(nullptr)
  , sqlStatusCode(L"HY000")
  , errorCode(SQL_ERROR) {
}

Cask::CdapOdbc::CdapException::CdapException(const std::wstring& message)
  : std::logic_error(nullptr)
  , sqlStatusCode(L"HY000") 
  , message(message)
  , errorCode(SQL_ERROR) {
}

Cask::CdapOdbc::CdapException::CdapException(const std::wstring& sqlStatusCode, const std::wstring& message) 
  : std::logic_error(nullptr)
  , sqlStatusCode(sqlStatusCode)
  , message(message)
  , errorCode(SQL_ERROR) {
}

Cask::CdapOdbc::CdapException::CdapException(SQLRETURN errorCode, const std::wstring& sqlStatusCode, const std::wstring& message) 
  : std::logic_error(nullptr)
  , sqlStatusCode(L"HY000")
  , message(message)
  , errorCode(errorCode) {
}

Cask::CdapOdbc::CdapException::CdapException(SQLRETURN errorCode, const std::wstring& message) 
  : std::logic_error(nullptr)
  , sqlStatusCode(sqlStatusCode)
  , message(message)
  , errorCode(errorCode) {
}

Cask::CdapOdbc::CdapException::CdapException(const CdapException& other)
  : std::logic_error(nullptr) 
  , sqlStatusCode(other.sqlStatusCode)
  , message(other.message)
  , errorCode(other.errorCode) {
}

Cask::CdapOdbc::CdapException::CdapException(CdapException&& other)
  : std::logic_error(nullptr)
  , sqlStatusCode(std::move(other.sqlStatusCode))
  , message(std::move(other.message))
  , errorCode(other.errorCode) {
  other.errorCode = SQL_ERROR;
}

void Cask::CdapOdbc::CdapException::operator=(const CdapException& other) {
  this->sqlStatusCode = other.sqlStatusCode;
  this->message = other.message;
  this->errorCode = other.errorCode;
}

void Cask::CdapOdbc::CdapException::operator=(CdapException&& other) {
  this->sqlStatusCode = std::move(other.sqlStatusCode);
  this->message = std::move(other.message);
  this->errorCode = other.errorCode;
  other.errorCode = SQL_ERROR;
}
