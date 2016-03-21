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
#include "spi.h"
#include "Driver.h"
#include "ConnectionInfo.h"
#include "CdapException.h"
#include "InvalidHandleException.h"
#include "Argument.h"

using namespace Cask::CdapOdbc;

SQLRETURN SQL_SPI SQLCleanupConnectionPoolID(
  SQLHENV environmentHandle,
  POOLID poolID) {
  return SQL_ERROR;
}

SQLRETURN SQL_SPI SQLGetPoolID(
  SQLHDBC_INFO_TOKEN hDbcInfoToken,
  _Out_ POOLID* pPoolID) {
  return SQL_ERROR;
}

SQLRETURN SQL_SPI SQLPoolConnectW(
  SQLHDBC hdbc,
  SQLHDBC_INFO_TOKEN hDbcInfoToken,
  _Out_writes_opt_(cchConnStrOutMax) SQLWCHAR* szConnStrOut,
  SQLSMALLINT cchConnStrOutMax,
  _Out_opt_ SQLSMALLINT* pcchConnStrOut) {
  return SQL_ERROR;
}

SQLRETURN SQL_SPI SQLRateConnection(
  SQLHDBC_INFO_TOKEN hRequest,
  SQLHDBC hCandidateConnection,
  BOOL fRequiresTransactionEnlistment,
  TRANSID transId,
  _Out_ SQLConnPoolRating* pRating) {
  TRACE(L"SQLRateConnection(hRequest = %X, hCandidateConnection = %X)\n", hRequest, hCandidateConnection);
  try {
    auto& info = Driver::getInstance().getConnectionInfo(hRequest);
    auto& connection = Driver::getInstance().getConnection(hCandidateConnection);
    try {
      info.getSqlStatus().clear();
      *pRating = static_cast<SQLConnPoolRating>(info.rateConnection(connection));
      TRACE(L"SQLRateConnection returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    } catch (CdapException& cex) {
      TRACE(L"SQLRateConnection returns SQL_ERROR\n");
      info.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLRateConnection returns SQL_ERROR\n");
      info.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLRateConnection returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLRateConnection returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_SPI SQLSetConnectAttrForDbcInfoW(
  SQLHDBC_INFO_TOKEN hDbcInfoToken,
  SQLINTEGER Attribute,
  _In_reads_bytes_opt_(StringLength) SQLPOINTER Value,
  SQLINTEGER StringLength) {
  TRACE(L"SQLSetConnectAttrForDbcInfoW(hDbcInfoToken = %X, Attribute = %d)\n", hDbcInfoToken, Attribute);
  try {
    auto& info = Driver::getInstance().getConnectionInfo(hDbcInfoToken);
    try {
      info.getSqlStatus().clear();
      SQLULEN value = 0;
      switch (Attribute) {
        case SQL_ATTR_ASYNC_ENABLE:
          value = reinterpret_cast<SQLULEN>(Value);
          info.setAsync(value == SQL_ASYNC_ENABLE_ON);
          TRACE(L"SQLSetConnectAttrForDbcInfoW returns SQL_SUCCESS\n");
          return SQL_SUCCESS;
        case SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE:
          value = reinterpret_cast<SQLULEN>(Value);
          info.setFunctionsAsync(value == SQL_ASYNC_DBC_ENABLE_ON);
          TRACE(L"SQLSetConnectAttrForDbcInfoW returns SQL_SUCCESS\n");
          return SQL_SUCCESS;
        default:
          TRACE(L"SQLSetConnectAttrForDbcInfoW returns SQL_SUCCESS\n");
          return SQL_SUCCESS;
      }
    } catch (CdapException& cex) {
      TRACE(L"SQLSetConnectAttrForDbcInfoW returns SQL_ERROR\n");
      info.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLSetConnectAttrForDbcInfoW returns SQL_ERROR\n");
      info.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLSetConnectAttrForDbcInfoW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLSetConnectAttrForDbcInfoW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_SPI SQLSetConnectInfoW(
  SQLHDBC_INFO_TOKEN hDbcInfoToken,
  _In_reads_(cchDSN) SQLWCHAR* szDSN,
  SQLSMALLINT cchDSN,
  _In_reads_(cchUID) SQLWCHAR* szUID,
  SQLSMALLINT cchUID,
  _In_reads_(cchAuthStr) SQLWCHAR* szAuthStr,
  SQLSMALLINT cchAuthStr) {
  // Used for SQLConnect call
  // Not supported
  TRACE(L"SQLSetConnectInfoW\n");
  TRACE(L"SQLSetConnectInfoW returns SQL_ERROR\n");
  return SQL_ERROR;
}

SQLRETURN SQL_SPI SQLSetDriverConnectInfoW(
  SQLHDBC_INFO_TOKEN hDbcInfoToken,
  _In_reads_(cchConnStrIn) SQLWCHAR* szConnStrIn,
  SQLSMALLINT cchConnStrIn) {
  TRACE(L"SQLSetDriverConnectInfoW(hDbcInfoToken = %X, szConnStrIn = %s)\n", hDbcInfoToken, szConnStrIn);
  try {
    auto& info = Driver::getInstance().getConnectionInfo(hDbcInfoToken);
    try {
      info.getSqlStatus().clear();
      auto connectionString = Argument::toStdString(szConnStrIn, cchConnStrIn);
      if (connectionString) {
        info.setConnectionString(*connectionString);
        TRACE(L"SQLSetDriverConnectInfoW returns SQL_SUCCESS\n");
        return SQL_SUCCESS;
      } else {
        throw CdapException(L"Connection string cannot be empty.");
      }
    } catch (CdapException& cex) {
      TRACE(L"SQLSetDriverConnectInfoW returns SQL_ERROR\n");
      info.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLSetDriverConnectInfoW returns SQL_ERROR\n");
      info.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLSetDriverConnectInfoW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLSetDriverConnectInfoW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}