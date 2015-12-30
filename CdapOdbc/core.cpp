/*
 * Copyright � 2015 Cask Data, Inc.
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
#include "StillExecutingException.h"
#include "NoDataException.h"
#include "Driver.h"
#include "Environment.h"
#include "Connection.h"
#include "Argument.h"
#include "Statement.h"

using namespace Cask::CdapOdbc;

SQLRETURN SQL_API SQLAllocHandle(
  SQLSMALLINT   HandleType,
  SQLHANDLE     InputHandle,
  SQLHANDLE *   OutputHandlePtr) {
  TRACE(L"SQLAllocHandle\n");
  try {
    // OutputHandlePtr must be a valid pointer
    if (!OutputHandlePtr) {
      return SQL_ERROR;
    }

    *OutputHandlePtr = SQL_NULL_HANDLE;

    switch (HandleType) {
      case SQL_HANDLE_ENV:
        // InputHandle must be NULL
        if (InputHandle != SQL_NULL_HANDLE) {
          return SQL_ERROR;
        }

        // Allocate new environment
        *OutputHandlePtr = Driver::getInstance().allocEnvironment();
        return SQL_SUCCESS;

      case SQL_HANDLE_DBC:
        // InputHandle must not be NULL
        if (InputHandle == SQL_NULL_HANDLE) {
          return SQL_ERROR;
        }

        *OutputHandlePtr = Driver::getInstance().allocConnection(InputHandle);
        return SQL_SUCCESS;

      case SQL_HANDLE_STMT:
        // InputHandle must not be NULL
        if (InputHandle == SQL_NULL_HANDLE) {
          return SQL_ERROR;
        }

        *OutputHandlePtr = Driver::getInstance().allocStatement(InputHandle);
        return SQL_SUCCESS;

      case SQL_HANDLE_DESC:
        // InputHandle must not be NULL
        if (InputHandle == SQL_NULL_HANDLE) {
          return SQL_ERROR;
        }

        *OutputHandlePtr = Driver::getInstance().allocDescriptor(InputHandle);
        return SQL_SUCCESS;

      default:
        return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
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
  TRACE(L"SQLDriverConnectW\n");
  try {
    auto& connection = Driver::getInstance().getConnection(ConnectionHandle);
    std::wstring connectionString;
    switch (DriverCompletion) {
      case SQL_DRIVER_PROMPT:
        // DRIVER
        connectionString = Argument::toStdString(InConnectionString, StringLength1);
        connectionString += L"HOST=localhost;PORT=10000";
        connection.open(connectionString);
        Argument::fromStdString(connectionString, OutConnectionString, BufferLength, StringLength2Ptr);
        return SQL_SUCCESS;
      case SQL_DRIVER_COMPLETE:
      case SQL_DRIVER_COMPLETE_REQUIRED:
        // DSN
        connectionString = Argument::toStdString(InConnectionString, StringLength1);
        connection.open(L"HOST=localhost;PORT=10000");
        Argument::fromStdString(connectionString, OutConnectionString, BufferLength, StringLength2Ptr);
        return SQL_SUCCESS;
      case SQL_DRIVER_NOPROMPT:
        // DRIVER 2
        connectionString = Argument::toStdString(InConnectionString, StringLength1);
        connection.open(connectionString);
        return SQL_SUCCESS;
    }

    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
    return SQL_ERROR;
  }
 }

SQLRETURN SQL_API SQLBrowseConnectW(
  SQLHDBC         ConnectionHandle,
  SQLWCHAR *      InConnectionString,
  SQLSMALLINT     StringLength1,
  SQLWCHAR *      OutConnectionString,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLength2Ptr) {
  TRACE(L"SQLBrowseConnectW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetTypeInfoW(
  SQLHSTMT      StatementHandle,
  SQLSMALLINT   DataType) {
  TRACE(L"SQLGetTypeInfoW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLDataSourcesW(
  SQLHENV          EnvironmentHandle,
  SQLUSMALLINT     Direction,
  SQLWCHAR *       ServerName,
  SQLSMALLINT      BufferLength1,
  SQLSMALLINT *    NameLength1Ptr,
  SQLWCHAR *       Description,
  SQLSMALLINT      BufferLength2,
  SQLSMALLINT *    NameLength2Ptr) {
  TRACE(L"SQLDataSourcesW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetInfoW(
  SQLHDBC         ConnectionHandle,
  SQLUSMALLINT    InfoType,
  SQLPOINTER      InfoValuePtr,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLengthPtr) {
  TRACE(L"SQLGetInfoW\n");
  try {
    Driver::getInstance().getConnection(ConnectionHandle);
    switch (InfoType) {
      case SQL_DRIVER_ODBC_VER:
        Argument::fromStdString(L"03.00", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_DATA_SOURCE_NAME:
        Argument::fromStdString(L"CDAP Datasets", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_SEARCH_PATTERN_ESCAPE:
        Argument::fromStdString(L"", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_GETDATA_EXTENSIONS:
        assert(BufferLength == sizeof(SQLUINTEGER));
        *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = SQL_GD_ANY_COLUMN | SQL_GD_ANY_ORDER | SQL_GD_BOUND;
        return SQL_SUCCESS;
      case SQL_CURSOR_COMMIT_BEHAVIOR:
      case SQL_CURSOR_ROLLBACK_BEHAVIOR:
        assert(BufferLength == sizeof(SQLUSMALLINT));
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_CB_PRESERVE;
        return SQL_SUCCESS;
      case SQL_ACTIVE_STATEMENTS:
        assert(BufferLength == sizeof(SQLUSMALLINT));
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = 0;
        return SQL_SUCCESS;
      case SQL_DATA_SOURCE_READ_ONLY:
        Argument::fromStdString(L"Y", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_DRIVER_NAME:
        Argument::fromStdString(L"CDAP ODBC", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_CORRELATION_NAME:
        assert(BufferLength == sizeof(SQLUSMALLINT));
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_CN_DIFFERENT;
        return SQL_SUCCESS;
      case SQL_NON_NULLABLE_COLUMNS:
        assert(BufferLength == sizeof(SQLUSMALLINT));
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_NNC_NON_NULL;
        return SQL_SUCCESS;
      case SQL_QUALIFIER_NAME_SEPARATOR:
        Argument::fromStdString(L".", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_FILE_USAGE:
        assert(BufferLength == sizeof(SQLUSMALLINT));
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_FILE_CATALOG;
        return SQL_SUCCESS;
      case SQL_QUALIFIER_TERM:
        Argument::fromStdString(L"CATALOG", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_DATABASE_NAME:
        Argument::fromStdString(L"", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
    }

    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
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
  TRACE(L"SQLGetFunctions\n");
  try {
    Driver::getInstance().getConnection(ConnectionHandle);
    if (FunctionId == SQL_API_ODBC3_ALL_FUNCTIONS) {
      Driver::getInstance().setupSupportedFunctions(SupportedPtr);
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

      return SQL_SUCCESS;
    }

    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLSetConnectAttrW(
  SQLHDBC       ConnectionHandle,
  SQLINTEGER    Attribute,
  SQLPOINTER    ValuePtr,
  SQLINTEGER    StringLength) {
  TRACE(L"SQLSetConnectAttrW\n");
  // Attrs not supported 
  // Always report success 
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
  TRACE(L"SQLGetStmtAttrW\n");
  try {
    switch (Attribute) {
      case SQL_ATTR_APP_PARAM_DESC:
      case SQL_ATTR_APP_ROW_DESC:
      case SQL_ATTR_IMP_ROW_DESC:
      case SQL_ATTR_IMP_PARAM_DESC:
        assert(BufferLength == SQL_IS_POINTER);
        *(reinterpret_cast<void**>(ValuePtr)) = nullptr;
        return SQL_SUCCESS;
      case SQL_ATTR_ASYNC_ENABLE:
        assert(BufferLength == sizeof(SQLULEN));
        *(reinterpret_cast<SQLULEN*>(ValuePtr)) = SQL_ASYNC_ENABLE_OFF;
        return SQL_SUCCESS;
    }

    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLPrepareW(
  SQLHSTMT      StatementHandle,
  SQLWCHAR *     StatementText,
  SQLINTEGER    TextLength) {
  TRACE(L"SQLPrepareW\n");
  return SQL_ERROR;
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
  TRACE(L"SQLExecute\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLExecDirectW(
  SQLHSTMT     StatementHandle,
  SQLWCHAR *   StatementText,
  SQLINTEGER   TextLength) {
  TRACE(L"SQLExecDirectW\n");
  return SQL_ERROR;
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
  TRACE(L"SQLNumResultCols\n");
  return SQL_ERROR;
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
  TRACE(L"SQLDescribeColW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLColAttributeW(
  SQLHSTMT        StatementHandle,
  SQLUSMALLINT    ColumnNumber,
  SQLUSMALLINT    FieldIdentifier,
  SQLPOINTER      CharacterAttributePtr,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLengthPtr,
  SQLLEN *        NumericAttributePtr) {
  TRACE(L"SQLColAttributeW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLBindCol(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   ColumnNumber,
  SQLSMALLINT    TargetType,
  SQLPOINTER     TargetValuePtr,
  SQLLEN         BufferLength,
  SQLLEN *       StrLen_or_Ind) {
  TRACE(L"SQLBindCol\n");
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
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
    }

    return SQL_SUCCESS;
  } catch (InvalidHandleException&) {
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLFetch(
  SQLHSTMT     StatementHandle) {
  TRACE(L"SQLFetch\n");
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    statement.fetch();
    return SQL_SUCCESS;
  } catch (InvalidHandleException&) {
    return SQL_INVALID_HANDLE;
  } catch (StillExecutingException&) {
    return SQL_STILL_EXECUTING;
  } catch (NoDataException&) {
    return SQL_NO_DATA;
  } catch (std::exception&) {
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
  TRACE(L"SQLColumnsW\n");
  return SQL_ERROR;
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
  TRACE(L"SQLTablesW\n");
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    auto catalogName = Argument::toStdString(CatalogName, NameLength1);
    auto schemaName = Argument::toStdString(SchemaName, NameLength2);
    auto tableName = Argument::toStdString(TableName, NameLength3);
    auto tableType = Argument::toStdString(TableType, NameLength4);

    if (catalogName == L"%" && schemaName.size() == 0 && tableName.size() == 0) {
      statement.getCatalogs();
      return SQL_SUCCESS;
    }

    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
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
  TRACE(L"SQLSpecialColumnsW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLFreeStmt(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   Option) {
  TRACE(L"SQLFreeStmt\n");
  return SQL_ERROR;
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
  TRACE(L"SQLDisconnect\n");
  try {
    Driver::getInstance().getConnection(ConnectionHandle).close();
    return SQL_SUCCESS;
  } catch (InvalidHandleException&) {
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLFreeHandle(
  SQLSMALLINT   HandleType,
  SQLHANDLE     Handle) {
  TRACE(L"SQLFreeHandle\n");
  if (!Handle) {
    return SQL_INVALID_HANDLE;
  }

  try {
    switch (HandleType) {
      case SQL_HANDLE_ENV:
        Driver::getInstance().freeEnvironment(Handle);
        return SQL_SUCCESS;

      case SQL_HANDLE_DBC:
        Driver::getInstance().freeConnection(Handle);
        return SQL_SUCCESS;

      case SQL_HANDLE_STMT:
        Driver::getInstance().freeStatement(Handle);
        return SQL_SUCCESS;

      case SQL_HANDLE_DESC:
        Driver::getInstance().freeDescriptor(Handle);
        return SQL_SUCCESS;

      default:
        return SQL_ERROR;
    }
  } catch (InvalidHandleException&) {
    return SQL_INVALID_HANDLE;
  } catch (std::exception&) {
    return SQL_ERROR;
  }
}