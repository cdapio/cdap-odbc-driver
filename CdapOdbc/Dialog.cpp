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
#include "Dialog.h"
#include "Library.h"

namespace {
  Cask::CdapOdbc::Dialog* getSelf(HWND window) {
    return reinterpret_cast<Cask::CdapOdbc::Dialog*>(::GetWindowLongPtrW(window, GWLP_USERDATA));
  }
}

INT_PTR Cask::CdapOdbc::Dialog::dialogProc(_In_ HWND hwndDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
  switch (uMsg) {
    case WM_INITDIALOG:
      ::SetWindowLongPtrW(hwndDlg, GWLP_USERDATA, lParam);
      getSelf(hwndDlg)->handle = hwndDlg;
      getSelf(hwndDlg)->init();
      return TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK:
          getSelf(hwndDlg)->readFromDialog();
        case IDCANCEL:
          ::EndDialog(hwndDlg, wParam);
          return TRUE;
      }

      break;
  }

  if (getSelf(hwndDlg)) {
    return getSelf(hwndDlg)->proc(uMsg, wParam, lParam);
  } else {
    return FALSE;
  }
}

INT_PTR Cask::CdapOdbc::Dialog::proc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  return FALSE;
}

void Cask::CdapOdbc::Dialog::init() {
  this->initBoldFont();
}

void Cask::CdapOdbc::Dialog::readFromDialog() {
}

void Cask::CdapOdbc::Dialog::initBoldFont() {
  HFONT dialogFont = (HFONT)SendMessageW(this->getHandle(), WM_GETFONT, 0, 0);
  LOGFONTW lf = { 0 };
  if (dialogFont) {
    GetObjectW(dialogFont, sizeof(lf), &lf);
    lf.lfWeight = FW_BOLD;
    this->boldFont = CreateFontIndirectW(&lf);
  }
}

Cask::CdapOdbc::Dialog::Dialog(HWND parent, UINT id)
  : handle(nullptr)
  , id(id)
  , parent(parent)
  , result(false)
  , boldFont(nullptr) {
}

Cask::CdapOdbc::Dialog::~Dialog() {
  if (this->boldFont) {
    DeleteObject(this->boldFont);
    this->boldFont = nullptr;
  }
}

bool Cask::CdapOdbc::Dialog::show() {
  return ::DialogBoxParamW(
    Library::getInstanceHandle(),
    MAKEINTRESOURCE(this->id),
    this->parent,
    dialogProc,
    reinterpret_cast<LPARAM>(this)) == IDOK;
}
