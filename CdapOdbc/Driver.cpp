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

using namespace Cask::CdapOdbc;

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

  throw std::invalid_argument("env");
}

Connection* Cask::CdapOdbc::Driver::findConnection(SQLHDBC dbc) {
  auto it = this->connections.find(dbc);
  if (it != this->connections.end()) {
    return it->second.get();
  }

  throw std::invalid_argument("dbc");
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

  throw std::invalid_argument("stmt");
}

Descriptor & Cask::CdapOdbc::Driver::getDescriptor(SQLHDESC desc) {
  std::lock_guard<std::mutex> lock(this->mutex);
  auto it = this->descriptors.find(desc);
  if (it != this->descriptors.end()) {
    return *(it->second);
  }

  throw std::invalid_argument("desc");
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
    throw std::invalid_argument("env");
  }

  this->freeConnections(*it->second);
  this->environments.erase(it);
}

void Cask::CdapOdbc::Driver::freeConnection(SQLHDBC dbc) {
  std::lock_guard<std::mutex> lock(this->mutex);

  auto it = this->connections.find(dbc);
  if (it == this->connections.end()) {
    throw std::invalid_argument("dbc");
  }

  this->freeDescriptors(*it->second);
  this->freeStatements(*it->second);
  this->connections.erase(it);
}

void Cask::CdapOdbc::Driver::freeStatement(SQLHSTMT stmt) {
  std::lock_guard<std::mutex> lock(this->mutex);
  if (this->statements.erase(stmt) == 0) {
    throw std::invalid_argument("stmt");
  }
}

void Cask::CdapOdbc::Driver::freeDescriptor(SQLHDESC desc) {
  std::lock_guard<std::mutex> lock(this->mutex);
  if (this->descriptors.erase(desc) == 0) {
    throw std::invalid_argument("desc");
  }
}
