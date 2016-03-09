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
#include "DataSourceDialog.h"
#include "Library.h"
#include "resource.h"

INT_PTR Cask::CdapOdbc::DataSourceDialog::proc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  return FALSE;
}

void Cask::CdapOdbc::DataSourceDialog::init() {
  Dialog::init();
  ::SetDlgItemTextW(this->getHandle(), IDC_NAME, this->getName().c_str());
}

void Cask::CdapOdbc::DataSourceDialog::readFromDialog() {
  Dialog::readFromDialog();

  const int MaxLen = 255;
  wchar_t buffer[MaxLen];
  UINT size = 0;
  BOOL result = FALSE;

  size = ::GetDlgItemTextW(this->getHandle(), IDC_NAME, buffer, MaxLen);
  if (size > 0) {
    this->setName(buffer);
  }
}

Cask::CdapOdbc::DataSourceDialog::DataSourceDialog(HWND parent)
  : Dialog(parent, IDD_DSNDIALOG) {
}