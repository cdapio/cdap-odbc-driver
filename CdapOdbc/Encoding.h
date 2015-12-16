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
      Utility class for converting strings to UTF-8 and UTF-16 encodings.
    */
    class Encoding {
      static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;

      Encoding() = delete;

    public:
      
      /**
        Converts UTF-8 string to UTF-16 string.
      */
      static std::wstring toUtf16(const std::string& utf8);

      /**
        Converts UTF-16 string to UTF-8 string.
      */
      static std::string toUtf8(const std::wstring& utf16);
    };
  }
}