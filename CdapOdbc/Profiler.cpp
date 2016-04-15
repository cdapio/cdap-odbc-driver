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
#include "Profiler.h"
#include "Timer.h"

std::unique_ptr<Cask::CdapOdbc::Profiler> Cask::CdapOdbc::Profiler::instance(std::make_unique<Cask::CdapOdbc::Profiler>());

Cask::CdapOdbc::Profiler::Profiler() {
  this->addTimer(TIMER_EXECDIRECT);
  this->addTimer(TIMER_TABLES);
  this->addTimer(TIMER_COLUMNS);
  this->addTimer(TIMER_FETCH);
  this->addTimer(TIMER_EXECUTE);
  this->addTimer(TIMER_PREPARE);
  this->addTimer(TIMER_QUERY);
  this->addTimer(TIMER_PARSING);
}

Cask::CdapOdbc::Profiler::~Profiler() {
  this->reportTimers();
}

Cask::CdapOdbc::Profiler& Cask::CdapOdbc::Profiler::getInstance() {
  return *instance;
}

void Cask::CdapOdbc::Profiler::addTimer(const std::wstring& name) {
  this->timers.insert_or_assign(name, std::make_unique<Timer>(name));
}

void Cask::CdapOdbc::Profiler::startTimer(const std::wstring& name) {
  this->timers.at(name)->start();
}

void Cask::CdapOdbc::Profiler::stopTimer(const std::wstring& name) {
  this->timers.at(name)->stop();
}

void Cask::CdapOdbc::Profiler::reportTimers() {
  for (auto& item : this->timers) {
    TRACE(L"Timer %s - %g seconds\n", item.first.c_str(), item.second->getTime() / 1000000000.0);
  }
}
