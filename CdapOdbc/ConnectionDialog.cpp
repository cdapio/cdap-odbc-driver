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
#include "ConnectionDialog.h"
#include "Library.h"
#include "resource.h"

namespace {
  Cask::CdapOdbc::ConnectionDialog* getSelf(HWND window) {
    return reinterpret_cast<Cask::CdapOdbc::ConnectionDialog*>(::GetWindowLongPtrW(window, GWLP_USERDATA));
  }
}

INT_PTR Cask::CdapOdbc::ConnectionDialog::dialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

void Cask::CdapOdbc::ConnectionDialog::init(HWND window) {
  ::SetDlgItemTextW(window, IDC_HOST, this->params.getHost().c_str());
  ::SetDlgItemTextW(window, IDC_PORT, std::to_wstring(this->params.getPort()).c_str());
  ::SetDlgItemTextW(window, IDC_AUTHTOKEN, this->params.getAuthToken().c_str());
  ::SetDlgItemTextW(window, IDC_NAMESPACE, this->params.getNamespace().c_str());
  ::CheckDlgButton(window, IDC_SSL, this->params.getSslEnabled() ? BST_CHECKED : BST_UNCHECKED);
  ::CheckDlgButton(window, IDC_VERIFYCERT, this->params.getVerifySslCert() ? BST_CHECKED : BST_UNCHECKED);
}

void Cask::CdapOdbc::ConnectionDialog::readFromDialog(HWND window) {
  const int MaxLen = 255;
  wchar_t buffer[MaxLen];
  UINT size = 0;
  BOOL result = FALSE;
  UINT checked = 0;

  size = ::GetDlgItemTextW(window, IDC_HOST, buffer, MaxLen);
  if (size > 0) {
    this->params.setHost(buffer);
  }

  size = ::GetDlgItemTextW(window, IDC_PORT, buffer, MaxLen);
  if (size > 0) {
    this->params.setPort(std::wcstol(buffer, nullptr, 10));
  }

  size = ::GetDlgItemTextW(window, IDC_AUTHTOKEN, buffer, MaxLen);
  if (size > 0) {
    this->params.setAuthToken(buffer);
  }

  size = ::GetDlgItemTextW(window, IDC_NAMESPACE, buffer, MaxLen);
  if (size > 0) {
    this->params.setNamespace(buffer);
  }

  checked = ::IsDlgButtonChecked(window, IDC_SSL);
  if (result != BST_INDETERMINATE) {
    this->params.setSslEnabled(checked == BST_CHECKED);
  }

  checked = ::IsDlgButtonChecked(window, IDC_VERIFYCERT);
  if (result != BST_INDETERMINATE) {
    this->params.setVerifySslCert(checked == BST_CHECKED);
  }
}

Cask::CdapOdbc::ConnectionDialog::ConnectionDialog(HWND parent)
  : parent(parent)
  , result(false) {
}

Cask::CdapOdbc::ConnectionDialog::~ConnectionDialog() {
}

bool Cask::CdapOdbc::ConnectionDialog::show() {
  return ::DialogBoxParamW(
    Library::getInstanceHandle(),
    MAKEINTRESOURCE(IDD_CONNECTIONSTRINGDIALOG),
    this->parent,
    dialogProc,
    reinterpret_cast<LPARAM>(this)) == IDOK;
}
