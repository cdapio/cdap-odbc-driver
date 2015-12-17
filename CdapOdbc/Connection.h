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

namespace Cask {
  namespace CdapOdbc {
    class Environment;

    class Connection {
      Environment* environment;
      SQLHDBC handle;
      std::mutex mutex;
      bool isOpen;

      Connection(const Connection&) = delete;
      void operator=(const Connection&) = delete;

    public:
      Connection(Environment* environment, SQLHDBC handle);
      ~Connection() = default;

      SQLHDBC getHandle() const {
        return this->handle;
      }

      void open(const std::string& connectionString);
      void close();
    };
  }
}
