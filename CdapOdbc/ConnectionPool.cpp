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
#include "ConnectionPool.h"
#include "ConnectionParams.h"
#include "ConnectionInfo.h"
#include "Driver.h"

using namespace Cask::CdapOdbc;

bool Cask::CdapOdbc::ConnectionPool::keysEqual(const Key& k1, const Key& k2) {
  return (_wcsicmp(k1.first.c_str(), k2.first.c_str()) == 0 && k1.second == k2.second);
}

POOLID Cask::CdapOdbc::ConnectionPool::getPoolID(const ConnectionInfo& info) {
  std::lock_guard<std::mutex> lock(this->mutex);
  auto key = std::make_pair(info.getParams().getHost(), info.getParams().getPort());
  auto& it = std::find_if(
    this->pools.begin(), 
    this->pools.end(), [this, key](const std::pair<Key, POOLID>& item) { return this->keysEqual(item.first, key); });
  if (it != this->pools.end()) {
    return it->second;
  } else {
    auto newID = reinterpret_cast<POOLID>(Driver::generateNewHandle());
    this->pools.insert({ key, newID });
    return newID;
  }
}

void Cask::CdapOdbc::ConnectionPool::removePool(POOLID poolID) {
  std::lock_guard<std::mutex> lock(this->mutex);
  auto& it = std::find_if(
    this->pools.begin(),
    this->pools.end(),
    [poolID](const std::pair<Key, POOLID>& item) { return item.second == poolID;});
  if (it != this->pools.end()) {
    this->pools.erase(it);
  }
}
