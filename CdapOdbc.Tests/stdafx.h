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

#include "targetver.h"

#include "CppUnitTest.h"

#include <Windows.h>
#include <sql.h>
#include <sqlext.h>
#include <sqlspi.h>

#include <tuple>

#define TEST_CONNECTION_STRING L"Driver=Cask CDAP ODBC Driver;Host=104.154.22.63"
#define TEST_CONNECTION_STRING2 L"Driver=Cask CDAP ODBC Driver;Host=localhost"