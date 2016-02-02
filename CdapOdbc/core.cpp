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
#include "ConnectionDialog.h"
#include "Encoding.h"

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
    std::unique_ptr<std::wstring> connectionString;
    std::wstring newConnectionString;
    std::unique_ptr<ConnectionDialog> dialog;

    switch (DriverCompletion) {
      case SQL_DRIVER_PROMPT:
        // DRIVER
        connectionString = Argument::toStdString(InConnectionString, StringLength1);
        dialog = std::make_unique<ConnectionDialog>(WindowHandle);
        dialog->setParams(ConnectionParams(*connectionString));
        if (dialog->show()) {
          newConnectionString = dialog->getParams().getFullConnectionString();
          connection.open(newConnectionString);
          Argument::fromStdString(newConnectionString, OutConnectionString, BufferLength, StringLength2Ptr);
          TRACE(L"SQLDriverConnectW returns SQL_SUCCESS, OutConnectionString = %s\n", OutConnectionString);
          return SQL_SUCCESS;
        }

        TRACE(L"SQLDriverConnectW returns SQL_ERROR\n");
        return SQL_ERROR;
      case SQL_DRIVER_COMPLETE:
      case SQL_DRIVER_COMPLETE_REQUIRED:
        // DSN
        connectionString = Argument::toStdString(InConnectionString, StringLength1);
        if (connectionString->find(L"DSN") != std::wstring::npos) {
          dialog = std::make_unique<ConnectionDialog>(WindowHandle);
          dialog->setParams(ConnectionParams(*connectionString));
          if (!dialog->show()) {
            TRACE(L"SQLDriverConnectW returns SQL_ERROR\n");
            return SQL_ERROR;
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
  TRACE(L"SQLGetTypeInfoW(StatementHandle = %X, DataType = %d)\n", StatementHandle, DataType);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    statement.getDataTypes();
    TRACE(L"SQLGetTypeInfoW returns SQL_SUCCESS\n");
    return SQL_SUCCESS;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLGetTypeInfoW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
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
  TRACE(L"SQLGetInfoW(ConnectionHandle = %X, InfoType = %d)\n", ConnectionHandle, InfoType);
  try {
    if (StringLengthPtr) {
      *StringLengthPtr = 0;
    }

    Driver::getInstance().getConnection(ConnectionHandle);

    if (InfoValuePtr == nullptr) {
      TRACE(L"SQLGetInfoW returns SQL_ERROR\n");
      return SQL_ERROR;
    }

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
      case SQL_QUALIFIER_NAME_SEPARATOR:
        Argument::fromStdString(L".", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
        TRACE(L"SQLGetInfoW returns SQL_SUCCESS, InfoValuePtr = %s\n", static_cast<SQLWCHAR*>(InfoValuePtr));
        return SQL_SUCCESS;
      case SQL_IDENTIFIER_QUOTE_CHAR:
        Argument::fromStdString(L"\"", static_cast<SQLWCHAR*>(InfoValuePtr), BufferLength, StringLengthPtr);
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
      case SQL_MAX_OWNER_NAME_LEN:
        assert(BufferLength == sizeof(SQLUSMALLINT));
        *(reinterpret_cast<SQLUSMALLINT*>(InfoValuePtr)) = 128;
        TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = 128\n");
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
        assert(BufferLength == sizeof(SQLUINTEGER));
        *(reinterpret_cast<SQLUINTEGER*>(InfoValuePtr)) = 0;
        TRACE(L"SQLGetInfoW returns SQL_SUCCESS, *InfoValuePtr = 0\n");
        return SQL_SUCCESS;
    }

    TRACE(L"SQLGetInfoW returns SQL_ERROR\n");
    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLGetInfoW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
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
    if (SupportedPtr) {
      Driver::getInstance().getConnection(ConnectionHandle);
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
      }
    }

    TRACE(L"SQLGetFunctions returns SQL_ERROR\n");
    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLGetFunctions returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
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
    Driver::getInstance().getStatement(StatementHandle);

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
    }

    TRACE(L"SQLGetStmtAttrW returns SQL_ERROR\n");
    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLGetStmtAttrW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
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
    auto sql = Argument::toStdString(StatementText, static_cast<SQLSMALLINT>(TextLength));
    if (sql) {
      statement.prepare(*sql);
      TRACE(L"SQLPrepareW returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    }

    TRACE(L"SQLPrepareW returns SQL_ERROR\n");
    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLPrepareW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
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
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    statement.execute();
    TRACE(L"SQLExecute returns SQL_SUCCESS\n");
    return SQL_SUCCESS;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLExecute returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (StillExecutingException&) {
    TRACE(L"SQLExecute returns SQL_STILL_EXECUTING\n");
    return SQL_STILL_EXECUTING;
  } catch (NoDataException&) {
    TRACE(L"SQLExecute returns SQL_NO_DATA\n");
    return SQL_NO_DATA;
  } catch (std::exception) {
    TRACE(L"SQLExecute returns SQL_ERROR\n");
    return SQL_ERROR;
  }
}

SQLRETURN SQL_API SQLExecDirectW(
  SQLHSTMT     StatementHandle,
  SQLWCHAR *   StatementText,
  SQLINTEGER   TextLength) {
  TRACE(L"SQLExecDirectW(StatementHandle = %X, StatementText = %s)\n", StatementHandle, StatementText);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    auto query = Argument::toStdString(StatementText, static_cast<SQLSMALLINT>(TextLength));
    if (query) {
      statement.executeDirect(*query);
      TRACE(L"SQLExecDirectW returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    }

    TRACE(L"SQLExecDirectW returns SQL_ERROR\n");
    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLExecDirectW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (StillExecutingException&) {
    TRACE(L"SQLExecDirectW returns SQL_STILL_EXECUTING\n");
    return SQL_STILL_EXECUTING;
  } catch (NoDataException&) {
    TRACE(L"SQLExecDirectW returns SQL_NO_DATA\n");
    return SQL_NO_DATA;
  } catch (std::exception) {
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
    if (!ColumnCountPtr) {
      TRACE(L"SQLNumResultCols returns SQL_ERROR\n");
      return SQL_ERROR;
    }

    *ColumnCountPtr = statement.getColumnCount();
    TRACE(L"SQLNumResultCols returns SQL_SUCCESS\n");
    return SQL_SUCCESS;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLNumResultCols returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
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
  } catch (InvalidHandleException&) {
    TRACE(L"SQLDescribeColW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
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
    auto& columnInfo = statement.getColumnInfo(ColumnNumber);

    switch (FieldIdentifier) {
      case SQL_COLUMN_DISPLAY_SIZE:
        if (NumericAttributePtr) {
          *NumericAttributePtr = columnInfo->getDataType().getDisplaySize();
        }

        TRACE(L"SQLColAttributeW returns SQL_SUCCESS\n");
        return SQL_SUCCESS;
    }
    
    TRACE(L"SQLColAttributeW returns SQL_ERROR\n");
    return SQL_ERROR;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLColAttributeW returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
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
    BufferLength);
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

    TRACE(L"SQLBindCol returns SQL_SUCCESS\n");
    return SQL_SUCCESS;
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
    statement.fetch();
    TRACE(L"SQLFetch returns SQL_SUCCESS\n");
    return SQL_SUCCESS;
  } catch (StillExecutingException&) {
    TRACE(L"SQLFetch returns SQL_STILL_EXECUTING\n");
    return SQL_STILL_EXECUTING;
  } catch (NoDataException&) {
    TRACE(L"SQLFetch returns SQL_NO_DATA\n");
    return SQL_NO_DATA;
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
    ColumnName);
  try {
    auto& statement = Driver::getInstance().getStatement(StatementHandle);
    auto streamName = Argument::toStdString(TableName, NameLength3);
    if (streamName) {
      statement.getColumns(*streamName);
      TRACE(L"SQLColumnsW returns SQL_SUCCESS\n");
      return SQL_SUCCESS;
    }

    TRACE(L"SQLColumnsW returns SQL_ERROR\n");
    return SQL_ERROR;
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

    TRACE(L"SQLTablesW returns SQL_ERROR\n");
    return SQL_ERROR;
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
    statement.getSpecialColumns();
    TRACE(L"SQLSpecialColumnsW returns SQL_SUCCESS\n");
    return SQL_SUCCESS;
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
    }

    TRACE(L"SQLFreeStmt returns SQL_ERROR\n");
    return SQL_ERROR;
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
    Driver::getInstance().getConnection(ConnectionHandle).close();
    TRACE(L"SQLDisconnect returns SQL_SUCCESS\n");
    return SQL_SUCCESS;
  } catch (InvalidHandleException&) {
    TRACE(L"SQLDisconnect returns SQL_INVALID_HANDLE\n");
    return SQL_INVALID_HANDLE;
  } catch (std::exception) {
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

BOOL INSTAPI ConfigDSN(
  HWND     hwndParent,
  WORD     fRequest,
  LPCSTR   lpszDriver,
  LPCSTR   lpszAttributes) {
  TRACE(L"ConfigDSN(hwndParent = %X, fRequest = %d)\n", hwndParent, fRequest);
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
  } catch (std::exception&) {
    TRACE(L"ConfigDSN returns FALSE\n");
    return FALSE;
  }
}

BOOL INSTAPI ConfigDriver(
  HWND    hwndParent,
  WORD    fRequest,
  LPCSTR  lpszDriver,
  LPCSTR  lpszArgs,
  LPSTR   lpszMsg,
  WORD    cbMsgMax,
  WORD *  pcbMsgOut) {
  TRACE(
    L"ConfigDriver(hwndParent = %X, fRequest = %d, lpszDriver = %s, lpszArgs = %s)\n", 
    hwndParent, 
    fRequest, 
    lpszDriver,
    lpszArgs);
  TRACE(L"ConfigDSN returns TRUE\n");
  return TRUE;
}