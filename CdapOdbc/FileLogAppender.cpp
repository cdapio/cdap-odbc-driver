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
#include "FileLogAppender.h"
#include "Library.h"

// http://stackoverflow.com/questions/6924195/get-dll-path-at-runtime
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

void Cask::CdapOdbc::FileLogAppender::setupFilePath() {
  wchar_t dllPath[MAX_PATH] = { 0 };
  if (GetModuleFileNameW((HINSTANCE)&__ImageBase, dllPath, _countof(dllPath))) {
    if (PathRemoveFileSpecW(dllPath)) {
      this->filePath = dllPath;
      this->filePath += L'\\';
      this->filePath += LOG_FILE;
    }
  }
}

Cask::CdapOdbc::FileLogAppender::FileLogAppender() {
  this->setupFilePath();
}

void Cask::CdapOdbc::FileLogAppender::write(const std::wstring& message) {
  if (this->filePath.size() > 0) {
    std::wfstream file;
    file.open(this->filePath, std::wfstream::in | std::wfstream::app);
    file << message;
    file.close();
  }
}
