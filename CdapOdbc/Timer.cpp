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
#include "Timer.h"

Cask::CdapOdbc::Timer::Timer() 
  : diffTime(0)
  , running(false) {
}

void Cask::CdapOdbc::Timer::start() {
  assert(!this->running);
  this->startTime = std::chrono::high_resolution_clock::now();
  this->running = true;
}

void Cask::CdapOdbc::Timer::stop() {
  assert(this->running);
  auto now = std::chrono::high_resolution_clock::now();
  this->diffTime += (now - this->startTime).count();
  this->startTime = TimePoint();
  this->running = false;
}

void Cask::CdapOdbc::Timer::reset() {
  bool wasRunning = this->running;
  if (wasRunning) {
    this->stop();
  }

  this->startTime = TimePoint();
  this->diffTime = 0;
  if (wasRunning) {
    this->start();
  }
}

Cask::CdapOdbc::Tick Cask::CdapOdbc::Timer::getTime() {
  if (this->running) {
    auto now = std::chrono::high_resolution_clock::now();
    return (now - this->startTime).count() + this->diffTime;
  } else {
    return this->diffTime;
  }
}
