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

void Cask::CdapOdbc::ConnectionDialog::init() {
  Dialog::init();
  ::SetDlgItemTextW(this->getHandle(), IDC_HOST, this->params.getHost().c_str());
  ::SetDlgItemTextW(this->getHandle(), IDC_PORT, std::to_wstring(this->params.getPort()).c_str());
  ::SetDlgItemTextW(this->getHandle(), IDC_AUTHTOKEN, this->params.getAuthToken().c_str());
  ::SetDlgItemTextW(this->getHandle(), IDC_NAMESPACE, this->params.getNamespace().c_str());
  ::CheckDlgButton(this->getHandle(), IDC_SSL, this->params.getSslEnabled() ? BST_CHECKED : BST_UNCHECKED);
  ::CheckDlgButton(this->getHandle(), IDC_VERIFYCERT, this->params.getVerifySslCert() ? BST_CHECKED : BST_UNCHECKED);
}

void Cask::CdapOdbc::ConnectionDialog::readFromDialog() {
  Dialog::readFromDialog();

  const int MaxLen = 255;
  wchar_t buffer[MaxLen];
  UINT size = 0;
  BOOL result = FALSE;
  UINT checked = 0;

  size = ::GetDlgItemTextW(this->getHandle(), IDC_HOST, buffer, MaxLen);
  if (size > 0) {
    this->params.setHost(buffer);
  }

  size = ::GetDlgItemTextW(this->getHandle(), IDC_PORT, buffer, MaxLen);
  if (size > 0) {
    this->params.setPort(std::wcstol(buffer, nullptr, 10));
  }

  size = ::GetDlgItemTextW(this->getHandle(), IDC_AUTHTOKEN, buffer, MaxLen);
  if (size > 0) {
    this->params.setAuthToken(buffer);
  }

  size = ::GetDlgItemTextW(this->getHandle(), IDC_NAMESPACE, buffer, MaxLen);
  if (size > 0) {
    this->params.setNamespace(buffer);
  }

  checked = ::IsDlgButtonChecked(this->getHandle(), IDC_SSL);
  if (result != BST_INDETERMINATE) {
    this->params.setSslEnabled(checked == BST_CHECKED);
  }

  checked = ::IsDlgButtonChecked(this->getHandle(), IDC_VERIFYCERT);
  if (result != BST_INDETERMINATE) {
    this->params.setVerifySslCert(checked == BST_CHECKED);
  }
}

void Cask::CdapOdbc::ConnectionDialog::updateOKButton() {
  const int MaxLen = 255;
  wchar_t buffer[MaxLen];
  UINT hostSize = 0;
  UINT nsSize = 0;
  BOOL enabled = FALSE;
  HWND button = nullptr;

  hostSize = ::GetDlgItemTextW(this->getHandle(), IDC_HOST, buffer, MaxLen);
  nsSize = ::GetDlgItemTextW(this->getHandle(), IDC_NAMESPACE, buffer, MaxLen);
  enabled = hostSize > 0 && nsSize > 0;
  button = ::GetDlgItem(this->getHandle(), IDOK);
  ::EnableWindow(button, enabled);
}

INT_PTR Cask::CdapOdbc::ConnectionDialog::proc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  
  switch (uMsg) {
    case WM_CTLCOLORSTATIC:
      if (this->getBoldFont()) {
        switch (::GetWindowLongW(reinterpret_cast<HWND>(lParam), GWL_ID)) {
          case IDC_STATIC_HOST:
          case IDC_STATIC_NAMESPACE:
            ::SelectObject(reinterpret_cast<HDC>(wParam), this->getBoldFont());
            break;
        }
      }

      break;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDC_HOST:
        case IDC_NAMESPACE:
          switch (HIWORD(wParam)) {
            case EN_CHANGE:
              this->updateOKButton();
              break;
          }

          break;
      }

      break;
  }
  
  return Dialog::proc(uMsg, wParam, lParam);
}

Cask::CdapOdbc::ConnectionDialog::ConnectionDialog(HWND parent)
  : Dialog(parent, IDD_CONNECTIONSTRINGDIALOG) {
}