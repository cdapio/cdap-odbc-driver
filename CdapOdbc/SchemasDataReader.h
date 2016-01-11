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

#include "DataReader.h"

namespace Cask {
  namespace CdapOdbc {

    /**
     * Reads a forward-only stream of rows.
     */
    class SchemasDataReader : public DataReader {
   
    public:

      // Inherited via DataReader
      virtual bool read() override;

      // Inherited via DataReader
      virtual void getColumnValue(const ColumnBinding & binding) override;

      // Inherited via DataReader
      virtual short getColumnCount() const override;
    };
  }
}
