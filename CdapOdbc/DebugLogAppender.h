/*
* Copyright � 2016 Cask Data, Inc.
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

#include "LogAppender.h"

namespace Cask {
  namespace CdapOdbc {

    /**
     * Writes logs into debug output window.
     */
    class DebugLogAppender : public LogAppender {

    public:

      // Inherited via LogAppender
      virtual void write(const std::wstring& message) override;
    };
  }
}