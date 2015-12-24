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
     * Represents the status of an operation.
     */
    enum class OperationStatus {
      INITIALIZED,
      RUNNING,
      FINISHED,
      CANCELED,
      CLOSED,
      ERROR,
      UNKNOWN,
      PENDING,
    };

    /**
     * Represents the status of a submitted query operation.
     */
    class QueryStatus {
      OperationStatus opStatus;
      bool results;

      OperationStatus parseStatus(const utility::string_t& value);

    public:

      /**
       * Creates an instance of QueryStatus.
       */
      QueryStatus();

      /**
       * Creates an instance of QueryStatus from JSON.
       */
      QueryStatus(web::json::value value);

      /**
       * Gets if a query not running any more.
       */
      bool isDone() {
        return this->opStatus == OperationStatus::FINISHED || 
               this->opStatus == OperationStatus::CANCELED || 
               this->opStatus == OperationStatus::CLOSED ||
               this->opStatus == OperationStatus::ERROR;
      }

      /**
       * Gets an operation status.
       */
      OperationStatus getOperationStatus() {
        return this->opStatus;
      }

      /**
       * Gets if a query has some results.
       */
      bool hasResults() {
        return this->results;
      }

      /**
       * Sets an operation status.
       */
      void setOperationStatus(OperationStatus value) {
        this->opStatus = value;
      }

      /**
       * Sets if a query has some results.
       */
      void setHasResults(bool value) {
        this->results = value;
      }
    };
  }
}
