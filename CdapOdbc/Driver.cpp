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

using namespace Cask::CdapOdbc;

std::unique_ptr<Driver> Cask::CdapOdbc::Driver::instance(std::make_unique<Driver>());
std::atomic_int Cask::CdapOdbc::Driver::lastHandleIndex = 100;

SQLHANDLE Cask::CdapOdbc::Driver::generateNewHandle()
{
	char* ptr = nullptr;
	return static_cast<SQLHANDLE>(ptr + lastHandleIndex.fetch_add(1));
}

Driver& Cask::CdapOdbc::Driver::getInstance()
{
	return *instance;
}

bool Cask::CdapOdbc::Driver::hasEnvironment(SQLHENV env)
{
	assert(env);
	std::lock_guard<std::mutex> lock(this->envMutex);
	return this->environments.find(env) != this->environments.end();
}

Environment& Cask::CdapOdbc::Driver::getEnvironment(SQLHENV env)
{
	std::lock_guard<std::mutex> lock(this->envMutex);
	assert(this->environments.find(env) != this->environments.end());
	return *this->environments.at(env);
}

SQLHENV Cask::CdapOdbc::Driver::allocEnvironment()
{
	SQLHENV env = this->generateNewHandle();
	assert(env);
	std::lock_guard<std::mutex> lock(this->envMutex);
	this->environments.emplace(env, std::make_unique<Environment>());
	return env;
}

void Cask::CdapOdbc::Driver::freeEnvironment(SQLHENV env)
{
	assert(env);
	std::lock_guard<std::mutex> lock(this->envMutex);
	if (this->environments.erase(env) == 0)
	{
		throw std::invalid_argument("env");
	}
}
