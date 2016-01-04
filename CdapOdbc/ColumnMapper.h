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

#include "ColumnDesc.h"
#include "ColumnBinding.h"

namespace Cask {
  namespace CdapOdbc {

    /**
     * Represents mappings of columns from Explore to ODBC datasets.
     */
    class ColumnMapper {
      std::vector<ColumnDesc> columnDescs;
      std::vector<ColumnBinding> columnBindings;

    protected:

      const std::vector<ColumnDesc>& getColumnDescs() const {
        return this->columnDescs;
      }

      virtual const ColumnDesc* getDesc(const ColumnBinding& binding) const;

    public:

      /**
       * Destructor
       */
      virtual ~ColumnMapper() = default;

      /**
       * Creates an instance of ColumnMapper.
       */
      ColumnMapper() = default;

      /**
       * Sets column bindings.
       */
      void setColumnBindings(const std::vector<ColumnBinding>& value) {
        this->columnBindings = value;
      }

      /**
      * Sets column descriptors.
      */
      void setColumnDescs(const std::vector<ColumnDesc>& value) {
        this->columnDescs = value;
      }

      /**
       * Maps data to column bindings.
       */
      void map(const web::json::value& data);
    };
  }
}
