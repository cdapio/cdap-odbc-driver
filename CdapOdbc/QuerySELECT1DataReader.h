#pragma once
#include "stdafx.h"
#include "DataReader.h"
#include "QueryResult.h"
#include "ColumnDesc.h"
#include "ConcurrentQueue.h"

namespace Cask {
  namespace CdapOdbc {
    class QuerySELECT1DataReader : public DataReader
    {

      bool valueAvailable;
      std::vector<ColumnDesc> schema;

    public:

      /**
      * Creates an instance of SELECT1QueryDataReader.
      */
      QuerySELECT1DataReader();

      // Inherited via DataReader
      virtual bool read() override;

      // Inherited via DataReader
      virtual void getColumnValue(const ColumnBinding& binding) const override;

      // Inherited via DataReader
      virtual short getColumnCount() const override;

      // Inherited via DataReader
      virtual std::unique_ptr<ColumnInfo> getColumnInfo(short columnNumber) const override;

      // Inherited via DataReader
      virtual bool canReadFast() const override;

//      virtual ~SELECT1QueryDataReader();
    };
  }
}

