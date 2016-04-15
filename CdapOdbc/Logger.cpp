/*
* Copyright © 2016 Cask Data, Inc.
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
#include "Logger.h"
#include "LogAppender.h"
#include "DebugLogAppender.h"
#include "FileLogAppender.h"

std::unique_ptr<Cask::CdapOdbc::Logger> Cask::CdapOdbc::Logger::instance(std::make_unique<Cask::CdapOdbc::Logger>());

Cask::CdapOdbc::Logger::Logger() {
  this->addAppender(std::make_unique<DebugLogAppender>());
  this->addAppender(std::make_unique<FileLogAppender>());
}

Cask::CdapOdbc::Logger& Cask::CdapOdbc::Logger::getInstance() {
  return *instance;
}

void Cask::CdapOdbc::Logger::addAppender(std::unique_ptr<LogAppender> appender) {
  this->appenders.emplace_back(std::move(appender));
}

void Cask::CdapOdbc::Logger::write(const wchar_t* message, ...) {
  va_list argList;
  va_start(argList, message);

  wchar_t buf[4096];
  _vsnwprintf_s(buf, _countof(buf), message, argList);
  std::wstring tmp = buf;
  for (auto& item : this->appenders) {
    item->write(tmp);
  }

  va_end(argList);
}
