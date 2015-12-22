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
    class Connection;

    /**
     * Represents an ODBC descriptor which holds information 
     * about either columns or dynamic parameters.
     */
    class Descriptor {
      Connection* connection;
      SQLHDESC handle;

    public:

      /**
       * Creates a descriptor instance.
       */
      Descriptor(Connection* connection, SQLHDESC handle);
      
      /**
       * Desctructor.
       */ 
      ~Descriptor() = default;

      /**
       * Gets a descriptor handle.
       */
      SQLHDESC getHandle() const {
        return this->handle;
      }

      /**
       * Gets parent connection.
       */
      Connection* getConnection() const {
        return this->connection;
      }
    };
  }
}
