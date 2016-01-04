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
     * Represents a column inside a query result.
     */
    class ColumnDesc {
      utility::string_t name;
      utility::string_t type;
      int position;
      utility::string_t comment;

    public:

      /**
       * Creates an instance of ColumnDesc.
       */
      ColumnDesc();

      /**
      * Creates an instance of ColumnDesc from JSON.
      */
      ColumnDesc(web::json::value value);

      /**
       * Gets a column name.
       */
      const utility::string_t& getName() const {
        return this->name;
      }

      /**
      * Gets a column type.
      */
      const utility::string_t& getType() const {
        return this->type;
      }

      /**
      * Gets a column position.
      */
      int getPosition() const {
        return this->position;
      }

      /**
      * Gets a column comment.
      */
      const utility::string_t& getComment() const {
        return this->comment;
      }

      /**
      * Sets a column name.
      */
      void setName(const utility::string_t& value) {
        this->name = value;
      }

      /**
      * Sets a column type.
      */
      void setType(const utility::string_t& value) {
        this->type = value;
      }

      /**
      * Sets a column position.
      */
      void setPosition(int value) {
        this->position = value;
      }

      /**
      * Sets a column comment.
      */
      void setComment(const utility::string_t& value) {
        this->comment = value;
      }
    };
  }
}
