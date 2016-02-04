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
#include "QueryResult.h"

namespace Cask {
  namespace CdapOdbc {

    /**
     * Reads a forward-only stream of rows.
     */
    class TablesDataReader : public DataReader {
   
      QueryResult streams;
      QueryResult datasets;
      int currentRowIndex;

      std::wstring getTableName();

    public:

      /**
       * Creates an instance of TablesDataReader.
       */
      TablesDataReader(const QueryResult& streams, const QueryResult& datasets);

      // Inherited via DataReader
      virtual bool read() override;

      // Inherited via DataReader
      virtual void getColumnValue(const ColumnBinding& binding) override;

      // Inherited via DataReader
      virtual short getColumnCount() const override;

      // Inherited via DataReader
      virtual std::unique_ptr<ColumnInfo> getColumnInfo(short columnNumber) const override;
    };
  }
}
