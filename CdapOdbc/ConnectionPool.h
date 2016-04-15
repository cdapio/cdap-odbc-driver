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

#pragma once

namespace Cask {
  namespace CdapOdbc {
    class ConnectionInfo;

    /**
     * Represents a connection pool.
     */
    class ConnectionPool {

      using Key = std::pair<std::wstring, int>;
      using PoolMap = std::map<Key, POOLID>;

      PoolMap pools;
      std::mutex mutex;

      bool keysEqual(const Key& k1, const Key& k2);

      ConnectionPool(const ConnectionPool&) = delete;
      void operator=(const ConnectionPool&) = delete;

    public:

      /**
       * Creates a connection pool instance.
       */
      ConnectionPool() = default;

      /**
       * Destructor.
       */
      ~ConnectionPool() = default;

      /**
       * Gets pool id for connection info.
       */
      POOLID getPoolID(const ConnectionInfo& info);

      /**
       * Removes pool id.
       */
      void removePool(POOLID poolID);
    };
  }
}
