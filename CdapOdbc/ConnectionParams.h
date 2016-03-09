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
     * Represents connection parameters parsed from connection string.
     */
    class ConnectionParams {
      std::wstring driver;
      std::wstring dsn;
      std::wstring host;
      int port;
      std::wstring authToken;
      std::wstring namespace_;
      bool sslEnabled;
      bool verifySslCert;

      void parse(const std::wstring& connectionString);

    public:

      /**
      * Creates an instance of connection params.
      */
      ConnectionParams();

      /**
       * Copy construdtor.
       */ 
      ConnectionParams(const ConnectionParams&);

      /**
      * Move construdtor.
      */
      ConnectionParams(ConnectionParams&&);

      /**
       * Creates an instance of connection params.
       */
      ConnectionParams(const std::wstring& connectionString);

      /**
       * Destructor.
       */
      ~ConnectionParams() = default;

      /**
       * Assignment copy operator.
       */
      void operator=(const ConnectionParams&);

      /**
       * Assignment move operator.
       */
      void operator=(ConnectionParams&&);

      /**
       * Gets DRIVER connection parameter.
       */
      const std::wstring& getDriver() const {
        return this->driver;
      }

      /**
       * Gets DSN connection parameter.
       */
      const std::wstring& getDsn() const {
        return this->dsn;
      }

      /**
       * Gets HOST connection parameter.
       */
      const std::wstring& getHost() const {
        return this->host;
      }

      /**
       * Gets PORT connection parameter.
       */
      int getPort() const {
        return this->port;
      }

      /**
       * Gets AUTH_TOKEN connection parameter.
       */
      const std::wstring& getAuthToken() const {
        return this->authToken;
      }

      /**
       * Gets NAMESPACE connection parameter.
       */
      const std::wstring& getNamespace() const {
        return this->namespace_;
      }

      /**
       * Gets SSL_ENABLED connection parameter.
       */
      bool getSslEnabled() const {
        return this->sslEnabled;
      }

      /**
       * Gets VERIFY_SSL_CERT connection parameter.
       */
      bool getVerifySslCert() const {
        return this->verifySslCert;
      }

      /**
       * Gets full version of connection string.
       */
      std::wstring getFullConnectionString() const;

      /**
       * Sets DRIVER connection parameter.
       */
      void setDriver(const std::wstring& value) {
        this->driver = value;
      }

      /**
       * Sets DSN connection parameter.
       */
      void setDsn(const std::wstring& value) {
        this->dsn = value;
      }

      /**
       * Sets HOST connection parameter.
       */
      void setHost(const std::wstring& value) {
        this->host = value;
      }

      /**
       * Sets PORT connection parameter.
       */
      void setPort(int value) {
        this->port = value;
      }

      /**
       * Sets AUTH_TOKEN connection parameter.
       */
      void setAuthToken(const std::wstring& value) {
        this->authToken = value;
      }

      /**
       * Sets NAMESPACE connection parameter.
       */
      void setNamespace(const std::wstring& value) {
        this->namespace_ = value;
      }

      /**
       * Sets SSL_ENABLED connection parameter.
       */
      void setSslEnabled(bool value) {
        this->sslEnabled = value;
      }

      /**
       * Sets VERIFY_SSL_CERT connection parameter.
       */
      void setVerifySslCert(bool value) {
        this->verifySslCert = value;
      }
    };
  }
}