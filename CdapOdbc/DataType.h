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
     * Holds information aabout data type.
     */
    class DataType {
      std::wstring name;
      std::wstring createParams;
      SQLULEN size;
      SQLLEN displaySize;
      SQLSMALLINT sqlType;
      SQLSMALLINT decimalDigits;
      SQLSMALLINT nullable;
      SQLSMALLINT searchable;
      SQLSMALLINT fixedPrecScale;
      ////case 4: // LITERAL_PREFIX 
      ////case 5: // LITERAL_SUFFIX
      ////case 10: // UNSIGNED_ATTRIBUTE
      ////case 12: // AUTO_UNIQUE_VALUE 
      ////case 13: // LOCAL_TYPE_NAME 
      ////case 14: // MINIMUM_SCALE 
      ////case 15: // MAXIMUM_SCALE 
      ////case 17: // SQL_DATETIME_SUB 
      ////case 18: // NUM_PREC_RADIX 
      ////case 19: // INTERVAL_PRECISION 

    public:

      /**
       * Creates an instance of DataType.
       */
      DataType(
        const std::wstring& name, 
        SQLSMALLINT sqlType, 
        SQLULEN size = 0U,
        SQLLEN displaySize = 0,
        SQLSMALLINT decimalDigits = 0, 
        SQLSMALLINT nullable = SQL_NULLABLE);

      /**
       * Gets type name.
       */
      const std::wstring& getName() const {
        return this->name;
      }

      /**
       * Gets the size (in characters) of the column.
       * If the column size cannot be determined, returns 0.
       */
      SQLULEN getSize() const {
        return this->size;
      }

      /**
       * Gets the SQL data type of the column.
       */
      SQLSMALLINT getSqlType() const {
        return this->sqlType;
      }

      /**
       * Gets the number of decimal digits of the column. If the number of decimal digits
       * cannot be determined or is not applicable, the driver returns 0.
       */
      SQLSMALLINT getDecimalDigits() const {
        return this->decimalDigits;
      }

      /**
       * Gets a value that indicates whether the column allows NULL values.
       */
      SQLSMALLINT getNullable() const {
        return this->nullable;
      }

      /**
       * Gets maximum number of characters required to display data from the column.
       */
      SQLLEN getDisplaySize() const {
        return this->displaySize;
      }
    };
  }
}
