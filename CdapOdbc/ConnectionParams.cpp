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

#include "stdafx.h"
#include "ConnectionParams.h"

namespace {

  void split(const std::wstring& str, wchar_t delim, std::vector<std::wstring>& tokens);
  std::wstring trim(const std::wstring& str);
  bool equals(const std::wstring& str1, const std::wstring& str2);
  bool parseBool(const std::wstring& str);
  int parseInt(const std::wstring& str);

  void split(const std::wstring& str, wchar_t delim, std::vector<std::wstring>& tokens) {
    std::wstringstream stream(str);
    std::wstring item;
    while (std::getline(stream, item, delim)) {
      tokens.push_back(item);
    }
  }

  std::wstring trim(const std::wstring& str) {
    std::wstring result = str;

    // Trim trailing spaces
    size_t endPos = result.find_last_not_of(L" \t");
    if (std::wstring::npos != endPos) {
      result = result.substr(0, endPos + 1);
    }

    // Trim leading spaces
    size_t startPos = result.find_first_not_of(L" \t");
    if (std::wstring::npos != startPos) {
      result = result.substr(startPos);
    }

    return result;
  }

  bool equals(const std::wstring& str1, const std::wstring& str2) {
    return _wcsicmp(str1.c_str(), str2.c_str()) == 0;
  }

  bool parseBool(const std::wstring& str) {
    if (equals(str, L"true")) {
      return true;
    }

    if (equals(str, L"false")) {
      return false;
    }

    throw std::invalid_argument("connectionString");
  }

  int parseInt(const std::wstring& str) {
    _set_errno(0);
    int result = _wtoi(str.c_str());
    if (errno != 0) {
      throw std::invalid_argument("connectionString");
    }

    return result;
  }
}

void Cask::CdapOdbc::ConnectionParams::parse(const std::wstring& connectionString) {
  std::vector<std::wstring> params;
  std::vector<std::wstring> values;
  std::wstring key;

  split(connectionString, L';', params);
  for (auto& item : params) {
    values.clear();
    split(item, L'=', values);
    if (values.size() != 2) {
      throw std::invalid_argument("connectionString");
    }

    key = trim(values[0]);
    if (key.size() == 0) {
      throw std::invalid_argument("connectionString");
    }

    if (equals(key, L"driver")) {
      this->driver = trim(values[1]);
    } else if (equals(key, L"host")) {
      this->host = trim(values[1]);
    } else if (equals(key, L"port")) {
      this->port = parseInt(trim(values[1]));
    } else if (equals(key, L"auth_token")) {
      this->authToken = trim(values[1]);
    } else if (equals(key, L"namespace")) {
      this->namespace_ = trim(values[1]);
    } else if (equals(key, L"ssl_enabled")) {
      this->sslEnabled = parseBool(trim(values[1]));
    } else if (equals(key, L"verify_ssl_cert")) {
      this->verifySslCert = parseBool(trim(values[1]));
    }
  }

  if (this->host.size() == 0) {
    throw std::invalid_argument("connectionString");
  }
}

Cask::CdapOdbc::ConnectionParams::ConnectionParams(const std::wstring& connectionString)
  : driver()
  , host()
  , port(10000)
  , authToken()
  , namespace_(L"default")
  , sslEnabled(false)
  , verifySslCert(true) {
  this->parse(connectionString);
}

std::wstring Cask::CdapOdbc::ConnectionParams::getFullConnectionString() const {
  std::wstring result;

  if (this->driver.size() > 0) {
    result += L"Driver=";
    result += this->driver;
    result += L";";
  }

  result += L"Host=";
  result += this->host;
  result += L";";

  if (this->port > 0) {
    result += L"Port=";
    result += this->port;
    result += L";";
  }

  if (this->authToken.size() > 0) {
    result += L"Auth_Token=";
    result += this->authToken;
    result += L";";
  }

  if (!equals(this->namespace_, L"default")) {
    result += L"Namespace=";
    result += this->namespace_;
    result += L";";
  }

  if (sslEnabled) {
    result += L"SSL_Enabled=True;";
  }

  if (!verifySslCert) {
    result += L"Verify_SSL_Cert=False;";
  }

  return result;
}
