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
    auto& connection = Driver::getInstance().getConnection(ConnectionHandle);
    std::wstring connectionString;
    switch (DriverCompletion) {
      case SQL_DRIVER_PROMPT:
        // DRIVER
        connectionString = Argument::toStdString(InConnectionString, StringLength1);
        connectionString += L"HOST=localhost;PORT=10000";
        connection.open(connectionString);
        Argument::fromStdString(connectionString, OutConnectionString, BufferLength, StringLength2Ptr);
        TRACE(L"SQLDriverConnectW returns SQL_SUCCESS, OutConnectionString = %s\n", OutConnectionString);
        return SQL_SUCCESS;
      case SQL_DRIVER_COMPLETE:
      case SQL_DRIVER_COMPLETE_REQUIRED:
        // DSN
        connectionString = Argument::toStdString(InConnectionString, StringLength1);
        connection.open(L"HOST=localhost;PORT=10000");
        Argument::fromStdString(connectionString, OutConnectionString, BufferLength, StringLength2Ptr);
        TRACE(L"SQLDriverConnectW returns SQL_SUCCESS, OutConnectionString = %s\n", OutConnectionString);
        return SQL_SUCCESS;
      case SQL_DRIVER_NOPROMPT:
        // DRIVER 2
        connectionString = Argument::toStdString(InConnectionString, StringLength1);
        connection.open(connectionString);
        Argument::fromStdString(connectionString, OutConnectionString, BufferLength, StringLength2Ptr);
        TRACE(L"SQLDriverConnectW returns SQL_SUCCESS, OutConnectionString = %s\n", OutConnectionString);
        return SQL_SUCCESS;
    }

    TRACE(L"SQLDriverConnectW returns SQL_ERROR\n");
    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLDriverConnectW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
    TRACE(L"SQLDriverConnectW returns SQL_ERROR\n");
    return SQL_ERROR;
  }
 }

SQLRETURN SQL_API SQLGetTypeInfoW(
  SQLHSTMT      StatementHandle,
  SQLSMALLINT   DataType) {
  TRACE(L"SQLGetTypeInfoW\n");
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetInfoW(
  SQLHDBC         ConnectionHandle,
  SQLUSMALLINT    InfoType,
  SQLPOINTER      InfoValuePtr,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLengthPtr) {
  TRACE(L"SQLGetInfoW(ConnectionHandle = %X, InfoType = %d)\n", ConnectionHandle, InfoType);
  try {
    Driver::getInstance().getConnection(ConnectionHandle);
    switch (InfoType) {
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
        assert(BufferLength == sizeof(SQLUSMALLINT));
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_CB_PRESERVE;
        TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = SQL_CB_PRESERVE\n");
        return SQL_SUCCESS;
      case SQL_ACTIVE_STATEMENTS:
        assert(BufferLength == sizeof(SQLUSMALLINT));
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = 0;
        TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = 0\n");
        return SQL_SUCCESS;
      case SQL_DATA_SOURCE_READ_ONLY:
        Argument::fromStdString(L"Y", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
        return SQL_SUCCESS;
      case SQL_DRIVER_NAME:
        Argument::fromStdString(L"CDAP ODBC", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
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
      case SQL_QUALIFIER_NAME_SEPARATOR:
        Argument::fromStdString(L".", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
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
      case SQL_DATABASE_NAME:
        Argument::fromStdString(L"", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
        return SQL_SUCCESS;
    }

    TRACE(L"SQLGetInfoW returns SQL_ERROR");
    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLGetInfoW returns SQL_INVALID_HANDLE");
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
    TRACE(L"SQLGetInfoW returns SQL_ERROR");
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
  TRACE(L"SQLSetConnectAttrW(ConnectionHandle = %X, Attribute = %d)\n", ConnectionHandle, Attribute);
  // Attrs not supported 
  // Always report success 
  TRACE(L"SQLSetConnectAttrW returns SQL_SUCCESS");
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
  TRACE(L"SQLGetDiagFieldW\n");
  return SQL_ERROR;
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
  TRACE(L"SQLGetDiagRecW\n");
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
  TRACE(L"SQLSetStmtAttrW\n");
  return SQL_ERROR;
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