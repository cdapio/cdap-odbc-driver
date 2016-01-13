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
#include "ColumnDesc.h"

namespace Cask {
  namespace CdapOdbc {
    class QueryCommand;

    /**
     * Reads a forward-only stream of rows.
     */
    class QueryDataReader : public DataReader {

      static const int FETCH_SIZE = 100;

      QueryCommand* queryCommand;
      std::vector<ColumnDesc> schema;
      QueryResult queryResult;
      int fetchCount;
      int currentRowIndex;
      bool moreData;
   
      bool loadData();

    public:

      /**
       * Creates an instance of QueryDataReader.
       */
      QueryDataReader(QueryCommand* command);

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
