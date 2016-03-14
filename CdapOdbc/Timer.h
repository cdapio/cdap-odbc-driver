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

    using Tick = std::uint64_t;

    /**
     * Represents a timer object for time measurement.
     */
    class Timer {
      
      using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
      
      std::wstring name;
      Tick diffTime;
      TimePoint startTime;
      bool running;

    public:

      /**
       * Creates an instance of timer.
       */
      Timer(const std::wstring& name);

      /**
       * Starts the timer.
       */
      void start();

      /**
       * Stops the timer.
       */
      void stop();

      /**
       * Resets the timer.
       */
      void reset();

      /**
       * Tests if timer is running.
       */
      bool isRunning() const {
        return this->running;
      }

      /**
       * Gets time.
       */
      Tick getTime();

      /**
       * Gets name.
       */
      const std::wstring& getName() const {
        return this->name;
      }
    };
  }
}