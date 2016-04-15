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
    class LogAppender;

    /**
     * Writes logs into files.
     */
    class Logger {

      static std::unique_ptr<Logger> instance;
      std::vector<std::unique_ptr<LogAppender>> appenders;

    public:

      /**
       * Creates an instance of logger.
       */
      Logger();

      /**
      * Gets a logger instance.
      */
      static Logger& getInstance();

      /**
       * Adds new appender.
       */
      void addAppender(std::unique_ptr<LogAppender> appender);

      /**
       * Logs message.
       */
      void write(const wchar_t* message, ...);
    };
  }
}
