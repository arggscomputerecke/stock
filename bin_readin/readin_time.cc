#include "reader_base.hh"
#include "TDatime.h"

unsigned date2int(const std::string& date) {
  int year = atoi(date.substr(0, 4).c_str());
  int month = atoi(date.substr(5, 2).c_str());
  int day = atoi(date.substr(8, 2).c_str());
  TDatime d(year, month, day, 0, 0, 0);
  return d.Get();
}

class readin_dates :public readinBase {
public:
  readin_dates(TTree * tree, const std::string& name, char delim = ',') :readinBase(tree, name, delim) {
    tree->Branch(remove_white_spaces(name).c_str(), &m_data);
  }
  virtual bool readBuffer(std::istream& in) {
    getline(in, m_buffer, m_delim);
    if (m_buffer.empty())
    {
      return false;
    }
    m_data = date2int(m_buffer);
    return true;
  };
  unsigned m_data;

};
namespace {
  register_reader<readin_dates> ___double("T");
}