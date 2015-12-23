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

  void split(const std::string& str, char delim, std::vector<std::string>& tokens);
  std::string trim(const std::string& str);
  bool equals(const std::string& str1, const std::string& str2);
  bool parseBool(const std::string& str);
  int parseInt(const std::string& str);

  void split(const std::string& str, char delim, std::vector<std::string>& tokens) {
    std::stringstream stream(str);
    std::string item;
    while (std::getline(stream, item, delim)) {
      tokens.push_back(item);
    }
  }

  std::string trim(const std::string& str) {
    std::string result = str;

    // Trim trailing spaces
    size_t endPos = result.find_last_not_of(" \t");
    if (std::string::npos != endPos) {
      result = result.substr(0, endPos + 1);
    }

    // Trim leading spaces
    size_t startPos = result.find_first_not_of(" \t");
    if (std::string::npos != startPos) {
      result = result.substr(startPos);
    }

    return result;
  }

  bool equals(const std::string& str1, const std::string& str2) {
    return _stricmp(str1.c_str(), str2.c_str()) == 0;
  }

  bool parseBool(const std::string& str) {
    if (equals(str, "true")) {
      return true;
    }

    if (equals(str, "false")) {
      return false;
    }

    throw std::invalid_argument("connectionString");
  }

  int parseInt(const std::string& str) {
    _set_errno(0);
    int result = atoi(str.c_str());
    if (errno != 0) {
      throw std::invalid_argument("connectionString");
    }

    return result;
  }
}

void Cask::CdapOdbc::ConnectionParams::parse(const std::string& connectionString) {
  std::vector<std::string> params;
  std::vector<std::string> values;
  std::string key;

  split(connectionString, ';', params);
  for (auto& item : params) {
    values.clear();
    split(item, '=', values);
    if (values.size() != 2) {
      throw std::invalid_argument("connectionString");
    }

    key = trim(values[0]);
    if (key.size() == 0) {
      throw std::invalid_argument("connectionString");
    }

    if (equals(key, "host")) {
      this->host = trim(values[1]);
    } else if (equals(key, "port")) {
      this->port = parseInt(trim(values[1]));
    } else if (equals(key, "auth_token")) {
      this->authToken = trim(values[1]);
    } else if (equals(key, "namespace")) {
      this->namespace_ = trim(values[1]);
    } else if (equals(key, "ssl_enabled")) {
      this->sslEnabled = parseBool(trim(values[1]));
    } else if (equals(key, "verify_ssl_cert")) {
      this->verifySslCert = parseBool(trim(values[1]));
    }
  }

  if (this->host.size() == 0) {
    throw std::invalid_argument("connectionString");
  }
}

Cask::CdapOdbc::ConnectionParams::ConnectionParams(const std::string& connectionString)
  : host()
  , port(10000)
  , authToken()
  , namespace_("default")
  , sslEnabled(false)
  , verifySslCert(true) {
  this->parse(connectionString);
}

std::string Cask::CdapOdbc::ConnectionParams::getFullConnectionString() const {
  std::string result;

  result += "Host=";
  result += this->host;
  result += ";";

  if (this->port > 0) {
    result += "Port=";
    result += this->port;
    result += ";";
  }

  if (this->authToken.size() > 0) {
    result += "Auth_Token=";
    result += this->authToken;
    result += ";";
  }

  if (!equals(this->namespace_, "default")) {
    result += "Namespace=";
    result += this->namespace_;
    result += ";";
  }

  if (sslEnabled) {
    result += "SSL_Enabled=True;";
  }

  if (!verifySslCert) {
    result += "Verify_SSL_Cert=False;";
  }

  return result;
}
