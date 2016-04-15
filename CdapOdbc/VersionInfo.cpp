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
#include "VersionInfo.h"
#include "Library.h"

std::wstring Cask::CdapOdbc::VersionInfo::getProductVersion() {
  wchar_t fullPath[MAX_PATH];
  DWORD len;
  DWORD handle = 0;
  std::unique_ptr<char[]> data;
  VS_FIXEDFILEINFO *fileInfo;
  UINT bufLen;

  if (GetModuleFileNameW(Library::getInstanceHandle(), fullPath, _countof(fullPath))) {
    len = GetFileVersionInfoSizeW(fullPath, &handle);
    if (len > 0) {
      data.reset(new char[len]);
      if (GetFileVersionInfoW(fullPath, handle, len, data.get())) {
        if (VerQueryValueW(data.get(), L"\\", reinterpret_cast<LPVOID*>(&fileInfo), &bufLen)) {
          std::wstringstream s;
          s << std::setfill(L'0') << std::setw(2) << HIWORD(fileInfo->dwFileVersionMS) << L'.';
          s << std::setfill(L'0') << std::setw(2) << LOWORD(fileInfo->dwFileVersionMS) << L'.';
          s << std::setfill(L'0') << std::setw(4) << HIWORD(fileInfo->dwFileVersionLS);
          return s.str();
        }
      }
    }
  }

  return L"01.00.0000";
}
