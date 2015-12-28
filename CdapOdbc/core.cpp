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
  SQLCHAR *       InConnectionString,
  SQLSMALLINT     StringLength1,
  SQLCHAR *       OutConnectionString,
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
  SQLCHAR *       DriverDescription,
  SQLSMALLINT     BufferLength1,
  SQLSMALLINT *   DescriptionLengthPtr,
  SQLCHAR *       DriverAttributes,
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
      // ANSI
      SupportedPtr[SQL_API_SQLALLOCHANDLE] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLGETDESCFIELD
      SupportedPtr[SQL_API_SQLBINDCOL] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLGETDESCREC
      SupportedPtr[SQL_API_SQLCANCEL] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLGETDIAGFIELD
      //SupportedPtr[SQL_API_SQLCLOSECURSOR
      //SupportedPtr[SQL_API_SQLGETDIAGREC
      SupportedPtr[SQL_API_SQLCOLATTRIBUTE] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLGETENVATTR
      //SupportedPtr[SQL_API_SQLCONNECT
      SupportedPtr[SQL_API_SQLGETFUNCTIONS] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLCOPYDESC
      SupportedPtr[SQL_API_SQLGETINFO] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLDATASOURCES
      //SupportedPtr[SQL_API_SQLGETSTMTATTR
      SupportedPtr[SQL_API_SQLDESCRIBECOL] = SQL_TRUE;
      SupportedPtr[SQL_API_SQLGETTYPEINFO] = SQL_TRUE;
      SupportedPtr[SQL_API_SQLDISCONNECT] = SQL_TRUE;
      SupportedPtr[SQL_API_SQLNUMRESULTCOLS] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLDRIVERS] = SQL_TRUE;
      SupportedPtr[SQL_API_SQLPARAMDATA] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLENDTRAN
      SupportedPtr[SQL_API_SQLPREPARE] = SQL_TRUE;
      SupportedPtr[SQL_API_SQLEXECDIRECT] = SQL_TRUE;
      SupportedPtr[SQL_API_SQLPUTDATA] = SQL_TRUE;
      SupportedPtr[SQL_API_SQLEXECUTE] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLROWCOUNT
      SupportedPtr[SQL_API_SQLFETCH] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLSETCONNECTATTR
      //SupportedPtr[SQL_API_SQLFETCHSCROLL
      //SupportedPtr[SQL_API_SQLSETCURSORNAME
      SupportedPtr[SQL_API_SQLFREEHANDLE] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLSETDESCFIELD
      SupportedPtr[SQL_API_SQLFREESTMT] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLSETDESCREC
      //SupportedPtr[SQL_API_SQLGETCONNECTATTR
      //SupportedPtr[SQL_API_SQLSETENVATTR
      //SupportedPtr[SQL_API_SQLGETCURSORNAME
      //SupportedPtr[SQL_API_SQLSETSTMTATTR
      SupportedPtr[SQL_API_SQLGETDATA] = SQL_TRUE;

      // Open Group
      SupportedPtr[SQL_API_SQLCOLUMNS] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLSTATISTICS] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLSPECIALCOLUMNS
      SupportedPtr[SQL_API_SQLTABLES] = SQL_TRUE;

      // ODBC
      SupportedPtr[SQL_API_SQLBINDPARAMETER] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLNATIVESQL
      //SupportedPtr[SQL_API_SQLBROWSECONNECT
      SupportedPtr[SQL_API_SQLNUMPARAMS] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLBULKOPERATIONS[1]
      //SupportedPtr[SQL_API_SQLPRIMARYKEYS
      //SupportedPtr[SQL_API_SQLCOLUMNPRIVILEGES
      //SupportedPtr[SQL_API_SQLPROCEDURECOLUMNS
      //SupportedPtr[SQL_API_SQLDESCRIBEPARAM
      //SupportedPtr[SQL_API_SQLPROCEDURES
      SupportedPtr[SQL_API_SQLDRIVERCONNECT] = SQL_TRUE;
      //SupportedPtr[SQL_API_SQLSETPOS
      //SupportedPtr[SQL_API_SQLFOREIGNKEYS
      //SupportedPtr[SQL_API_SQLTABLEPRIVILEGES
      SupportedPtr[SQL_API_SQLMORERESULTS] = SQL_TRUE;
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