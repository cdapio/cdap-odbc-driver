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
     * Represents a query result.
     */
    class QueryResult {
      web::json::value columns;
    
    public:

      /**
       * Creates an instance of QueryResult.
       */
      QueryResult() = default;

      /**
      * Creates an instance of QueryResult from JSON.
      */
      QueryResult(const web::json::value& value);

      /**
       * Gets a query data.
       */
      const web::json::array& getColumns() {
        return this->columns.as_array();
      }
    };
  }
}
