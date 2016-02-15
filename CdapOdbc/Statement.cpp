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
#include "Statement.h"
#include "Connection.h"
#include "NoDataException.h"
#include "String.h"
#include "Encoding.h"
#include "SchemasCommand.h"
#include "TablesCommand.h"
#include "TypesCommand.h"
#include "ColumnsCommand.h"
#include "SpecialColumnsCommand.h"
#include "QueryCommand.h"

void Cask::CdapOdbc::Statement::throwStateError() const {
  throw std::logic_error("Wrong statement state.");
}

void Cask::CdapOdbc::Statement::openQuery() {
  if (this->state != State::PREPARE) {
    this->throwStateError();
  }
  
  this->dataReader = this->command->executeReader();
  this->state = State::OPEN;
}

Cask::CdapOdbc::Statement::Statement(Connection* connection, SQLHSTMT handle)
  : state(State::INITIAL)
  , connection(connection)
  , handle(handle) {
  assert(connection);
  assert(handle);
}

void Cask::CdapOdbc::Statement::addColumnBinding(const ColumnBinding& binding) {
  if (this->state == State::CLOSED) {
    this->throwStateError();
  }

  auto it = std::find_if(
    this->columnBindings.begin(),
    this->columnBindings.end(),
    [binding](auto& b) { return b.getColumnNumber() == binding.getColumnNumber(); });
  if (it != this->columnBindings.end()) {
    // Replace existing binding. 
    *it = binding;
  } else {
    // Add new binding. 
    this->columnBindings.push_back(binding);
  }
}

void Cask::CdapOdbc::Statement::removeColumnBinding(SQLUSMALLINT columnNumber) {
  if (this->state == State::CLOSED) {
    this->throwStateError();
  }

  auto it = std::find_if(
    this->columnBindings.begin(),
    this->columnBindings.end(),
    [columnNumber](auto& b) { return b.getColumnNumber() == columnNumber; });
  if (it == this->columnBindings.end()) {
    throw std::invalid_argument("columnNumber");
  }

  this->columnBindings.erase(it);
}

void Cask::CdapOdbc::Statement::getSchemas(const std::wstring* catalog, const std::wstring* schemaPattern) {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->command = std::make_unique<SchemasCommand>(this->connection);
  this->state = State::PREPARE;

  this->openQuery();
}

void Cask::CdapOdbc::Statement::getTables(
  const std::wstring* catalog,
  const std::wstring* schemaPattern,
  const std::wstring* tableNamePattern,
  const std::wstring* tableTypes) {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->command = std::make_unique<TablesCommand>(this->connection);
  this->state = State::PREPARE;

  this->openQuery();
}

void Cask::CdapOdbc::Statement::getDataTypes() {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->command = std::make_unique<TypesCommand>(this->connection);
  this->state = State::PREPARE;

  this->openQuery();
}

void Cask::CdapOdbc::Statement::getColumns(const std::wstring& streamName) {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->command = std::make_unique<ColumnsCommand>(this->connection, streamName);
  this->state = State::PREPARE;

  this->openQuery();
}

void Cask::CdapOdbc::Statement::getSpecialColumns() {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->command = std::make_unique<SpecialColumnsCommand>(this->connection);
  this->state = State::PREPARE;

  this->openQuery();
}

void Cask::CdapOdbc::Statement::fetch() {
  if (this->state != State::OPEN && this->state != State::FETCH) {
    this->throwStateError();
  }

  this->state = State::FETCH;
  if (this->dataReader->read()) {
    for (auto& binding : this->columnBindings) {
      this->dataReader->getColumnValue(binding);
    }
  } else {
    this->state = State::CLOSED;
    throw NoDataException(L"No more data in the query.");
  }
}

void Cask::CdapOdbc::Statement::reset() {
  if (this->state != State::INITIAL) {
    this->command.reset();
    this->dataReader.reset();
    this->state = State::INITIAL;
  }
}

void Cask::CdapOdbc::Statement::unbindColumns() {
  if (this->state != State::INITIAL &&
    this->state != State::PREPARE &&
    this->state != State::OPEN) {
    this->throwStateError();
  }

  this->columnBindings.clear();
}

void Cask::CdapOdbc::Statement::resetParameters() {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }
}

void Cask::CdapOdbc::Statement::prepare(const std::wstring& query) {
  if (this->state != State::INITIAL) {
    this->throwStateError();
  }

  this->command = std::make_unique<QueryCommand>(this->connection, query);
  this->state = State::PREPARE;
}

void Cask::CdapOdbc::Statement::execute() {
  this->openQuery();
}

void Cask::CdapOdbc::Statement::executeDirect(const std::wstring& query) {
  if (this->state != State::INITIAL && this->state != State::PREPARE) {
    this->throwStateError();
  }

  if (this->state == State::INITIAL) {
    this->command = std::make_unique<QueryCommand>(this->connection, query);
    this->state = State::PREPARE;
  }
  
  if (this->state == State::PREPARE) {
    this->openQuery();
  }
}

SQLSMALLINT Cask::CdapOdbc::Statement::getColumnCount() const {
  if (this->state != State::OPEN) {
    this->throwStateError();
  }

  return this->dataReader->getColumnCount();
}

std::unique_ptr<Cask::CdapOdbc::ColumnInfo> Cask::CdapOdbc::Statement::getColumnInfo(short columnNumber) const {
  return this->dataReader->getColumnInfo(columnNumber);
}
