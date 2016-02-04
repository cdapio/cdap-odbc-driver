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

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN             

// Windows Header Files:
#include <windows.h>

// Conflicts with query op status
#undef ERROR
#undef max
#undef min

#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifndef DBG_NEW
#define DBG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#define new DBG_NEW
#endif

#endif // _DEBUG

#include <cassert>

#include <memory>
#include <map>
#include <atomic>
#include <mutex>
#include <exception>
#include <stdexcept>
#include <codecvt>
#include <locale>
#include <string>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <array>
#include <limits>
#include <cwctype>
#include <iomanip>

#include <cpprest/uri.h>
#include <cpprest/uri_builder.h>
#include <cpprest/http_client.h>
#include <cpprest/http_headers.h>
#include <cpprest/json.h>

#include "debug.h"