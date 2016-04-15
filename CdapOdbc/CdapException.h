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
     * Base exception for all CDAP driver exceptions.
     */
    class CdapException : public std::logic_error {
      std::wstring sqlStatusCode;
      std::wstring message;
      SQLRETURN errorCode;

    public:

      /**
       * Creates an instance.
       */
      CdapException();

      /**
       * Creates an instance.
       */
      CdapException(const std::wstring& message);

      /**
       * Creates an instance.
       */
      CdapException(const std::wstring& sqlStatusCode, const std::wstring& message);

      /**
       * Creates an instance.
       */
      CdapException(SQLRETURN errorCode, const std::wstring& sqlStatusCode, const std::wstring& message);

      /**
      * Creates an instance.
      */
      CdapException(SQLRETURN errorCode, const std::wstring& message);

      /**
       * Copy consructor.
       */
      CdapException(const CdapException&);

      /**
       * Move consructor.
       */
      CdapException(CdapException&&);

      /**
       * Descructor.
       */
      virtual ~CdapException() override = default;

      /**
       * Assignment operator.
       */
      void operator=(const CdapException&);

      /**
       * Assignment move operator.
       */
      void operator=(CdapException&&);

      /**
       * Gets SQLState code.
       */
      const std::wstring& getSqlStatusCode() const {
        return this->sqlStatusCode;
      }

      /**
       * Gets error message.
       */
      const std::wstring& getMessage() const {
        return this->message;
      }

      /**
       * Gets error code.
       */
      SQLRETURN getErrorCode() const {
        return this->errorCode;
      }
    };
  }
}
