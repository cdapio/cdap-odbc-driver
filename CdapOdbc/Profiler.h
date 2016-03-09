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
    class Timer;

    /**
     * Represents a profiler object for managing timers.
     */
    class Profiler {
      static std::unique_ptr<Profiler> instance;
      std::map<std::wstring, std::unique_ptr<Timer>> timers;

      Profiler(const Profiler&) = delete;
      void operator=(const Profiler&) = delete;

    public:

      /**
       * Creates a profiler instance.
       */
      Profiler() = default;

      /**
       * Destructor.
       */
      ~Profiler();

      /**
       * Gets a profiler instance.
       */
      static Profiler& getInstance();

      /**
       * Adds a new timer.
       */
      void addTimer(const std::wstring& name);

      /**
       * Starts a timer.
       */
      void startTimer(const std::wstring& name);
  
      /**
       * Stops a timer.
       */
      void stopTimer(const std::wstring& name);
  
      /**
       * Outputs time information into debug window.
       */
      void reportTimers();
    };
  }
}