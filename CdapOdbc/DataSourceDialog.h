#pragma once
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

#include "Dialog.h"

namespace Cask {
  namespace CdapOdbc {
 
    class DataSourceDialog : public Dialog {

      std::wstring name;

    protected:

      virtual INT_PTR proc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
      virtual void init() override;
      virtual void readFromDialog() override;

    public:

      /**
       * Creates an instance of DataSourceDialog.
       */
      DataSourceDialog(HWND parent);

      /**
       * Gets data source name.
       */
      const std::wstring& getName() const {
        return this->name;
      }

      /**
       * Sets data source name.
       */
      void setName(const std::wstring& value) {
        this->name = value;
      }
    };
  }
}