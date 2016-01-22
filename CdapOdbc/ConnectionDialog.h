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

#include "ConnectionParams.h"

namespace Cask {
  namespace CdapOdbc {
 
    class ConnectionDialog {
    
      HWND parent;
      bool result;
      ConnectionParams params;

      static INT_PTR CALLBACK dialogProc(_In_ HWND hwndDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
      void init(HWND window);
      void readFromDialog(HWND window);

      ConnectionDialog(const ConnectionDialog&) = delete;
      void operator=(const ConnectionDialog&) = delete;
    
    public:
      
      /**
       * Creates an instance of ConnectionDialog.
       */
      ConnectionDialog(HWND parent);

      /**
       * Destructor.
       */
      ~ConnectionDialog();

      /**
       * Gets connection params.
       */
      const ConnectionParams& getParams() const {
        return this->params;
      }

      /**
       * Sets connection params.
       */
      void setParams(const ConnectionParams& value) {
        this->params = value;
      }

      /**
       * Shows connection dialog.
       */
      bool show();
    };
  }
}