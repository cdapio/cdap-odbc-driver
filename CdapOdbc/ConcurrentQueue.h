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

    /**
     * Represents the classic multiple producer, multiple consumer thread-safe queue.
     */
    template <typename T>
    class ConcurrentQueue {
      std::queue<T> queue;
      mutable std::mutex mutex;

    public:

      /**
       * Accesses next element.
       */ 
      T& front() {
        std::unique_lock<std::mutex> lock(this->mutex);
        return this->queue.front();
      }

      /**
       * Accesses next element.
       */
      const T& front() const {
        std::unique_lock<std::mutex> lock(this->mutex);
        return this->queue.front();
      }

      /**
       * Removes next element.
       */
      void pop() {
        std::unique_lock<std::mutex> lock(this->mutex);
        queue.pop();
      }

      /**
       * Inserts element.
       */
      void push(const T& item) {
        std::unique_lock<std::mutex> lock(this->mutex);
        queue.push(item);
      }

      /**
       * Inserts element.
       */
      void push(T&& item) {
        std::unique_lock<std::mutex> lock(this->mutex);
        queue.push(std::move(item));
      }

      /**
       * Tests whether container is empty.
       */
      bool empty() const {
        std::unique_lock<std::mutex> lock(this->mutex);
        return queue.empty();
      }

      /**
       * Returns size.
       */
      size_t size() const {
        std::unique_lock<std::mutex> lock(this->mutex);
        return queue.size();
      }
    };
  }
}