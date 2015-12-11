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
#include "Driver.h"

using namespace Cask::CdapOdbc;

SQLRETURN SQLAllocHandle(
	SQLSMALLINT   HandleType,
	SQLHANDLE     InputHandle,
	SQLHANDLE *   OutputHandlePtr)
{
	try
	{
		// OutputHandlePtr must be a valid pointer
		if (!OutputHandlePtr)
		{
			return SQL_ERROR;
		}

		*OutputHandlePtr = SQL_NULL_HANDLE;

		switch (HandleType)
		{
			case SQL_HANDLE_ENV:
			{
				// InputHandle must be NULL
				if (InputHandle != SQL_NULL_HANDLE)
				{
					return SQL_ERROR;
				}

				// Allocate new environment
				*OutputHandlePtr = Driver::getInstance().allocEnvironment();
				return SQL_SUCCESS;
			}

			case SQL_HANDLE_DBC:
			{
				// InputHandle must not be NULL
				if (InputHandle == SQL_NULL_HANDLE)
				{
					return SQL_ERROR;
				}

				break;
			}

			case SQL_HANDLE_STMT:
			{
				// InputHandle must not be NULL
				if (InputHandle == SQL_NULL_HANDLE)
				{
					return SQL_ERROR;
				}

				break;
			}

			case SQL_HANDLE_DESC:
			{
				// InputHandle must not be NULL
				if (InputHandle == SQL_NULL_HANDLE)
				{
					return SQL_ERROR;
				}

				break;
			}
		}

		return SQL_ERROR;
	}
	catch (std::invalid_argument&)
	{
		return SQL_INVALID_HANDLE;
	}
	catch (std::exception&)
	{
		return SQL_ERROR;
	}
}

SQLRETURN SQLConnectW(
	SQLHDBC        ConnectionHandle,
	SQLWCHAR *     ServerName,
	SQLSMALLINT    NameLength1,
	SQLWCHAR *     UserName,
	SQLSMALLINT    NameLength2,
	SQLWCHAR *     Authentication,
	SQLSMALLINT    NameLength3)
{
	return SQL_ERROR;
}

SQLRETURN SQLConnectA(
	SQLHDBC        ConnectionHandle,
	SQLCHAR *      ServerName,
	SQLSMALLINT    NameLength1,
	SQLCHAR *      UserName,
	SQLSMALLINT    NameLength2,
	SQLCHAR *      Authentication,
	SQLSMALLINT    NameLength3)
{
	return SQL_ERROR;
}

SQLRETURN SQLGetTypeInfoW(
	SQLHSTMT      StatementHandle,
	SQLSMALLINT   DataType)
{
	return SQL_ERROR;
}

SQLRETURN SQLGetTypeInfoA(
	SQLHSTMT      StatementHandle,
	SQLSMALLINT   DataType)
{
	return SQL_ERROR;
}

SQLRETURN SQLDataSourcesW(
	SQLHENV          EnvironmentHandle,
	SQLUSMALLINT     Direction,
	SQLWCHAR *       ServerName,
	SQLSMALLINT      BufferLength1,
	SQLSMALLINT *    NameLength1Ptr,
	SQLWCHAR *       Description,
	SQLSMALLINT      BufferLength2,
	SQLSMALLINT *    NameLength2Ptr)
{
	return SQL_ERROR;
}

SQLRETURN SQLDataSourcesA(
	SQLHENV          EnvironmentHandle,
	SQLUSMALLINT     Direction,
	SQLCHAR *        ServerName,
	SQLSMALLINT      BufferLength1,
	SQLSMALLINT *    NameLength1Ptr,
	SQLCHAR *        Description,
	SQLSMALLINT      BufferLength2,
	SQLSMALLINT *    NameLength2Ptr)
{
	return SQL_ERROR;
}

SQLRETURN SQLPrepareW(
	SQLHSTMT      StatementHandle,
	SQLWCHAR *     StatementText,
	SQLINTEGER    TextLength)
{
	return SQL_ERROR;
}

SQLRETURN SQLPrepareA(
	SQLHSTMT      StatementHandle,
	SQLCHAR *     StatementText,
	SQLINTEGER    TextLength)
{
	return SQL_ERROR;
}

SQLRETURN SQLBindParameter(
	SQLHSTMT        StatementHandle,
	SQLUSMALLINT    ParameterNumber,
	SQLSMALLINT     InputOutputType,
	SQLSMALLINT     ValueType,
	SQLSMALLINT     ParameterType,
	SQLULEN         ColumnSize,
	SQLSMALLINT     DecimalDigits,
	SQLPOINTER      ParameterValuePtr,
	SQLLEN          BufferLength,
	SQLLEN *        StrLen_or_IndPtr)
{
	return SQL_ERROR;
}

SQLRETURN SQLExecute(
	SQLHSTMT     StatementHandle)
{
	return SQL_ERROR;
}

SQLRETURN SQLExecDirectW(
	SQLHSTMT     StatementHandle,
	SQLWCHAR *   StatementText,
	SQLINTEGER   TextLength)
{
	return SQL_ERROR;
}

SQLRETURN SQLExecDirectA(
	SQLHSTMT     StatementHandle,
	SQLCHAR *    StatementText,
	SQLINTEGER   TextLength)
{
	return SQL_ERROR;
}

SQLRETURN SQLNumParams(
	SQLHSTMT        StatementHandle,
	SQLSMALLINT *   ParameterCountPtr)
{
	return SQL_ERROR;
}

SQLRETURN SQLParamData(
	SQLHSTMT       StatementHandle,
	SQLPOINTER *   ValuePtrPtr)
{
	return SQL_ERROR;
}

SQLRETURN SQLNumResultCols(
	SQLHSTMT        StatementHandle,
	SQLSMALLINT *   ColumnCountPtr)
{
	return SQL_ERROR;
}

SQLRETURN SQLDescribeColW(
	SQLHSTMT       StatementHandle,
	SQLUSMALLINT   ColumnNumber,
	SQLWCHAR *     ColumnName,
	SQLSMALLINT    BufferLength,
	SQLSMALLINT *  NameLengthPtr,
	SQLSMALLINT *  DataTypePtr,
	SQLULEN *      ColumnSizePtr,
	SQLSMALLINT *  DecimalDigitsPtr,
	SQLSMALLINT *  NullablePtr)
{
	return SQL_ERROR;
}

SQLRETURN SQLDescribeColA(
	SQLHSTMT       StatementHandle,
	SQLUSMALLINT   ColumnNumber,
	SQLCHAR *      ColumnName,
	SQLSMALLINT    BufferLength,
	SQLSMALLINT *  NameLengthPtr,
	SQLSMALLINT *  DataTypePtr,
	SQLULEN *      ColumnSizePtr,
	SQLSMALLINT *  DecimalDigitsPtr,
	SQLSMALLINT *  NullablePtr)
{
	return SQL_ERROR;
}

SQLRETURN SQLColAttributeW(
	SQLHSTMT        StatementHandle,
	SQLUSMALLINT    ColumnNumber,
	SQLUSMALLINT    FieldIdentifier,
	SQLPOINTER      CharacterAttributePtr,
	SQLSMALLINT     BufferLength,
	SQLSMALLINT *   StringLengthPtr,
	SQLLEN *        NumericAttributePtr)
{
	return SQL_ERROR;
}

SQLRETURN SQLColAttributeA(
	SQLHSTMT        StatementHandle,
	SQLUSMALLINT    ColumnNumber,
	SQLUSMALLINT    FieldIdentifier,
	SQLPOINTER      CharacterAttributePtr,
	SQLSMALLINT     BufferLength,
	SQLSMALLINT *   StringLengthPtr,
	SQLLEN *        NumericAttributePtr)
{
	return SQL_ERROR;
}

SQLRETURN SQLBindCol(
	SQLHSTMT       StatementHandle,
	SQLUSMALLINT   ColumnNumber,
	SQLSMALLINT    TargetType,
	SQLPOINTER     TargetValuePtr,
	SQLLEN         BufferLength,
	SQLLEN *       StrLen_or_Ind)
{
	return SQL_ERROR;
}

SQLRETURN SQLFetch(
	SQLHSTMT     StatementHandle)
{
	return SQL_ERROR;
}

SQLRETURN SQLGetData(
	SQLHSTMT       StatementHandle,
	SQLUSMALLINT   Col_or_Param_Num,
	SQLSMALLINT    TargetType,
	SQLPOINTER     TargetValuePtr,
	SQLLEN         BufferLength,
	SQLLEN *       StrLen_or_IndPtr)
{
	return SQL_ERROR;
}

SQLRETURN SQLSetPos(
	SQLHSTMT        StatementHandle,
	SQLSETPOSIROW   RowNumber,
	SQLUSMALLINT    Operation,
	SQLUSMALLINT    LockType)
{
	return SQL_ERROR;
}

SQLRETURN SQLMoreResults(
	SQLHSTMT     StatementHandle)
{
	return SQL_ERROR;
}

SQLRETURN SQLColumnsW(
	SQLHSTMT       StatementHandle,
	SQLWCHAR *      CatalogName,
	SQLSMALLINT    NameLength1,
	SQLWCHAR *      SchemaName,
	SQLSMALLINT    NameLength2,
	SQLWCHAR *      TableName,
	SQLSMALLINT    NameLength3,
	SQLWCHAR *      ColumnName,
	SQLSMALLINT    NameLength4)
{
	return SQL_ERROR;
}

SQLRETURN SQLColumnsA(
	SQLHSTMT       StatementHandle,
	SQLCHAR *      CatalogName,
	SQLSMALLINT    NameLength1,
	SQLCHAR *      SchemaName,
	SQLSMALLINT    NameLength2,
	SQLCHAR *      TableName,
	SQLSMALLINT    NameLength3,
	SQLCHAR *      ColumnName,
	SQLSMALLINT    NameLength4)
{
	return SQL_ERROR;
}

SQLRETURN SQLPrimaryKeysW(
	SQLHSTMT       StatementHandle,
	SQLWCHAR *     CatalogName,
	SQLSMALLINT    NameLength1,
	SQLWCHAR *     SchemaName,
	SQLSMALLINT    NameLength2,
	SQLWCHAR *     TableName,
	SQLSMALLINT    NameLength3)
{
	return SQL_ERROR;
}

SQLRETURN SQLPrimaryKeysA(
	SQLHSTMT       StatementHandle,
	SQLCHAR *     CatalogName,
	SQLSMALLINT    NameLength1,
	SQLCHAR *     SchemaName,
	SQLSMALLINT    NameLength2,
	SQLCHAR *     TableName,
	SQLSMALLINT    NameLength3)
{
	return SQL_ERROR;
}

SQLRETURN SQLTablesW(
	SQLHSTMT       StatementHandle,
	SQLWCHAR *     CatalogName,
	SQLSMALLINT    NameLength1,
	SQLWCHAR *     SchemaName,
	SQLSMALLINT    NameLength2,
	SQLWCHAR *     TableName,
	SQLSMALLINT    NameLength3,
	SQLWCHAR *     TableType,
	SQLSMALLINT    NameLength4)
{
	return SQL_ERROR;
}

SQLRETURN SQLTablesA(
	SQLHSTMT       StatementHandle,
	SQLCHAR *      CatalogName,
	SQLSMALLINT    NameLength1,
	SQLCHAR *      SchemaName,
	SQLSMALLINT    NameLength2,
	SQLCHAR *      TableName,
	SQLSMALLINT    NameLength3,
	SQLCHAR *      TableType,
	SQLSMALLINT    NameLength4)
{
	return SQL_ERROR;
}

SQLRETURN SQLFreeStmt(
	SQLHSTMT       StatementHandle,
	SQLUSMALLINT   Option)
{
	return SQL_ERROR;
}

SQLRETURN SQLCloseCursor(
	SQLHSTMT     StatementHandle)
{
	return SQL_ERROR;
}

SQLRETURN SQLCancel(
	SQLHSTMT     StatementHandle)
{
	return SQL_ERROR;
}

SQLRETURN SQLDisconnect(
	SQLHDBC     ConnectionHandle)
{
	return SQL_ERROR;
}

SQLRETURN SQLFreeHandle(
	SQLSMALLINT   HandleType,
	SQLHANDLE     Handle)
{
	if (!Handle)
	{
		return SQL_INVALID_HANDLE;
	}

	try
	{
		switch (HandleType)
		{
			case SQL_HANDLE_ENV:
			{
				Driver::getInstance().freeEnvironment(Handle);
				return SQL_SUCCESS;
			}
		}

		return SQL_ERROR;
	}
	catch (std::invalid_argument&)
	{
		return SQL_INVALID_HANDLE;
	}
	catch (std::exception&)
	{
		return SQL_ERROR;
	}
}