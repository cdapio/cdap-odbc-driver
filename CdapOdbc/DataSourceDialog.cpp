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

namespace {
  Cask::CdapOdbc::DataSourceDialog* getSelf(HWND window) {
    return reinterpret_cast<Cask::CdapOdbc::DataSourceDialog*>(::GetWindowLongPtrW(window, GWLP_USERDATA));
  }
}

INT_PTR Cask::CdapOdbc::DataSourceDialog::dialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_INITDIALOG:
      ::SetWindowLongPtrW(hwndDlg, GWLP_USERDATA, lParam);
      getSelf(hwndDlg)->init(hwndDlg);
      return TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK:
          getSelf(hwndDlg)->readFromDialog(hwndDlg);
        case IDCANCEL:
          ::EndDialog(hwndDlg, wParam);
          return TRUE;
      }
  }

  return FALSE;
}

void Cask::CdapOdbc::DataSourceDialog::init(HWND window) {
  ::SetDlgItemTextW(window, IDC_NAME, this->getName().c_str());
}

void Cask::CdapOdbc::DataSourceDialog::readFromDialog(HWND window) {
  const int MaxLen = 255;
  wchar_t buffer[MaxLen];
  UINT size = 0;
  BOOL result = FALSE;

  size = ::GetDlgItemTextW(window, IDC_NAME, buffer, MaxLen);
  if (size > 0) {
    this->setName(buffer);
  }
}

Cask::CdapOdbc::DataSourceDialog::DataSourceDialog(HWND parent)
  : parent(parent)
  , result(false) {
}

Cask::CdapOdbc::DataSourceDialog::~DataSourceDialog() {
}

bool Cask::CdapOdbc::DataSourceDialog::show() {
  return ::DialogBoxParamW(
    Library::getInstanceHandle(),
    MAKEINTRESOURCE(IDD_DSNDIALOG),
    this->parent,
    dialogProc,
    reinterpret_cast<LPARAM>(this)) == IDOK;
}
