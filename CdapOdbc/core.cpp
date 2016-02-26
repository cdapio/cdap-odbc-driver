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
#include "core.h"
#include "InvalidHandleException.h"
#include "CancelException.h"
#include "Driver.h"
#include "Environment.h"
#include "Connection.h"
#include "Argument.h"
#include "Statement.h"
#include "ConnectionDialog.h"
#include "Encoding.h"
#include "SQLStatus.h"
#include "CdapException.h"
#include "VersionInfo.h"

using namespace Cask::CdapOdbc;

SQLRETURN SQL_API SQLAllocHandle(
  SQLSMALLINT HandleType,
  SQLHANDLE InputHandle,
  _Out_ SQLHANDLE* OutputHandlePtr) {
  TRACE(L"SQLAllocHandle(HandleType = %d, InputHandle = %X)\n", HandleType, InputHandle);
  try {
    // OutputHandlePtr must be a valid pointer
    if (!OutputHandlePtr) {
      TRACE(L"SQLAllocHandle returns SQL_ERROR\n");
      return SQL_ERROR;
    }

    *OutputHandlePtr = SQL_NULL_HANDLE;

    switch (HandleType) {
      case SQL_HANDLE_ENV:
        // InputHandle must be NULL
        if (InputHandle != SQL_NULL_HANDLE) {
          TRACE(L"SQLAllocHandle returns SQL_ERROR\n");
          return SQL_ERROR;
        }

        // Allocate new environment
        *OutputHandlePtr = Driver::getInstance().allocEnvironment();
        TRACE(L"SQLAllocHandle returns SQL_SUCCESS, new environment = %X\n", *OutputHandlePtr);
        return SQL_SUCCESS;

      case SQL_HANDLE_DBC:
        // InputHandle must not be NULL
        if (InputHandle == SQL_NULL_HANDLE) {
          TRACE(L"SQLAllocHandle returns SQL_ERROR\n");
          return SQL_ERROR;
        }

        *OutputHandlePtr = Driver::getInstance().allocConnection(InputHandle);
        TRACE(L"SQLAllocHandle returns SQL_SUCCESS, new connection = %X\n", *OutputHandlePtr);
        return SQL_SUCCESS;

      case SQL_HANDLE_STMT:
        // InputHandle must not be NULL
        if (InputHandle == SQL_NULL_HANDLE) {
          TRACE(L"SQLAllocHandle returns SQL_ERROR\n");
          return SQL_ERROR;
        }

        *OutputHandlePtr = Driver::getInstance().allocStatement(InputHandle);
        TRACE(L"SQLAllocHandle returns SQL_SUCCESS, new statement = %X\n", *OutputHandlePtr);
        return SQL_SUCCESS;

      case SQL_HANDLE_DESC:
        // InputHandle must not be NULL
        if (InputHandle == SQL_NULL_HANDLE) {
          TRACE(L"SQLAllocHandle returns SQL_ERROR\n");
          return SQL_ERROR;
        }

        *OutputHandlePtr = Driver::getInstance().allocDescriptor(InputHandle);
        TRACE(L"SQLAllocHandle returns SQL_SUCCESS, new descriptor = %X\n", *OutputHandlePtr);
        return SQL_SUCCESS;

      default:
        TRACE(L"SQLAllocHandle returns SQL_ERROR\n");
        return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLAllocHandle returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLAllocHandle returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLConnectW(
  SQLHDBC        ConnectionHandle,
  SQLWCHAR *     ServerName,
  SQLSMALLINT    NameLength1,
  SQLWCHAR *     UserName,
  SQLSMALLINT    NameLength2,
  SQLWCHAR *     Authentication,
  SQLSMALLINT    NameLength3) {
  TRACE(L"SQLConnectW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLDriverConnectW(
  SQLHDBC         ConnectionHandle,
  SQLHWND         WindowHandle,
  SQLWCHAR *      InConnectionString,
  SQLSMALLINT     StringLength1,
  SQLWCHAR *      OutConnectionString,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLength2Ptr,
  SQLUSMALLINT    DriverCompletion) {
  TRACE(
    L"SQLDriverConnectW(ConnectionHandle = %X, WindowHandle = %X, InConnectionString = %s, DriverCompletion = %d)\n",
    ConnectionHandle,
    WindowHandle,
    InConnectionString,
    DriverCompletion);
  try {
    if (StringLength2Ptr) {
      *StringLength2Ptr = 0;
    }

    auto& connection = Driver::getInstance().getConnection(ConnectionHandle);
    std::lock_guard<Connection> lock(connection);
    try {
      std::unique_ptr<std::wstring> connectionString;
      std::wstring newConnectionString;
      std::unique_ptr<ConnectionDialog> dialog;

      // Clear Connection SQL Status
      connection.getSqlStatus().clear();

      switch (DriverCompletion) {
        case SQL_DRIVER_PROMPT:
          // DRIVER
          connectionString = Argument::toStdString(InConnectionString, StringLength1);
          dialog = std::make_unique<ConnectionDialog>(WindowHandle);
          dialog->setParams(ConnectionParams(*connectionString));
          if (!dialog->show()) {
            TRACE(L"SQLDriverConnectW returns SQL_NO_DATA\n");
            // Not an error
            return SQL_NO_DATA;
          }

          newConnectionString = dialog->getParams().getFullConnectionString();
          connection.open(newConnectionString);
          Argument::fromStdString(newConnectionString, OutConnectionString, BufferLength, StringLength2Ptr);
          TRACE(L"SQLDriverConnectW returns SQL_SUCCESS, OutConnectionString = %s\n", OutConnectionString);
          return SQL_SUCCESS;
        case SQL_DRIVER_COMPLETE:
        case SQL_DRIVER_COMPLETE_REQUIRED:
          // DSN
          connectionString = Argument::toStdString(InConnectionString, StringLength1);
          if (connectionString->find(L"DSN") != std::wstring::npos) {
            dialog = std::make_unique<ConnectionDialog>(WindowHandle);
            dialog->setParams(ConnectionParams(*connectionString));
            if (!dialog->show()) {
              TRACE(L"SQLDriverConnectW returns SQL_NO_DATA\n");
              // Not an error
              return SQL_NO_DATA;
            }

            newConnectionString = dialog->getParams().getFullConnectionString();
          } else {
            newConnectionString = *connectionString;
          }

          connection.open(newConnectionString);
          Argument::fromStdString(newConnectionString, OutConnectionString, BufferLength, StringLength2Ptr);
          TRACE(L"SQLDriverConnectW returns SQL_SUCCESS, OutConnectionString = %s\n", OutConnectionString);
          return SQL_SUCCESS;
        case SQL_DRIVER_NOPROMPT:
          // DRIVER 2
          connectionString = Argument::toStdString(InConnectionString, StringLength1);
          connection.open(*connectionString);
          Argument::fromStdString(*connectionString, OutConnectionString, BufferLength, StringLength2Ptr);
          TRACE(L"SQLDriverConnectW returns SQL_SUCCESS, OutConnectionString = %s\n", OutConnectionString);
          return SQL_SUCCESS;
        default:
          throw CdapException(L"Unknown DriverCompletion option.");
      }
    } catch (CdapException& cex) {
      TRACE(L"SQLDriverConnectW returns SQL_ERROR\n");
      connection.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLDriverConnectW returns SQL_ERROR\n");
      connection.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLDriverConnectW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLDriverConnectW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLGetTypeInfoW(
  SQLHSTMT      StatementHandle,
  SQLSMALLINT   DataType) {
  TRACE(L"SQLGetTypeInfoW(StatementHandle = %X, DataType = %d)\n", StatementHandle, DataType);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      statement.getDataTypes();
      TRACE(L"SQLGetTypeInfoW returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    } catch (CdapException& cex) {
      TRACE(L"SQLGetTypeInfoW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLGetTypeInfoW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLGetTypeInfoW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLGetTypeInfoW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLGetInfoW(
  SQLHDBC         ConnectionHandle,
  SQLUSMALLINT    InfoType,
  SQLPOINTER      InfoValuePtr,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLengthPtr) {
  TRACE(L"SQLGetInfoW(ConnectionHandle = %X, InfoType = %d, BufferLength = %d)\n", ConnectionHandle, InfoType, BufferLength);
  try {
    if (StringLengthPtr) {
      *StringLengthPtr = 0;
    }

    auto& connection = Driver::getInstance().getConnection(ConnectionHandle);
    std::lock_guard<Connection> lock(connection);
    try {
      if (InfoValuePtr == nullptr) {
        TRACE(L"SQLGetInfoW returns SQL_ERROR\n");
        return SQL_ERROR;
      }

      std::wstring temp;

      switch (InfoType) {
        case SQL_ODBC_VER:
          Argument::fromStdString(L"03.80.0000", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_DRIVER_ODBC_VER:
          Argument::fromStdString(L"03.00", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_DATA_SOURCE_NAME:
          Argument::fromStdString(L"CDAP Datasets", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_SEARCH_PATTERN_ESCAPE:
          Argument::fromStdString(L"", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_GETDATA_EXTENSIONS:
          assert(BufferLength == sizeof(SQLUINTEGER));
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = SQL_GD_ANY_COLUMN | SQL_GD_ANY_ORDER | SQL_GD_BOUND;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = SQL_GD_ANY_COLUMN | SQL_GD_ANY_ORDER | SQL_GD_BOUND\n");
          return SQL_SUCCESS;
        case SQL_CURSOR_COMMIT_BEHAVIOR:
        case SQL_CURSOR_ROLLBACK_BEHAVIOR:
          *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_CB_PRESERVE;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = SQL_CB_PRESERVE\n");
          return SQL_SUCCESS;
        case SQL_ACTIVE_STATEMENTS:
          assert(BufferLength >= sizeof(SQLUSMALLINT));
          *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = 0;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = 0\n");
          return SQL_SUCCESS;
        case SQL_DATA_SOURCE_READ_ONLY:
          Argument::fromStdString(L"Y", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_DRIVER_NAME:
          Argument::fromStdString(L"CdapOdbc.dll", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_CORRELATION_NAME:
          assert(BufferLength == sizeof(SQLUSMALLINT));
          *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_CN_DIFFERENT;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = SQL_CN_DIFFERENT\n");
          return SQL_SUCCESS;
        case SQL_NON_NULLABLE_COLUMNS:
          assert(BufferLength == sizeof(SQLUSMALLINT));
          *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_NNC_NON_NULL;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = SQL_NNC_NON_NULL\n");
          return SQL_SUCCESS;
        case SQL_COLUMN_ALIAS:
          Argument::fromStdString(L"Y", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_QUALIFIER_NAME_SEPARATOR:
          Argument::fromStdString(L".", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
            TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
            return SQL_SUCCESS;
        case SQL_IDENTIFIER_QUOTE_CHAR:
          Argument::fromStdString(L"", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_SPECIAL_CHARACTERS:
          Argument::fromStdString(L"", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_FILE_USAGE:
          assert(BufferLength == sizeof(SQLUSMALLINT));
          *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_FILE_CATALOG;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = SQL_FILE_CATALOG\n");
          return SQL_SUCCESS;
        case SQL_QUALIFIER_TERM:
          Argument::fromStdString(L"CATALOG", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_SERVER_NAME:
        case SQL_DATABASE_NAME:
        case SQL_SCHEMA_TERM:
        case SQL_USER_NAME:
          Argument::fromStdString(L"", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_MAX_OWNER_NAME_LEN:
          assert(BufferLength == sizeof(SQLUSMALLINT));
          *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = 128;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = 128\n");
          return SQL_SUCCESS;
        case SQL_NUMERIC_FUNCTIONS:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = SQL_FN_NUM_ABS | SQL_FN_NUM_ACOS | SQL_FN_NUM_ASIN
            | SQL_FN_NUM_ATAN | SQL_FN_NUM_COS | SQL_FN_NUM_EXP | SQL_FN_NUM_LOG | SQL_FN_NUM_PI
            | SQL_FN_NUM_POWER | SQL_FN_NUM_ROUND | SQL_FN_NUM_SIN | SQL_FN_NUM_SQRT | SQL_FN_NUM_RADIANS
            | SQL_FN_NUM_TAN | SQL_FN_NUM_SIGN | SQL_FN_NUM_MOD;
          return SQL_SUCCESS;
        case SQL_AGGREGATE_FUNCTIONS:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = SQL_AF_AVG | SQL_AF_COUNT | SQL_AF_DISTINCT
            | SQL_AF_MAX | SQL_AF_MIN | SQL_AF_SUM | SQL_AF_ALL;
          return SQL_SUCCESS;
        case SQL_STRING_FUNCTIONS:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = SQL_FN_STR_ASCII | SQL_FN_STR_CONCAT
            | SQL_FN_STR_LCASE | SQL_FN_STR_LENGTH | SQL_FN_STR_LOCATE | SQL_FN_STR_LTRIM
            | SQL_FN_STR_REPEAT | SQL_FN_STR_RTRIM | SQL_FN_STR_SPACE | SQL_FN_STR_SUBSTRING
            | SQL_FN_STR_UCASE;
          return SQL_SUCCESS;
        case SQL_SQL92_DATETIME_FUNCTIONS:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = 0L;
          return SQL_SUCCESS;
        case SQL_SQL92_STRING_FUNCTIONS:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = SQL_SSF_CONVERT | SQL_SSF_LOWER | SQL_SSF_UPPER
            | SQL_SSF_SUBSTRING | SQL_SSF_TRIM_BOTH | SQL_SSF_TRIM_LEADING | SQL_SSF_TRIM_TRAILING;
          return SQL_SUCCESS;
        case SQL_SQL92_VALUE_EXPRESSIONS:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = SQL_SVE_CASE | SQL_SVE_CAST | SQL_SVE_COALESCE;
          return SQL_SUCCESS;
        case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = 0L;
          return SQL_SUCCESS;
        case SQL_TIMEDATE_FUNCTIONS:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = SQL_FN_TD_HOUR | SQL_FN_TD_MINUTE | SQL_FN_TD_SECOND
            | SQL_FN_TD_YEAR | SQL_FN_TD_MONTH | SQL_FN_TD_DAYOFMONTH;
          return SQL_SUCCESS;
        case SQL_TIMEDATE_ADD_INTERVALS:
        case SQL_TIMEDATE_DIFF_INTERVALS:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = 0L;
          return SQL_SUCCESS;
        case SQL_CONVERT_BIGINT:
        case SQL_CONVERT_BINARY:
        case SQL_CONVERT_BIT:
        case SQL_CONVERT_CHAR:
        case SQL_CONVERT_DATE:
        case SQL_CONVERT_DECIMAL:
        case SQL_CONVERT_DOUBLE:
        case SQL_CONVERT_FLOAT:
        case SQL_CONVERT_INTEGER:
        case SQL_CONVERT_LONGVARCHAR:
        case SQL_CONVERT_NUMERIC:
        case SQL_CONVERT_REAL:
        case SQL_CONVERT_SMALLINT:
        case SQL_CONVERT_TIME:
        case SQL_CONVERT_TIMESTAMP:
        case SQL_CONVERT_TINYINT:
        case SQL_CONVERT_VARBINARY:
        case SQL_CONVERT_VARCHAR:
        case SQL_CONVERT_LONGVARBINARY:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = 0;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = 0\n");
          return SQL_SUCCESS;
        case SQL_CONVERT_FUNCTIONS:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = SQL_FN_CVT_CAST | SQL_FN_CVT_CONVERT;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = 0\n");
          return SQL_SUCCESS;
        case SQL_TXN_CAPABLE:
          *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_TC_DML;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = 1\n");
          return SQL_SUCCESS;
        case SQL_DBMS_NAME:
          Argument::fromStdString(L"CDAP Databases", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_DBMS_VER:
          Argument::fromStdString(L"00.00.0000", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_ODBC_INTERFACE_CONFORMANCE:
          *(reinterpret_cast<SQLUBIGINT*>(InfoValuePtr)) = SQL_OIC_CORE;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = 1UL\n");
          return SQL_SUCCESS;
        case SQL_SQL_CONFORMANCE:
          *(reinterpret_cast<SQLUBIGINT*>(InfoValuePtr)) = SQL_SC_SQL92_ENTRY;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = 1UL\n");
          return SQL_SUCCESS;
        case SQL_DRIVER_VER:
          temp = VersionInfo::getProductVersion();
          Argument::fromStdString(temp, static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_TABLE_TERM:
          Argument::fromStdString(L"TABLE", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
          return SQL_SUCCESS;
        case SQL_CATALOG_USAGE:
        case SQL_SCHEMA_USAGE:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = 0;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = 0\n");
          return SQL_SUCCESS;
        case SQL_OJ_CAPABILITIES:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = SQL_OJ_LEFT | SQL_OJ_RIGHT | SQL_OJ_FULL | SQL_OJ_INNER | SQL_OJ_NOT_ORDERED | SQL_OJ_NESTED;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = SQL_OJ_LEFT | SQL_OJ_RIGHT | SQL_OJ_FULL | SQL_OJ_INNER | SQL_OJ_NOT_ORDERED | SQL_OJ_NESTED\n");
          return SQL_SUCCESS;
        case SQL_QUOTED_IDENTIFIER_CASE:
          *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_IC_LOWER;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = SQL_IC_LOWER\n");
          return SQL_SUCCESS;
        case SQL_SQL92_RELATIONAL_JOIN_OPERATORS:
          *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = 
            SQL_SRJO_CORRESPONDING_CLAUSE |
            SQL_SRJO_CROSS_JOIN |
            SQL_SRJO_FULL_OUTER_JOIN |
            SQL_SRJO_INNER_JOIN |
            SQL_SRJO_LEFT_OUTER_JOIN |
            SQL_SRJO_NATURAL_JOIN |
            SQL_SRJO_RIGHT_OUTER_JOIN |
            SQL_SRJO_UNION_JOIN;
          TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = SQL_OJ_LEFT | SQL_OJ_RIGHT | SQL_OJ_FULL | SQL_OJ_INNER | SQL_OJ_NOT_ORDERED | SQL_OJ_NESTED\n");
          return SQL_SUCCESS;
        default:
          throw CdapException(L"Unknown info type.");
      }
    } catch (CdapException& cex) {
      TRACE(L"SQLGetFunctions returns SQL_ERROR\n");
      connection.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLGetFunctions returns SQL_ERROR\n");
      connection.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLGetInfoW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLGetInfoW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLDriversW(
  SQLHENV         EnvironmentHandle,
  SQLUSMALLINT    Direction,
  SQLWCHAR *       DriverDescription,
  SQLSMALLINT     BufferLength1,
  SQLSMALLINT *   DescriptionLengthPtr,
  SQLWCHAR *       DriverAttributes,
  SQLSMALLINT     BufferLength2,
  SQLSMALLINT *   AttributesLengthPtr) {
  TRACE(L"SQLDriversW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetFunctions(
  SQLHDBC           ConnectionHandle,
  SQLUSMALLINT      FunctionId,
  SQLUSMALLINT *    SupportedPtr) {
  TRACE(L"SQLGetFunctions(ConnectionHandle = %X, FunctionId = %d)\n", ConnectionHandle, FunctionId);
  try {
    auto& connection = Driver::getInstance().getConnection(ConnectionHandle);
    std::lock_guard<Connection> lock(connection);
    try {
      if (FunctionId == SQL_API_ODBC3_ALL_FUNCTIONS) {
        Driver::getInstance().setupSupportedFunctions(SupportedPtr);
        TRACE(L"SQLGetFunctions returns SQL_SUCCESS\n");
        return SQL_SUCCESS;
      } else if (FunctionId >= 0 && FunctionId < SQL_API_ODBC3_ALL_FUNCTIONS_SIZE) {
        switch (FunctionId) {
          case SQL_API_SQLALLOCHANDLE:
          case SQL_API_SQLBINDCOL:
          case SQL_API_SQLCANCEL:
          case SQL_API_SQLGETFUNCTIONS:
          case SQL_API_SQLGETINFO:
          case SQL_API_SQLDISCONNECT:
          case SQL_API_SQLPREPARE:
          case SQL_API_SQLEXECDIRECT:
          case SQL_API_SQLEXECUTE:
          case SQL_API_SQLFETCH:
          case SQL_API_SQLFREEHANDLE:
          case SQL_API_SQLFREESTMT:
          case SQL_API_SQLCOLUMNS:
          case SQL_API_SQLTABLES:
          case SQL_API_SQLBINDPARAMETER:
          case SQL_API_SQLNUMPARAMS:
          case SQL_API_SQLDRIVERCONNECT:
          case SQL_API_SQLMORERESULTS:
            *SupportedPtr = SQL_TRUE;
            break;
          default:
            *SupportedPtr = SQL_FALSE;
        }

        TRACE(L"SQLGetFunctions returns SQL_SUCCESS\n");
        return SQL_SUCCESS;
      } else {
        throw CdapException(L"Unsupported FunctionId.");
      }
    } catch (CdapException& cex) {
      TRACE(L"SQLGetFunctions returns SQL_ERROR\n");
      connection.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLGetFunctions returns SQL_ERROR\n");
      connection.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLGetFunctions returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLGetFunctions returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLSetConnectAttrW(
  SQLHDBC       ConnectionHandle,
  SQLINTEGER    Attribute,
  SQLPOINTER    ValuePtr,
  SQLINTEGER    StringLength) {
  TRACE(L"SQLSetConnectAttrW(ConnectionHandle = %X, Attribute = %d)\n", ConnectionHandle, Attribute);
  // Attrs not supported 
  // Always report success 
  TRACE(L"SQLSetConnectAttrW returns SQL_SUCCESS\n");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetConnectAttrW(
  SQLHDBC        ConnectionHandle,
  SQLINTEGER     Attribute,
  SQLPOINTER     ValuePtr,
  SQLINTEGER     BufferLength,
  SQLINTEGER *   StringLengthPtr) {
  TRACE(L"SQLGetConnectAttrW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetStmtAttrW(
  SQLHSTMT        StatementHandle,
  SQLINTEGER      Attribute,
  SQLPOINTER      ValuePtr,
  SQLINTEGER      BufferLength,
  SQLINTEGER *    StringLengthPtr) {
  // Dummy address for setting dummy statement descriptors.
  static char dummyDesc = '\0';

  TRACE(L"SQLGetStmtAttrW(StatementHandle = %X, Attribute = %d)\n", StatementHandle, Attribute);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      switch (Attribute) {
        case SQL_ATTR_APP_PARAM_DESC:
        case SQL_ATTR_APP_ROW_DESC:
        case SQL_ATTR_IMP_ROW_DESC:
        case SQL_ATTR_IMP_PARAM_DESC:
          assert(BufferLength == SQL_IS_POINTER);
          // Requires by MSQUERY to be not null.
          // Set to dummy pointer.
          *(static_cast<SQLHDESC*>(ValuePtr)) = &dummyDesc;
          TRACE(L"SQLGetStmtAttrW returns SQL_SUCCESS, *ValuePtr = dummy descriptor\n");
          return SQL_SUCCESS;
        default:
          throw CdapException(L"Unknown attribute.");
      }
    } catch (CdapException& cex) {
      TRACE(L"SQLGetStmtAttrW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLGetStmtAttrW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLGetStmtAttrW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLGetStmtAttrW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLPrepareW(
  SQLHSTMT      StatementHandle,
  SQLWCHAR *     StatementText,
  SQLINTEGER    TextLength) {
  TRACE(L"SQLPrepareW(StatementHandle = %X, StatementText = %s)\n", StatementHandle, StatementText);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      auto sql = Argument::toStdString(StatementText, static_cast<SQLSMALLINT>(TextLength));
      if (!sql) {
        throw CdapException(L"Statement text cannot be empty.");
      }

      statement.prepare(*sql);
      statement.execute();

      TRACE(L"SQLPrepareW returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    } catch (CdapException& cex) {
      TRACE(L"SQLPrepareW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLPrepareW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLPrepareW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLPrepareW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLBindParameter(
  SQLHSTMT        StatementHandle,
  SQLUSMALLINT    ParameterNumber,
  SQLSMALLINT     InputOutputType,
  SQLSMALLINT     ValueType,
  SQLSMALLINT     ParameterType,
  SQLULEN         ColumnSize,
  SQLSMALLINT     DecimalDigits,
  SQLPOINTER      ParameterValuePtr,
  SQLLEN          BufferLength,
  SQLLEN *        StrLen_or_IndPtr) {
  TRACE(L"SQLBindParameter\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLExecute(
  SQLHSTMT     StatementHandle) {
  TRACE(L"SQLExecute(StatementHandle = %X)\n", StatementHandle);
  // For Tableau, statement execution moved to SQLPrepare
  // auto& statement = Driver::getInstance().getStatement(StatementHandle);
  // statement.execute();
  TRACE(L"SQLExecute returns SQL_SUCCESS\n");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLExecDirectW(
  SQLHSTMT     StatementHandle,
  SQLWCHAR *   StatementText,
  SQLINTEGER   TextLength) {
  TRACE(L"SQLExecDirectW(StatementHandle = %X, StatementText = %s)\n", StatementHandle, StatementText);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      auto query = Argument::toStdString(StatementText, static_cast<SQLSMALLINT>(TextLength));
      if (!query) {
        throw CdapException(L"Statement text cannot be empty.");
      }

      statement.executeDirect(*query);
      TRACE(L"SQLExecDirectW returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    } catch (CdapException& cex) {
      TRACE(L"SQLExecDirectW returns %d\n", cex.getErrorCode());
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLExecDirectW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLExecDirectW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLExecDirectW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLNumParams(
  SQLHSTMT        StatementHandle,
  SQLSMALLINT *   ParameterCountPtr) {
  TRACE(L"SQLNumParams\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLParamData(
  SQLHSTMT       StatementHandle,
  SQLPOINTER *   ValuePtrPtr) {
  TRACE(L"SQLParamData\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLNumResultCols(
  SQLHSTMT        StatementHandle,
  SQLSMALLINT *   ColumnCountPtr) {
  TRACE(L"SQLNumResultCols(StatementHandle = %X)\n", StatementHandle);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      if (!ColumnCountPtr) {
        TRACE(L"SQLNumResultCols returns SQL_ERROR\n");
        return SQL_ERROR;
      }

      *ColumnCountPtr = statement.getColumnCount();
      TRACE(L"SQLNumResultCols returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    } catch (CdapException& cex) {
      TRACE(L"SQLNumResultCols returns SQL_ERROR\n");
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLNumResultCols returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLNumResultCols returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLNumResultCols returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLDescribeColW(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   ColumnNumber,
  SQLWCHAR *     ColumnName,
  SQLSMALLINT    BufferLength,
  SQLSMALLINT *  NameLengthPtr,
  SQLSMALLINT *  DataTypePtr,
  SQLULEN *      ColumnSizePtr,
  SQLSMALLINT *  DecimalDigitsPtr,
  SQLSMALLINT *  NullablePtr) {
  TRACE(L"SQLDescribeColW(StatementHandle = %X, ColumnNumber = %d)\n", StatementHandle, ColumnNumber);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      auto columnInfo = statement.getColumnInfo(ColumnNumber);

      Argument::fromStdString(columnInfo->getName(), ColumnName, BufferLength, NameLengthPtr);

      if (DataTypePtr) {
        *DataTypePtr = columnInfo->getDataType().getSqlType();
      }

      if (ColumnSizePtr) {
        *ColumnSizePtr = columnInfo->getDataType().getSize();
      }

      if (DecimalDigitsPtr) {
        *DecimalDigitsPtr = columnInfo->getDataType().getDecimalDigits();
      }

      if (NullablePtr) {
        *NullablePtr = columnInfo->getDataType().getNullable();
      }

      TRACE(L"SQLDescribeColW returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    } catch (CdapException& cex) {
      TRACE(L"SQLDescribeColW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLDescribeColW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLDescribeColW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLDescribeColW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLColAttributeW(
  SQLHSTMT        StatementHandle,
  SQLUSMALLINT    ColumnNumber,
  SQLUSMALLINT    FieldIdentifier,
  SQLPOINTER      CharacterAttributePtr,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLengthPtr,
  SQLLEN *        NumericAttributePtr) {
  TRACE(
    L"SQLColAttributeW(StatementHandle = %X, ColumnNumber = %d, FieldIdentifier = %d)\n",
    StatementHandle,
    ColumnNumber,
    FieldIdentifier);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      auto& columnInfo = statement.getColumnInfo(ColumnNumber);
      switch (FieldIdentifier) {

        /* Column attributes */
        case SQL_COLUMN_TYPE:
        case SQL_DESC_TYPE: /*SQL_DESC_CONCISE_TYPE in ODBC 2.x */
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getSqlType();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        case SQL_COLUMN_DISPLAY_SIZE:
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getDisplaySize();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        case SQL_DESC_PRECISION:  /* ODBC 3.x*/
        case SQL_COLUMN_PRECISION: /* ODBC 2.x*/
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getPrecision();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        case SQL_COLUMN_NULLABLE:
        case SQL_DESC_NULLABLE:
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getNullable();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        case SQL_COLUMN_UNSIGNED:
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getUnsigned();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        case SQL_COLUMN_SEARCHABLE:
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getSearchable();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        case SQL_DESC_FIXED_PREC_SCALE:
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getFixedPrecScale();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        case SQL_COLUMN_CASE_SENSITIVE:
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getCaseSensitive();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        case SQL_COLUMN_TYPE_NAME:
          if (CharacterAttributePtr) {
            Argument::fromStdString(
              columnInfo->getDataType().getName(),
              static_cast<SQLWCHAR*>(CharacterAttributePtr),
              BufferLength,
              StringLengthPtr);
            TRACE(
              L"SQLColAttributeW returns SQL_SUCCESS, *CharacterAttributePtr = %s\n",
              static_cast<SQLWCHAR*>(CharacterAttributePtr));
            return SQL_SUCCESS;
          }

          break;
        case SQL_COLUMN_LABEL:
          if (CharacterAttributePtr) {
            Argument::fromStdString(
              columnInfo->getShortName(),
              static_cast<SQLWCHAR*>(CharacterAttributePtr),
              BufferLength,
              StringLengthPtr);
            TRACE(
              L"SQLColAttributeW returns SQL_SUCCESS, *CharacterAttributePtr = %s\n",
              static_cast<SQLWCHAR*>(CharacterAttributePtr));
            return SQL_SUCCESS;
          }

          break;
        /* Not implemented column attributes*/
        case SQL_COLUMN_UPDATABLE:
        case SQL_COLUMN_AUTO_INCREMENT:
        case SQL_COLUMN_TABLE_NAME:
        case SQL_COLUMN_OWNER_NAME:
        case SQL_COLUMN_QUALIFIER_NAME:
          break;

        case SQL_COLUMN_SCALE: /* ODBC 2.x */
        case SQL_DESC_SCALE: /* ODBC 3.x */
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getScale();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        case SQL_DESC_BASE_COLUMN_NAME:
          if (CharacterAttributePtr) {
            Argument::fromStdString(
              columnInfo->getShortName(),
              static_cast<SQLWCHAR*>(CharacterAttributePtr),
              BufferLength,
              StringLengthPtr);
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *CharacterAttributePtr = %s\n",
              static_cast<SQLWCHAR*>(CharacterAttributePtr)
              );
            return SQL_SUCCESS;
          }

          break;
        case SQL_COLUMN_LENGTH: /* ODBC 2.x */
        case SQL_DESC_LENGTH:   /* ODBC 3.x */
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getSize();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        case SQL_DESC_NAME:
          if (CharacterAttributePtr) {
            Argument::fromStdString(
              columnInfo->getShortName(),
              static_cast<SQLWCHAR*>(CharacterAttributePtr),
              BufferLength,
              StringLengthPtr
              );
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *CharacterAttributePtr = %s\n",
              static_cast<SQLWCHAR*>(CharacterAttributePtr)
              );
            return SQL_SUCCESS;
          }

          break;
        case SQL_DESC_NUM_PREC_RADIX:
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getPrecRadix();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        case SQL_DESC_OCTET_LENGTH:
          if (NumericAttributePtr) {
            *NumericAttributePtr = columnInfo->getDataType().getOctetLength();
            TRACE(L"SQLColAttributeW returns SQL_SUCCESS, *NumericAttributePtr = %d\n", *NumericAttributePtr);
            return SQL_SUCCESS;
          }

          break;
        default:
          throw CdapException(L"Unsupported field.");
      }

      TRACE(L"SQLColAttributeW returns SQL_ERROR\n");
      return SQL_ERROR;
    } catch (CdapException& cex) {
      TRACE(L"SQLColAttributeW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLColAttributeW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLColAttributeW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLColAttributeW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLBindCol(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   ColumnNumber,
  SQLSMALLINT    TargetType,
  SQLPOINTER     TargetValuePtr,
  SQLLEN         BufferLength,
  SQLLEN *       StrLen_or_Ind) {
  TRACE(
    L"SQLBindCol(StatementHandle = %X, ColumnNumber = %d, TargetType = %d, BufferLength = %d)\n",
    StatementHandle,
    ColumnNumber,
    TargetType,
    BufferLength
  );
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      if (TargetValuePtr != nullptr) {
        ColumnBinding binding;
        binding.setColumnNumber(ColumnNumber);
        binding.setTargetType(TargetType);
        binding.setTargetValuePtr(TargetValuePtr);
        binding.setBufferLength(BufferLength);
        binding.setStrLenOrInd(StrLen_or_Ind);
        statement.addColumnBinding(binding);
      } else {
        statement.removeColumnBinding(ColumnNumber);
    } // end try

      TRACE(L"SQLBindCol returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    } catch (CdapException& cex) {
      TRACE(L"SQLBindCol returns SQL_ERROR\n");
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLBindCol returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLBindCol returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLBindCol returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLFetch(
  SQLHSTMT     StatementHandle) {
  TRACE(L"SQLFetch(StatementHandle = %X)\n", StatementHandle);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      statement.fetch();
      TRACE(L"SQLFetch returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    } catch (CdapException& cex) {
      TRACE(L"SQLFetch returns %d\n", cex.getErrorCode());
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLFetch returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLFetch returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLFetch returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLGetData(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   Col_or_Param_Num,
  SQLSMALLINT    TargetType,
  SQLPOINTER     TargetValuePtr,
  SQLLEN         BufferLength,
  SQLLEN *       StrLen_or_IndPtr) {
  TRACE(L"SQLGetData\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetPos(
  SQLHSTMT        StatementHandle,
  SQLSETPOSIROW   RowNumber,
  SQLUSMALLINT    Operation,
  SQLUSMALLINT    LockType) {
  TRACE(L"SQLSetPos\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLMoreResults(
  SQLHSTMT     StatementHandle) {
  TRACE(L"SQLMoreResults\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLColumnsW(
  SQLHSTMT       StatementHandle,
  SQLWCHAR *      CatalogName,
  SQLSMALLINT    NameLength1,
  SQLWCHAR *      SchemaName,
  SQLSMALLINT    NameLength2,
  SQLWCHAR *      TableName,
  SQLSMALLINT    NameLength3,
  SQLWCHAR *      ColumnName,
  SQLSMALLINT    NameLength4) {
  TRACE(
    L"SQLColumnsW(StatementHandle = %X, CatalogName = %s, SchemaName = %s, TableName = %s, ColumnName = %s)\n",
    StatementHandle,
    CatalogName,
    SchemaName,
    TableName,
    ColumnName
  );
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      auto streamName = Argument::toStdString(TableName, NameLength3);
      if (!streamName) {
        throw CdapException(L"Table name cannot be empty.");
      }

      statement.getColumns(*streamName);
      TRACE(L"SQLColumnsW returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    } catch (CdapException& cex) {
      TRACE(L"SQLColumnsW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLColumnsW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLColumnsW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLColumnsW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLPrimaryKeysW(
  SQLHSTMT       StatementHandle,
  SQLWCHAR *     CatalogName,
  SQLSMALLINT    NameLength1,
  SQLWCHAR *     SchemaName,
  SQLSMALLINT    NameLength2,
  SQLWCHAR *     TableName,
  SQLSMALLINT    NameLength3) {
  TRACE(L"SQLPrimaryKeysW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLTablesW(
  SQLHSTMT       StatementHandle,
  SQLWCHAR *     CatalogName,
  SQLSMALLINT    NameLength1,
  SQLWCHAR *     SchemaName,
  SQLSMALLINT    NameLength2,
  SQLWCHAR *     TableName,
  SQLSMALLINT    NameLength3,
  SQLWCHAR *     TableType,
  SQLSMALLINT    NameLength4) {
  TRACE(
    L"SQLTablesW(StatementHandle = %X, CatalogName = %s, SchemaName = %s, TableName = %s, TableType = %s)\n",
    StatementHandle,
    CatalogName,
    SchemaName,
    TableName,
    TableType);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();

      auto catalogName = Argument::toStdString(CatalogName, NameLength1);
      auto schemaName = Argument::toStdString(SchemaName, NameLength2);
      auto streamName = Argument::toStdString(TableName, NameLength3);
      auto tableTypes = Argument::toStdString(TableType, NameLength4);

      if (schemaName && *schemaName == L"%") {
        statement.getSchemas(catalogName.get(), schemaName.get());
        TRACE(L"SQLTablesW returns SQL_SUCCESS\n");
        return SQL_SUCCESS;
      } else if (tableTypes) {
        statement.getTables(catalogName.get(), schemaName.get(), streamName.get(), tableTypes.get());
        TRACE(L"SQLTablesW returns SQL_SUCCESS\n");
        return SQL_SUCCESS;
      }

      throw CdapException(L"HYC00", L"Optional feature not implemented.");
    } catch (CdapException& cex) {
      TRACE(L"SQLTablesW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLTablesW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLTablesW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLTablesW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLSpecialColumnsW(
  SQLHSTMT      StatementHandle,
  SQLSMALLINT   IdentifierType,
  SQLWCHAR *    CatalogName,
  SQLSMALLINT   NameLength1,
  SQLWCHAR *    SchemaName,
  SQLSMALLINT   NameLength2,
  SQLWCHAR *    TableName,
  SQLSMALLINT   NameLength3,
  SQLSMALLINT   Scope,
  SQLSMALLINT   Nullable) {
  TRACE(
    L"SQLSpecialColumnsW(StatementHandle = %X, IdentifierType = %d, CatalogName = %s, SchemaName = %s, TableName = %s, Scope = %d, Nullable = %d)\n",
    StatementHandle,
    IdentifierType,
    CatalogName,
    SchemaName,
    TableName,
    Scope,
    Nullable);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      statement.getSpecialColumns();
      TRACE(L"SQLSpecialColumnsW returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    } catch (CdapException& cex) {
      TRACE(L"SQLSpecialColumnsW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLSpecialColumnsW returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLSpecialColumnsW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLSpecialColumnsW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLFreeStmt(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   Option) {
  TRACE(L"SQLFreeStmt(StatementHandle = %X, Option = %d)\n", StatementHandle, Option);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    std::lock_guard<Connection> lock(*statement.getConnection());
    try {
      statement.getSqlStatus().clear();
      switch (Option) {
        case SQL_CLOSE:
          statement.reset();
          TRACE(L"SQLFreeStmt returns SQL_SUCCESS\n");
          return SQL_SUCCESS;
        case SQL_UNBIND:
          statement.unbindColumns();
          TRACE(L"SQLFreeStmt returns SQL_SUCCESS\n");
          return SQL_SUCCESS;
        case SQL_RESET_PARAMS:
          statement.resetParameters();
          TRACE(L"SQLFreeStmt returns SQL_SUCCESS\n");
          return SQL_SUCCESS;
        default:
          throw CdapException(L"Unknown option.");
      }
    } catch (CdapException& cex) {
      TRACE(L"SQLFreeStmt returns SQL_ERROR\n");
      statement.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLFreeStmt returns SQL_ERROR\n");
      statement.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLFreeStmt returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLFreeStmt returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLCloseCursor(
  SQLHSTMT     StatementHandle) {
  TRACE(L"SQLCloseCursor\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLCancel(
  SQLHSTMT     StatementHandle) {
  TRACE(L"SQLCancel\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLDisconnect(
  SQLHDBC     ConnectionHandle) {
  TRACE(L"SQLDisconnect(ConnectionHandle = %X)\n", ConnectionHandle);
  try {
    auto& connection = Driver::getInstance().getConnection(ConnectionHandle);
    std::lock_guard<Connection> lock(connection);
    try {
      connection.close();
      TRACE(L"SQLDisconnect returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    } catch (CdapException& cex) {
      TRACE(L"SQLDisconnect returns SQL_ERROR\n");
      connection.getSqlStatus().addError(cex);
      return cex.getErrorCode();
    } catch (std::exception& ex) {
      TRACE(L"SQLDisconnect returns SQL_ERROR\n");
      connection.getSqlStatus().addError(ex);
      return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLDisconnect returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLDisconnect returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLFreeHandle(
  SQLSMALLINT   HandleType,
  SQLHANDLE     Handle) {
  TRACE(L"SQLFreeHandle(HandleType = %d, Handle = %X)\n", HandleType, Handle);
  if (!Handle) {
    TRACE(L"SQLFreeHandle returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  }

  try {
    switch (HandleType) {
      case SQL_HANDLE_ENV:
        Driver::getInstance().freeEnvironment(Handle);
        TRACE(L"SQLFreeHandle returns SQL_SUCCESS\n");
        return SQL_SUCCESS;

      case SQL_HANDLE_DBC:
        Driver::getInstance().freeConnection(Handle);
        TRACE(L"SQLFreeHandle returns SQL_SUCCESS\n");
        return SQL_SUCCESS;

      case SQL_HANDLE_STMT:
        Driver::getInstance().freeStatement(Handle);
        TRACE(L"SQLFreeHandle returns SQL_SUCCESS\n");
        return SQL_SUCCESS;

      case SQL_HANDLE_DESC:
        Driver::getInstance().freeDescriptor(Handle);
        TRACE(L"SQLFreeHandle returns SQL_SUCCESS\n");
        return SQL_SUCCESS;

      default:
        TRACE(L"SQLFreeHandle returns SQL_ERROR\n");
        return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    TRACE(L"SQLFreeHandle returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    TRACE(L"SQLFreeHandle returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLCopyDesc(
  SQLHDESC     SourceDescHandle,
  SQLHDESC     TargetDescHandle) {
  TRACE(L"SQLCopyDesc\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLEndTran(
  SQLSMALLINT   HandleType,
  SQLHANDLE     Handle,
  SQLSMALLINT   CompletionType) {
  TRACE(L"SQLEndTran\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLBulkOperations(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   Operation) {
  TRACE(L"SQLBulkOperations\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLExtendedFetch(
  SQLHSTMT         StatementHandle,
  SQLUSMALLINT     FetchOrientation,
  SQLLEN           FetchOffset,
  SQLULEN *        RowCountPtr,
  SQLUSMALLINT *   RowStatusArray) {
  TRACE(L"SQLExtendedFetch\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLPutData(
  SQLHSTMT     StatementHandle,
  SQLPOINTER   DataPtr,
  SQLLEN       StrLen_or_Ind) {
  TRACE(L"SQLPutData\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLFetchScroll(
  SQLHSTMT      StatementHandle,
  SQLSMALLINT   FetchOrientation,
  SQLLEN        FetchOffset) {
  TRACE(L"SQLFetchScroll\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetCursorNameW(
  SQLHSTMT StatementHandle,
  SQLWCHAR *CursorName,
  SQLSMALLINT BufferLength,
  SQLSMALLINT *NameLengthPtr) {
  TRACE(L"SQLGetCursorNameW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetDescFieldW(
  SQLHDESC DescriptorHandle,
  SQLSMALLINT RecNumber,
  SQLSMALLINT FieldIdentifier,
  SQLPOINTER Value,
  SQLINTEGER BufferLength,
  SQLINTEGER *StringLength) {
  TRACE(L"SQLGetDescFieldW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetDescRecW(
  SQLHDESC DescriptorHandle,
  SQLSMALLINT RecNumber,
  SQLWCHAR *Name,
  SQLSMALLINT BufferLength,
  SQLSMALLINT *StringLengthPtr,
  SQLSMALLINT *TypePtr,
  SQLSMALLINT *SubTypePtr,
  SQLLEN *LengthPtr,
  SQLSMALLINT *PrecisionPtr,
  SQLSMALLINT *ScalePtr,
  SQLSMALLINT *NullablePtr) {
  TRACE(L"SQLGetDescRecW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetDiagFieldW(
  SQLSMALLINT HandleType,
  SQLHANDLE Handle,
  SQLSMALLINT RecNumber,
  SQLSMALLINT DiagIdentifier,
  SQLPOINTER DiagInfo,
  SQLSMALLINT BufferLength,
  SQLSMALLINT *StringLength) {
  TRACE(
    L"SQLGetDiagFieldW (HandleType = %d, Handle = %X, RecNumber = %d, DiagIdentifier = %d, DiagInfo = %X, BufferLength = %d, StringLength = %d)\n",
    HandleType,
    Handle,
    RecNumber,
    DiagIdentifier,
    DiagInfo,
    BufferLength,
    StringLength);

  SQLStatus status;

  switch (HandleType) {
    case SQL_HANDLE_DBC:
      status = Driver::getInstance().getConnection(Handle).getSqlStatus();
      break;
    case SQL_HANDLE_STMT:
      status = Driver::getInstance().getStatement(Handle).getSqlStatus();
      break;
    case SQL_HANDLE_ENV:
    case SQL_HANDLE_DESC:
    default:
      return SQL_NO_DATA;
  }

  if (status.getRecCount() + 1 < RecNumber) {
    return SQL_NO_DATA;
  }

  auto& code = status.getCode(RecNumber);
  std::wstring logmsg;
  SQLSMALLINT copiedLen = 0;
  switch (DiagIdentifier) {
    case SQL_DIAG_CLASS_ORIGIN:
      logmsg = L"ISO 9075";
      if (code[0] == L'I' && code[1] == L'M') {
        logmsg = L"ODBC 3.0";
      }

      break;
    case SQL_DIAG_SUBCLASS_ORIGIN:
      logmsg = L"ISO 9075";
      if (code[0] == L'I' && code[1] == L'M') {
        logmsg = L"ODBC 3.0";
      } else if (code[0] == L'H' && code[1] == L'Y') {
        logmsg = L"ODBC 3.0";
      } else if (code[0] == L'2' || code[0] == L'0' || code[0] == L'4') {
        logmsg = L"ODBC 3.0";
      }

      break;
    case SQL_DIAG_CONNECTION_NAME:
    case SQL_DIAG_SERVER_NAME:
      // TODO get connection name from driver
      logmsg = L"NO DSN";
      break;
    case SQL_DIAG_SQLSTATE:
      logmsg = code;
      break;
    default:
      return SQL_NO_DATA;
  }

  if (BufferLength > 0) {
    Argument::fromStdString(logmsg, static_cast<SQLWCHAR*>(DiagInfo), BufferLength, &copiedLen);
  }

  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetDiagRecW(
  SQLSMALLINT HandleType,
  SQLHANDLE Handle,
  SQLSMALLINT RecNumber,
  SQLWCHAR *Sqlstate,
  SQLINTEGER *NativeError,
  SQLWCHAR* MessageText,
  SQLSMALLINT BufferLength,
  SQLSMALLINT *TextLength) {
  TRACE(
    L"SQLGetDiagRecW (HandleType = %d, Handle = %X, RecNumber = %d, Sqlstate = %d, NativeError = %X, MessageText = %s, BufferLength = %d, TextLength = )\n",
    HandleType,
    Handle,
    RecNumber,
    Sqlstate,
    NativeError,
    MessageText,
    BufferLength,
    TextLength);

  SQLStatus status;

  switch (HandleType) {
    case SQL_HANDLE_DBC:
      status = Driver::getInstance().getConnection(Handle).getSqlStatus();
      break;
    case SQL_HANDLE_STMT:
      status = Driver::getInstance().getStatement(Handle).getSqlStatus();
      break;
    case SQL_HANDLE_ENV:
    case SQL_HANDLE_DESC:
    default:
      return SQL_NO_DATA;
  }

  if (BufferLength < 0) {
    return SQL_ERROR;
  }

  if (status.getRecCount() + 1 < RecNumber) {
    return SQL_NO_DATA;
  }

  auto& code = status.getCode(RecNumber);
  auto& msg = status.getMessage(RecNumber);

  if (msg.length()) {
    *TextLength = (SQLSMALLINT)msg.size();
  }

  SQLSMALLINT copiedLen = 0;
  if (code.length() > 0) {
    Argument::fromStdString(code, Sqlstate, 5, &copiedLen);
    if (BufferLength > 0) {
      Argument::fromStdString(msg, MessageText, BufferLength, &copiedLen);
    }

    return SQL_SUCCESS;
  }

  return SQL_ERROR;
}

SQLRETURN SQL_API SQLNativeSqlW(
  SQLHDBC        ConnectionHandle,
  SQLWCHAR *     InStatementText,
  SQLINTEGER     TextLength1,
  SQLWCHAR *     OutStatementText,
  SQLINTEGER     BufferLength,
  SQLINTEGER *   TextLength2Ptr) {
  TRACE(L"SQLNativeSqlW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLProcedureColumnsW(
  SQLHSTMT    hstmt,
  SQLWCHAR*   szCatalogName,
  SQLSMALLINT cchCatalogName,
  SQLWCHAR*   szSchemaName,
  SQLSMALLINT cchSchemaName,
  SQLWCHAR*   szProcName,
  SQLSMALLINT cchProcName,
  SQLWCHAR*   szColumnName,
  SQLSMALLINT cchColumnName) {
  TRACE(L"SQLProcedureColumnsW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLProceduresW(
  SQLHSTMT    hstmt,
  SQLWCHAR*   szCatalogName,
  SQLSMALLINT cchCatalogName,
  SQLWCHAR*   szSchemaName,
  SQLSMALLINT cchSchemaName,
  SQLWCHAR*   szProcName,
  SQLSMALLINT cchProcName) {
  TRACE(L"SQLProceduresW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLRowCount(
  SQLHSTMT StatementHandle,
  SQLLEN* RowCount) {
  TRACE(L"SQLRowCount\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetConnectOptionW(
  SQLHDBC ConnectionHandle,
  SQLUSMALLINT Option,
  SQLULEN Value) {
  TRACE(L"SQLSetConnectOptionW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetCursorNameW(
  SQLHSTMT StatementHandle,
  SQLWCHAR* CursorName,
  SQLSMALLINT NameLength) {
  TRACE(L"SQLSetCursorNameW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetDescFieldW(
  SQLHDESC DescriptorHandle,
  SQLSMALLINT RecNumber,
  SQLSMALLINT FieldIdentifier,
  SQLPOINTER Value,
  SQLINTEGER BufferLength) {
  TRACE(L"SQLSetDescFieldW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetDescRec(
  SQLHDESC DescriptorHandle,
  SQLSMALLINT RecNumber, SQLSMALLINT Type,
  SQLSMALLINT SubType, SQLLEN Length,
  SQLSMALLINT Precision, SQLSMALLINT Scale,
  SQLPOINTER Data,
  SQLLEN *StringLength,
  SQLLEN *Indicator) {
  TRACE(L"SQLSetDescRec\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetEnvAttr(
  SQLHENV EnvironmentHandle,
  SQLINTEGER Attribute,
  SQLPOINTER Value,
  SQLINTEGER StringLength) {
  TRACE(L"SQLSetEnvAttr(EnvironmentHandle = %X, Attribute = %d)\n", EnvironmentHandle, Attribute);
  TRACE(L"SQLSetEnvAttr returns SQL_SUCCESS\n");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetStmtAttrW(
  SQLHSTMT StatementHandle,
  SQLINTEGER Attribute,
  SQLPOINTER Value,
  SQLINTEGER StringLength) {
  TRACE(L"SQLSetStmtAttrW(StatementHandle = %X, Attribute = %d)\n", StatementHandle, Attribute);
  TRACE(L"SQLSetStmtAttrW returns SQL_SUCCESS\n");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLStatisticsW(
  SQLHSTMT StatementHandle,
  SQLCHAR *CatalogName,
  SQLSMALLINT NameLength1,
  SQLCHAR *SchemaName,
  SQLSMALLINT NameLength2,
  SQLCHAR *TableName,
  SQLSMALLINT NameLength3,
  SQLUSMALLINT Unique,
  SQLUSMALLINT Reserved) {
  TRACE(L"SQLStatisticsW\n");
  return SQL_ERROR;
}