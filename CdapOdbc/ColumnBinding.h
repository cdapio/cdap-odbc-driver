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
    * Represents column binding information.
    */
    class ColumnBinding {
      SQLUSMALLINT columnNumber;
      SQLSMALLINT targetType;
      SQLPOINTER targetValuePtr;
      SQLLEN bufferLength;
      SQLLEN* strLenOrInd;

    public:

      /**
      * Creates a column binding instance.
      */
      ColumnBinding();

      /**
      * Destructor.
      */
      ~ColumnBinding() = default;

      /**
      * Gets a number of the result set column to bind.
      */
      SQLUSMALLINT getColumnNumber() const {
        return this->columnNumber;
      }

      /**
      * Gets an identifier of the C data type of the *TargetValuePtr buffer.
      */
      SQLSMALLINT getTargetType() const {
        return this->targetType;
      }

      /**
      * Gets a pointer to the data buffer to bind to the column.
      */
      SQLPOINTER getTargetValuePtr() const {
        return this->targetValuePtr;
      }

      /**
      * Gets a length of the *TargetValuePtr buffer in bytes.
      */
      SQLLEN getBufferLength() const {
        return this->bufferLength;
      }

      /**
      * Gets a pointer to the length/indicator buffer to bind to the column.
      */
      SQLLEN* getStrLenOrInd() const {
        return this->strLenOrInd;
      }

      /**
      * Sets a number of the result set column to bind.
      */
      void setColumnNumber(SQLUSMALLINT value) {
        this->columnNumber = value;
      }

      /**
      * Sets an identifier of the C data type of the *TargetValuePtr buffer.
      */
      void setTargetType(SQLSMALLINT value) {
        this->targetType = value;
      }

      /**
      * Sets a pointer to the data buffer to bind to the column.
      */
      void setTargetValuePtr(SQLPOINTER value) {
        this->targetValuePtr = value;
      }

      /**
      * Sets a length of the *TargetValuePtr buffer in bytes.
      */
      void setBufferLength(SQLLEN value) {
        this->bufferLength = value;
      }

      /**
      * Sets a pointer to the length/indicator buffer to bind to the column.
      */
      void setStrLenOrInd(SQLLEN* value) {
        this->strLenOrInd = value;
      }
    };
  }
}
