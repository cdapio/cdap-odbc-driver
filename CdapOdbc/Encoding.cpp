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
#include "Encoding.h"

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> Cask::CdapOdbc::Encoding::convert;

std::wstring Cask::CdapOdbc::Encoding::toUtf16(const std::string& utf8) {
  return convert.from_bytes(utf8);
}

std::string Cask::CdapOdbc::Encoding::toUtf8(const std::wstring& utf16) {
  return convert.to_bytes(utf16);
}