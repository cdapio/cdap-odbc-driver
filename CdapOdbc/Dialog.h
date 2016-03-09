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
 
    class Dialog {
    
      HWND handle;
      UINT id;
      HWND parent;
      bool result;
      HFONT boldFont;

      static INT_PTR CALLBACK dialogProc(_In_ HWND hwndDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

      void initBoldFont();

      Dialog(const Dialog&) = delete;
      void operator=(const Dialog&) = delete;

    protected:

      HWND getHandle() const {
        return this->handle;
      }

      HFONT getBoldFont() const {
        return this->boldFont;
      }

      virtual INT_PTR proc(UINT uMsg, WPARAM wParam, LPARAM lParam);
      virtual void init();
      virtual void readFromDialog();

      Dialog(HWND parent, UINT id);

    public:
      
      /**
       * Destructor.
       */
      virtual ~Dialog();

      /**
       * Shows dialog.
       */
      bool show();
    };
  }
}