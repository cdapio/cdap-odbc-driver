/*
 * Copyright � 2015 Cask Data, Inc.
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

#ifdef _DEBUG

inline void debugFPrint(const wchar_t* fmtString, ...) {
  va_list argList;
  va_start(argList, fmtString);
  wchar_t buf[4096];
  _vsnwprintf_s(buf, sizeof(buf), fmtString, argList);
  OutputDebugStringW(buf);
  va_end(argList);
}

#endif

#ifdef _DEBUG
#define TRACE(msg, ...) debugFPrint(msg, __VA_ARGS__)
#else
#define TRACE(msg, ...)
#endif

#ifdef _DEBUG

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

inline auto startTimePoint() {
  return std::chrono::high_resolution_clock::now();
}

inline void endTimePoint(TimePoint startTime) {
  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = endTime - startTime;
  TRACE(L"Duration: %g seconds\n", duration.count() / 1000000000.0);
}

#endif