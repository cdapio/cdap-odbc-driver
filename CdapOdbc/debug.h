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

#ifdef ENABLE_PROFILING
#include "Timer.h"
#include "Profiler.h"
#include "TimerProxy.h"
#define DECLARE_TIMER(timer) Profiler::getInstance().addTimer(timer)
#define START_TIMER(timer) Profiler::getInstance().startTimer(timer)
#define STOP_TIMER(timer) Profiler::getInstance().stopTimer(timer)
#define PROFILE_FUNCTION(timer) TimerProxy timerProxy(timer)
#else
#define DECLARE_TIMER(timer)
#define START_TIMER(timer)
#define STOP_TIMER(timer)
#define PROFILE_FUNCTION(timer)
#endif