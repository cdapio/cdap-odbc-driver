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
#include "Driver.h"
#include "Environment.h"
#include "Connection.h"
#include "Statement.h"
#include "Descriptor.h"
#include "InvalidHandleException.h"
#include "DataSourceDialog.h"
#include "Argument.h"

#define ODBC_INI L"ODBC.INI"

using namespace Cask::CdapOdbc;

namespace {
  void setFunction(SQLUSMALLINT* bitset, SQLUSMALLINT functionId) {
    bitset[(functionId) >> 4] |= (1 << ((functionId) & 0xF));
  }
}

std::unique_ptr<Driver> Cask::CdapOdbc::Driver::instance(std::make_unique<Driver>());
std::atomic_int Cask::CdapOdbc::Driver::lastHandleIndex = 100;

SQLHANDLE Cask::CdapOdbc::Driver::generateNewHandle() {
  char* ptr = nullptr;
  return static_cast<SQLHANDLE>(ptr + lastHandleIndex.fetch_add(1));
}

Environment* Cask::CdapOdbc::Driver::findEnvironment(SQLHENV env) {
  auto it = this->environments.find(env);
  if (it != this->environments.end()) {
    return it->second.get();
  }

  throw InvalidHandleException("env", env);
}

Connection* Cask::CdapOdbc::Driver::findConnection(SQLHDBC dbc) {
  auto it = this->connections.find(dbc);
  if (it != this->connections.end()) {
    return it->second.get();
  }

  throw InvalidHandleException("dbc", dbc);
}

void Cask::CdapOdbc::Driver::freeConnections(const Environment& env) {
  auto it = this->connections.begin();
  while (it != this->connections.end()) {
    if (it->second->getEnvironment() == &env) {
      this->freeDescriptors(*it->second);
      this->freeStatements(*it->second);
      it = this->connections.erase(it);
    } else {
      ++it;
    }
  }
}

void Cask::CdapOdbc::Driver::freeStatements(const Connection& dbc) {
  auto it = this->statements.begin();
  while (it != this->statements.end()) {
    if (it->second->getConnection() == &dbc) {
      it = this->statements.erase(it);
    } else {
      ++it;
    }
  }
}

void Cask::CdapOdbc::Driver::freeDescriptors(const Connection& dbc) {
  auto it = this->descriptors.begin();
  while (it != this->descriptors.end()) {
    if (it->second->getConnection() == &dbc) {
      it = this->descriptors.erase(it);
    } else {
      ++it;
    }
  }
}

void Cask::CdapOdbc::Driver::initializeDataTypes() {
  this->dataTypes.insert({ L"boolean", DataType(L"boolean", SQL_BIT, 0, 1) });
  this->dataTypes.insert({ L"binary", DataType(L"binary", SQL_BINARY, std::numeric_limits<std::int32_t>::max(), std::numeric_limits<std::int32_t>::max()) });
  this->dataTypes.insert({ L"double", DataType(L"double", SQL_DOUBLE, 0, 24) });
  this->dataTypes.insert({ L"float", DataType(L"float", SQL_REAL, 0, 12) });
  this->dataTypes.insert({ L"int", DataType(L"int", SQL_INTEGER, 0, 11) });
  this->dataTypes.insert({ L"bigint", DataType(L"bigint", SQL_BIGINT, 0, 20) });
  this->dataTypes.insert({ L"string", DataType(L"string", SQL_CHAR, std::numeric_limits<std::int32_t>::max(), std::numeric_limits<std::int32_t>::max()) });
  this->dataTypes.insert({ L"map<string,string>", DataType(L"string", SQL_CHAR, std::numeric_limits<std::int32_t>::max(), std::numeric_limits<std::int32_t>::max()) });
}

Cask::CdapOdbc::Driver::Driver() {
  this->initializeDataTypes();
}

Cask::CdapOdbc::Driver::~Driver() {
  TRACE(L"Destructing Driver object...\n");
}

Driver& Cask::CdapOdbc::Driver::getInstance() {
  return *instance;
}

Environment& Cask::CdapOdbc::Driver::getEnvironment(SQLHENV env) {
  std::lock_guard<std::mutex> lock(this->mutex);
  return *this->findEnvironment(env);
}

Connection& Cask::CdapOdbc::Driver::getConnection(SQLHDBC dbc) {
  std::lock_guard<std::mutex> lock(this->mutex);
  return *this->findConnection(dbc);
}

Statement& Cask::CdapOdbc::Driver::getStatement(SQLHSTMT stmt) {
  std::lock_guard<std::mutex> lock(this->mutex);
  auto it = this->statements.find(stmt);
  if (it != this->statements.end()) {
    return *(it->second);
  }

  throw InvalidHandleException("stmt", stmt);
}

Descriptor& Cask::CdapOdbc::Driver::getDescriptor(SQLHDESC desc) {
  std::lock_guard<std::mutex> lock(this->mutex);
  auto it = this->descriptors.find(desc);
  if (it != this->descriptors.end()) {
    return *(it->second);
  }

  throw InvalidHandleException("desc", desc);
}

SQLHENV Cask::CdapOdbc::Driver::allocEnvironment() {
  SQLHENV env = generateNewHandle();
  std::lock_guard<std::mutex> lock(this->mutex);
  this->environments.emplace(env, std::make_unique<Environment>(this, env));
  return env;
}

SQLHDBC Cask::CdapOdbc::Driver::allocConnection(SQLHENV env) {
  SQLHDBC dbc = generateNewHandle();
  std::lock_guard<std::mutex> lock(this->mutex);
  Environment* environment = this->findEnvironment(env);
  this->connections.emplace(dbc, std::make_unique<Connection>(environment, dbc));
  return dbc;
}

SQLHSTMT Cask::CdapOdbc::Driver::allocStatement(SQLHDBC dbc) {
  SQLHSTMT stmt = generateNewHandle();
  std::lock_guard<std::mutex> lock(this->mutex);
  Connection* connection = this->findConnection(dbc);
  this->statements.emplace(stmt, std::make_unique<Statement>(connection, stmt));
  return stmt;
}

SQLHDESC Cask::CdapOdbc::Driver::allocDescriptor(SQLHDBC dbc) {
  SQLHDESC desc = generateNewHandle();
  std::lock_guard<std::mutex> lock(this->mutex);
  Connection* connection = this->findConnection(dbc);
  this->descriptors.emplace(desc, std::make_unique<Descriptor>(connection, desc));
  return desc;
}

void Cask::CdapOdbc::Driver::freeEnvironment(SQLHENV env) {
  std::lock_guard<std::mutex> lock(this->mutex);

  auto it = this->environments.find(env);
  if (it == this->environments.end()) {
    throw InvalidHandleException("env", env);
  }

  this->freeConnections(*it->second);
  this->environments.erase(it);
}

void Cask::CdapOdbc::Driver::freeConnection(SQLHDBC dbc) {
  std::lock_guard<std::mutex> lock(this->mutex);

  auto it = this->connections.find(dbc);
  if (it == this->connections.end()) {
    throw InvalidHandleException("dbc", dbc);
  }

  this->freeDescriptors(*it->second);
  this->freeStatements(*it->second);
  this->connections.erase(it);
}

void Cask::CdapOdbc::Driver::freeStatement(SQLHSTMT stmt) {
  std::lock_guard<std::mutex> lock(this->mutex);
  if (this->statements.erase(stmt) == 0) {
    throw InvalidHandleException("stmt", stmt);
  }
}

void Cask::CdapOdbc::Driver::freeDescriptor(SQLHDESC desc) {
  std::lock_guard<std::mutex> lock(this->mutex);
  if (this->descriptors.erase(desc) == 0) {
    throw InvalidHandleException("desc", desc);
  }
}

void Cask::CdapOdbc::Driver::setupSupportedFunctions(SQLUSMALLINT* bitset) {
  if (bitset == nullptr) {
    throw std::invalid_argument("bitset");
  }

  auto it = stdext::make_checked_array_iterator(bitset, SQL_API_ODBC3_ALL_FUNCTIONS_SIZE);
  std::fill(it, it + SQL_API_ODBC3_ALL_FUNCTIONS_SIZE, 0);

  setFunction(bitset, SQL_API_SQLALLOCHANDLE);
  setFunction(bitset, SQL_API_SQLBINDCOL);
  setFunction(bitset, SQL_API_SQLBINDPARAMETER);
  setFunction(bitset, SQL_API_SQLBULKOPERATIONS);
  setFunction(bitset, SQL_API_SQLCANCEL);
  setFunction(bitset, SQL_API_SQLCLOSECURSOR);
  setFunction(bitset, SQL_API_SQLCOLATTRIBUTE);
  setFunction(bitset, SQL_API_SQLCOLUMNS);
  setFunction(bitset, SQL_API_SQLCONNECT);
  setFunction(bitset, SQL_API_SQLCOPYDESC);
  setFunction(bitset, SQL_API_SQLDESCRIBECOL);
  setFunction(bitset, SQL_API_SQLDISCONNECT);
  setFunction(bitset, SQL_API_SQLDRIVERCONNECT);
  setFunction(bitset, SQL_API_SQLDRIVERS);
  setFunction(bitset, SQL_API_SQLENDTRAN);
  setFunction(bitset, SQL_API_SQLEXECDIRECT);
  setFunction(bitset, SQL_API_SQLEXECUTE);
  setFunction(bitset, SQL_API_SQLEXTENDEDFETCH);
  setFunction(bitset, SQL_API_SQLFETCH);
  setFunction(bitset, SQL_API_SQLFETCHSCROLL);
  setFunction(bitset, SQL_API_SQLFREEHANDLE);
  setFunction(bitset, SQL_API_SQLFREESTMT);
  setFunction(bitset, SQL_API_SQLGETCONNECTATTR);
  setFunction(bitset, SQL_API_SQLGETCURSORNAME);
  setFunction(bitset, SQL_API_SQLGETDATA);
  setFunction(bitset, SQL_API_SQLGETDESCFIELD);
  setFunction(bitset, SQL_API_SQLGETDESCREC);
  setFunction(bitset, SQL_API_SQLGETDIAGFIELD);
  setFunction(bitset, SQL_API_SQLGETDIAGREC);
  setFunction(bitset, SQL_API_SQLGETFUNCTIONS);
  setFunction(bitset, SQL_API_SQLGETINFO);
  setFunction(bitset, SQL_API_SQLGETSTMTATTR);
  setFunction(bitset, SQL_API_SQLGETTYPEINFO);
  setFunction(bitset, SQL_API_SQLMORERESULTS);
  setFunction(bitset, SQL_API_SQLNATIVESQL);
  setFunction(bitset, SQL_API_SQLNUMPARAMS);
  setFunction(bitset, SQL_API_SQLNUMRESULTCOLS);
  setFunction(bitset, SQL_API_SQLPARAMDATA);
  setFunction(bitset, SQL_API_SQLPREPARE);
  setFunction(bitset, SQL_API_SQLPROCEDURECOLUMNS);
  setFunction(bitset, SQL_API_SQLPROCEDURES);
  setFunction(bitset, SQL_API_SQLPRIMARYKEYS);
  setFunction(bitset, SQL_API_SQLPUTDATA);
  setFunction(bitset, SQL_API_SQLROWCOUNT);
  setFunction(bitset, SQL_API_SQLSETCONNECTATTR);
  setFunction(bitset, SQL_API_SQLSETCURSORNAME);
  setFunction(bitset, SQL_API_SQLSETDESCFIELD);
  setFunction(bitset, SQL_API_SQLSETDESCREC);
  setFunction(bitset, SQL_API_SQLSETENVATTR);
  setFunction(bitset, SQL_API_SQLSETPOS);
  setFunction(bitset, SQL_API_SQLSETSTMTATTR);
  setFunction(bitset, SQL_API_SQLSPECIALCOLUMNS);
  setFunction(bitset, SQL_API_SQLSTATISTICS);
  setFunction(bitset, SQL_API_SQLTABLES);
}

void Cask::CdapOdbc::Driver::addDataSource(HWND parentWindow, const std::wstring& driver, const std::wstring& attrs) {
  auto dialog = std::make_unique<DataSourceDialog>(parentWindow);
  if (dialog->show()) {
    if (!::SQLWriteDSNToIniW(dialog->getName().c_str(), driver.c_str())) {
      throw std::logic_error("Cannot write DSN.");
    }
  }
}

void Cask::CdapOdbc::Driver::modifyDataSource(HWND parentWindow, const std::wstring& driver, const std::wstring& attrs) {
  auto params = ConnectionParams(attrs);
  auto dialog = std::make_unique<DataSourceDialog>(parentWindow);
  dialog->setName(params.getDsn());
  if (dialog->show()) {
    if (params.getDsn() != dialog->getName()) {
      ::SQLRemoveDSNFromIniW(params.getDsn().c_str());
      if (!::SQLWriteDSNToIniW(dialog->getName().c_str(), driver.c_str())) {
        throw std::logic_error("Cannot write DSN.");
      }
    }
  }
}

void Cask::CdapOdbc::Driver::deleteDataSource(const std::wstring& driver, const std::wstring& attrs) {
  auto params = ConnectionParams(attrs);
  ::SQLRemoveDSNFromIniW(params.getDsn().c_str());
}
