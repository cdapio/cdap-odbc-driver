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
      web::json::value rows;
      std::int64_t sizeInBytes;
    
    public:

      /**
       * Creates an instance of QueryResult.
       */
      QueryResult();

      /**
      * Creates an instance of QueryResult from JSON.
      */
      QueryResult(const web::json::value& value);

      /**
       * Creates an instance of QueryResult from JSON.
       */
      QueryResult(const web::json::value& value, std::int64_t sizeInBytes);

      /**
       * Gets a query data.
       */
      const web::json::array& getRows() const {
        return this->rows.as_array();
      }

      /**
       * Gets a number of rows.
       */
      int getSize() const {
        if (this->rows.is_array()) {
          return static_cast<int>(this->rows.as_array().size());
        } else {
          return 0;
        }
      }

      /**
       * Gets size in bytes of REST request with results.
       */
      std::int64_t getSizeInBytes() const {
        return this->sizeInBytes;
      }
    };
  }
}
