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

#include "ColumnBinding.h"
#include "ColumnInfo.h"

namespace Cask {
  namespace CdapOdbc {
    class Connection;

    /**
     * Reads a forward-only stream of rows.
     */
    class DataReader {
    
      DataReader(const DataReader&) = delete;
      void operator=(const DataReader&) = delete;

    protected:

      void fetchNull(const ColumnBinding& binding);
      void fetchString(const wchar_t* str, const ColumnBinding& binding);
      void fetchInt(SQLINTEGER value, const ColumnBinding& binding);
      void fetchSmallint(SQLSMALLINT value, const ColumnBinding& binding);

    public:

      /**
       * Creates an instance of DataReader.
       */
      DataReader() = default;

      /**
       * Destructor.
       */
      virtual ~DataReader() = default;

      /**
       * Advances the reader to the next record in a result set.
       */
      virtual bool read() = 0;

      /**
       * Gets the value of the specified column.
       */
      virtual void getColumnValue(const ColumnBinding& binding) = 0;

      /**
       * Gets number of columns in a result set.
       */
      virtual short getColumnCount() const = 0;

      /**
       * Gets column information.
       */
      virtual std::unique_ptr<ColumnInfo> getColumnInfo(short columnNumber) const = 0;
    };
  }
}
