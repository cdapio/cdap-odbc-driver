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

    /**
     * Allocator for secure string.
     */
    template <typename T>
    class SecureAllocator : public std::allocator<T> {
    public:

      template<typename U>
      struct rebind {
        typedef SecureAllocator<U> other;
      };

      SecureAllocator() throw() {
      }

      SecureAllocator(const SecureAllocator&) throw() {
      }

      template <typename U>
      SecureAllocator(const SecureAllocator<U>&) throw() {
      }

      void deallocate(pointer p, size_type n) {
        SecureZeroMemory(p, n * sizeof(T));
        std::allocator<T>::deallocate(p, n);
      }
    };

    /**
     * Secure string.
     */
    using SecureString = std::basic_string<wchar_t, std::char_traits<wchar_t>, SecureAllocator<wchar_t>>;
    using SecureStringStream = std::basic_stringstream<wchar_t, std::char_traits<wchar_t>, SecureAllocator<wchar_t>>;

    /**
     * Utility class for manipulating strings.
     */
    class String {
      String() = delete;

    public:

      /**
       * Splits a string to tokens separated by delimiter character.
       */
      static void split(const SecureString& str, wchar_t delim, std::vector<SecureString>& tokens);

      /**
       * Removes whitespaces from the start and the end of a string.
       */
      static SecureString trim(const SecureString& str);

      /**
       * Converts double to string with specified width.
       */
      static std::wstring fromDouble(double value, int width);
    };
  }
}