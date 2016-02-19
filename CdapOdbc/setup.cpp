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
#include "setup.h"
#include "CancelException.h"
#include "Driver.h"
#include "Encoding.h"

using namespace Cask::CdapOdbc;

BOOL INSTAPI ConfigDSN(
  HWND     hwndParent,
  WORD     fRequest,
  LPCSTR   lpszDriver,
  LPCSTR   lpszAttributes) {
  TRACE(L"ConfigDSN(hwndParent = %X, fRequest = %d)\n", hwndParent, fRequest);

  if (hwndParent == INVALID_HANDLE_VALUE) {
    SQLPostInstallerErrorW(ODBC_ERROR_INVALID_HWND, L"The hwndParent argument was invalid.");
    TRACE(L"ConfigDSN returns FALSE\n");
    return FALSE;
  }

  if (!(fRequest == ODBC_ADD_DSN || fRequest == ODBC_CONFIG_DSN || fRequest == ODBC_REMOVE_DSN)) {
    SQLPostInstallerErrorW(
      ODBC_ERROR_INVALID_REQUEST_TYPE,
      L"The fRequest argument was not one of the following: ODBC_ADD_DSN, ODBC_CONFIG_DSN, ODBC_REMOVE_DSN.");
    TRACE(L"ConfigDSN returns FALSE\n");
    return FALSE;
  }

  try {
    auto driver = Encoding::toUtf16(lpszDriver);
    auto attrs = Encoding::toUtf16(lpszAttributes);
    TRACE(L"ConfigDSN - lpszDriver = %s, lpszAttributes = %s\n", driver.c_str(), attrs.c_str());
    switch (fRequest) {
      case ODBC_ADD_DSN:
        Driver::getInstance().addDataSource(hwndParent, driver, attrs);
        TRACE(L"ConfigDSN returns TRUE\n");
        return TRUE;
      case ODBC_CONFIG_DSN:
        Driver::getInstance().modifyDataSource(hwndParent, driver, attrs);
        TRACE(L"ConfigDSN returns TRUE\n");
        return TRUE;
      case ODBC_REMOVE_DSN:
        Driver::getInstance().deleteDataSource(driver, attrs);
        TRACE(L"ConfigDSN returns TRUE\n");
        return TRUE;
    }

    TRACE(L"ConfigDSN returns FALSE\n");
    return FALSE;
  } catch (CancelException cex) {
    std::wstring msg = Encoding::toUtf16(cex.what());
    SQLPostInstallerErrorW(ODBC_ERROR_USER_CANCELED, msg.c_str());
    TRACE(L"ConfigDSN returns FALSE\n");
    return FALSE;
  } catch (std::exception& ex) {
    std::wstring msg = Encoding::toUtf16(ex.what());
    SQLPostInstallerErrorW(ODBC_ERROR_GENERAL_ERR, msg.c_str());
    TRACE(L"ConfigDSN returns FALSE\n");
    return FALSE;
  }
}