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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLConnectA(
  SQLHDBC        ConnectionHandle,
  SQLCHAR *      ServerName,
  SQLSMALLINT    NameLength1,
  SQLCHAR *      UserName,
  SQLSMALLINT    NameLength2,
  SQLCHAR *      Authentication,
  SQLSMALLINT    NameLength3) {
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
  try {
    auto& connection = Driver::getInstance().getConnection(ConnectionHandle);
    std::string connectionString;
    switch (DriverCompletion) {
      case SQL_DRIVER_PROMPT:
        connectionString = Argument::toStdString(InConnectionString, StringLength1);
        connectionString += "HOST=localhost;PORT=10000";
        connection.open(connectionString);
        Argument::fromStdString(connectionString, OutConnectionString, BufferLength, StringLength2Ptr);
        return SQL_SUCCESS;
      case SQL_DRIVER_COMPLETE:
      case SQL_DRIVER_COMPLETE_REQUIRED:
        return SQL_ERROR;
      case SQL_DRIVER_NOPROMPT:
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

SQLRETURN SQL_API SQLDriverConnectA(
  SQLHDBC         ConnectionHandle,
  SQLHWND         WindowHandle,
  SQLCHAR *       InConnectionString,
  SQLSMALLINT     StringLength1,
  SQLCHAR *       OutConnectionString,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLength2Ptr,
  SQLUSMALLINT    DriverCompletion) {
  try {
    auto& connection = Driver::getInstance().getConnection(ConnectionHandle);
    std::string connectionString = Argument::toStdString(InConnectionString, StringLength1);
    connection.open(connectionString);
    return SQL_SUCCESS;
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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetTypeInfoW(
  SQLHSTMT      StatementHandle,
  SQLSMALLINT   DataType) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetTypeInfoA(
  SQLHSTMT      StatementHandle,
  SQLSMALLINT   DataType) {
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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLDataSourcesA(
  SQLHENV          EnvironmentHandle,
  SQLUSMALLINT     Direction,
  SQLCHAR *        ServerName,
  SQLSMALLINT      BufferLength1,
  SQLSMALLINT *    NameLength1Ptr,
  SQLCHAR *        Description,
  SQLSMALLINT      BufferLength2,
  SQLSMALLINT *    NameLength2Ptr) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetInfoW(
  SQLHDBC         ConnectionHandle,
  SQLUSMALLINT    InfoType,
  SQLPOINTER      InfoValuePtr,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLengthPtr) {
  try {
    Driver::getInstance().getConnection(ConnectionHandle);
    switch (InfoType) {
      case SQL_DRIVER_ODBC_VER:
        Argument::fromStdString("03.80", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_DATA_SOURCE_NAME:
      case SQL_SEARCH_PATTERN_ESCAPE:
        Argument::fromStdString("", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_GETDATA_EXTENSIONS:
        *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = 0;
        return SQL_SUCCESS;
      case SQL_CURSOR_COMMIT_BEHAVIOR:
      case SQL_CURSOR_ROLLBACK_BEHAVIOR:
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_CB_DELETE;
        return SQL_SUCCESS;
      case SQL_ACTIVE_STATEMENTS:
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = 100;
        return SQL_SUCCESS;
      case SQL_DATA_SOURCE_READ_ONLY:
        Argument::fromStdString("Y", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_DRIVER_NAME:
        Argument::fromStdString("CDAP ODBC", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_CORRELATION_NAME:
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_CN_ANY;
        return SQL_SUCCESS;
      case SQL_NON_NULLABLE_COLUMNS:
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_NNC_NON_NULL;
        return SQL_SUCCESS;
      case SQL_QUALIFIER_NAME_SEPARATOR:
        Argument::fromStdString(".", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_FILE_USAGE:
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = SQL_FILE_NOT_SUPPORTED;
        return SQL_SUCCESS;
      case SQL_QUALIFIER_TERM:
        Argument::fromStdString("catalog", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        return SQL_SUCCESS;
      case SQL_DATABASE_NAME:
        Argument::fromStdString("", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetFunctions(
  SQLHDBC           ConnectionHandle,
  SQLUSMALLINT      FunctionId,
  SQLUSMALLINT *    SupportedPtr) {
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
  // Attrs not supported 
  // Always report success 
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetConnectAttrA(
  SQLHDBC       ConnectionHandle,
  SQLINTEGER    Attribute,
  SQLPOINTER    ValuePtr,
  SQLINTEGER    StringLength) {
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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetStmtAttrW(
  SQLHSTMT        StatementHandle,
  SQLINTEGER      Attribute,
  SQLPOINTER      ValuePtr,
  SQLINTEGER      BufferLength,
  SQLINTEGER *    StringLengthPtr) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLPrepareW(
  SQLHSTMT      StatementHandle,
  SQLWCHAR *     StatementText,
  SQLINTEGER    TextLength) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLPrepareA(
  SQLHSTMT      StatementHandle,
  SQLCHAR *     StatementText,
  SQLINTEGER    TextLength) {
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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLExecute(
  SQLHSTMT     StatementHandle) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLExecDirectW(
  SQLHSTMT     StatementHandle,
  SQLWCHAR *   StatementText,
  SQLINTEGER   TextLength) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLExecDirectA(
  SQLHSTMT     StatementHandle,
  SQLCHAR *    StatementText,
  SQLINTEGER   TextLength) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLNumParams(
  SQLHSTMT        StatementHandle,
  SQLSMALLINT *   ParameterCountPtr) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLParamData(
  SQLHSTMT       StatementHandle,
  SQLPOINTER *   ValuePtrPtr) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLNumResultCols(
  SQLHSTMT        StatementHandle,
  SQLSMALLINT *   ColumnCountPtr) {
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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLDescribeColA(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   ColumnNumber,
  SQLCHAR *      ColumnName,
  SQLSMALLINT    BufferLength,
  SQLSMALLINT *  NameLengthPtr,
  SQLSMALLINT *  DataTypePtr,
  SQLULEN *      ColumnSizePtr,
  SQLSMALLINT *  DecimalDigitsPtr,
  SQLSMALLINT *  NullablePtr) {
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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLColAttributeA(
  SQLHSTMT        StatementHandle,
  SQLUSMALLINT    ColumnNumber,
  SQLUSMALLINT    FieldIdentifier,
  SQLPOINTER      CharacterAttributePtr,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLengthPtr,
  SQLLEN *        NumericAttributePtr) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLBindCol(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   ColumnNumber,
  SQLSMALLINT    TargetType,
  SQLPOINTER     TargetValuePtr,
  SQLLEN         BufferLength,
  SQLLEN *       StrLen_or_Ind) {
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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetPos(
  SQLHSTMT        StatementHandle,
  SQLSETPOSIROW   RowNumber,
  SQLUSMALLINT    Operation,
  SQLUSMALLINT    LockType) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLMoreResults(
  SQLHSTMT     StatementHandle) {
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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLColumnsA(
  SQLHSTMT       StatementHandle,
  SQLCHAR *      CatalogName,
  SQLSMALLINT    NameLength1,
  SQLCHAR *      SchemaName,
  SQLSMALLINT    NameLength2,
  SQLCHAR *      TableName,
  SQLSMALLINT    NameLength3,
  SQLCHAR *      ColumnName,
  SQLSMALLINT    NameLength4) {
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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLPrimaryKeysA(
  SQLHSTMT       StatementHandle,
  SQLCHAR *     CatalogName,
  SQLSMALLINT    NameLength1,
  SQLCHAR *     SchemaName,
  SQLSMALLINT    NameLength2,
  SQLCHAR *     TableName,
  SQLSMALLINT    NameLength3) {
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
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    auto catalogName = Argument::toStdString(CatalogName, NameLength1);
    auto schemaName = Argument::toStdString(SchemaName, NameLength2);
    auto tableName = Argument::toStdString(TableName, NameLength3);
    auto tableType = Argument::toStdString(TableType, NameLength4);

    if (catalogName == SQL_ALL_CATALOGS && schemaName.size() == 0 && tableName.size() == 0) {
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

SQLRETURN SQL_API SQLTablesA(
  SQLHSTMT       StatementHandle,
  SQLCHAR *      CatalogName,
  SQLSMALLINT    NameLength1,
  SQLCHAR *      SchemaName,
  SQLSMALLINT    NameLength2,
  SQLCHAR *      TableName,
  SQLSMALLINT    NameLength3,
  SQLCHAR *      TableType,
  SQLSMALLINT    NameLength4) {

  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    auto catalogName = Argument::toStdString(CatalogName, NameLength1);
    auto schemaName = Argument::toStdString(SchemaName, NameLength2);
    auto tableName = Argument::toStdString(TableName, NameLength3);
    auto tableType = Argument::toStdString(TableType, NameLength4);

    if (catalogName == SQL_ALL_CATALOGS && schemaName.size() == 0 && tableName.size() == 0) {
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
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLFreeStmt(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   Option) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLCloseCursor(
  SQLHSTMT     StatementHandle) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLCancel(
  SQLHSTMT     StatementHandle) {
  return SQL_ERROR;
}

SQLRETURN SQL_API SQLDisconnect(
  SQLHDBC     ConnectionHandle) {

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