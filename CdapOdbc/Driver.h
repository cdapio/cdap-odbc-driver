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

#pragma once

namespace Cask
{
	namespace CdapOdbc
	{
		class Environment;

		/**
			Represents root object of ODBC driver.

			Implemented as a singleton.
		*/
		class Driver
		{
			static std::unique_ptr<Driver> instance;
			static std::atomic_int lastHandleIndex;

			std::unordered_map<SQLHENV, std::unique_ptr<Environment>> environments;
			std::mutex mutex;

			Driver(const Driver&) = delete;
			void operator=(const Driver&) = delete;

		public:

			Driver() = default;
			~Driver() = default;

			static Driver& getInstance();
			static SQLHANDLE generateNewHandle();

			bool hasEnvironment(SQLHENV);
			Environment& getEnvironment(SQLHENV);
			SQLHENV allocEnvironment();
			bool freeEnvironment(SQLHENV);
			bool freeConnection(SQLHDBC);
		};
	}
}
