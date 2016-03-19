#include "stdafx.h"
#include "QuerySELECT1DataReader.h"

namespace Cask {
  namespace CdapOdbc {
    QuerySELECT1DataReader::QuerySELECT1DataReader()
    {
      Cask::CdapOdbc::ColumnDesc columnDesc = Cask::CdapOdbc::ColumnDesc();
      columnDesc.setName(L"_c0");
      columnDesc.setType(L"int");
      columnDesc.setPosition(0);
      columnDesc.setComment(L"");

      this->valueAvailable = true;
      this->schema = std::vector<ColumnDesc>();
      this->schema.push_back(columnDesc);
    }

    bool Cask::CdapOdbc::QuerySELECT1DataReader::read() {
      bool result = this->valueAvailable;
      if (this->valueAvailable) {
        this->valueAvailable = false;
      }
      return result;
    }

    void Cask::CdapOdbc::QuerySELECT1DataReader::getColumnValue(const ColumnBinding& binding) const {
      this->fetchValue(web::json::value::number(1), binding);
    }

    short Cask::CdapOdbc::QuerySELECT1DataReader::getColumnCount() const {
      return static_cast<short>(1);
    }

    std::unique_ptr<Cask::CdapOdbc::ColumnInfo> Cask::CdapOdbc::QuerySELECT1DataReader::getColumnInfo(short columnNumber) const {
      auto& columnDesc = this->schema[columnNumber - 1];
      return std::make_unique<ColumnInfo>(columnDesc);
    }

    bool Cask::CdapOdbc::QuerySELECT1DataReader::canReadFast() const {
      return true;
    }
  }
}

