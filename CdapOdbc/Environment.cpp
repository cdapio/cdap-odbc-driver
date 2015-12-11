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
#include "Environment.h"
#include "Driver.h"

using namespace Cask::CdapOdbc;

bool Cask::CdapOdbc::Environment::hasConnection(SQLHDBC dbc)
{
	assert(dbc);
	std::lock_guard<std::mutex> lock(this->mutex);
	return this->connections.find(dbc) != this->connections.end();
}

Connection& Cask::CdapOdbc::Environment::getConnection(SQLHDBC dbc)
{
	std::lock_guard<std::mutex> lock(this->mutex);
	auto it = this->connections.find(dbc);
	if (it == this->connections.end())
	{
		throw std::invalid_argument("dbc");
	}

	return *(it->second);
}

SQLHDBC Cask::CdapOdbc::Environment::allocConnection()
{
	SQLHDBC dbc = Driver::generateNewHandle();
	assert(dbc);
	std::lock_guard<std::mutex> lock(this->mutex);
	this->connections.emplace(dbc, std::make_unique<Connection>());
	return dbc;
}

bool Cask::CdapOdbc::Environment::freeConnection(SQLHDBC dbc)
{
	assert(dbc);
	std::lock_guard<std::mutex> lock(this->mutex);
	return (this->connections.erase(dbc) != 0);
}
