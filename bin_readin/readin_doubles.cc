#include "reader_base.hh"

double str2double(const std::string& date) {
  return atof(date.c_str());
}

class readin_doubles :public readinBase {
public:
  readin_doubles(TTree * tree, const std::string& name, char delim = ',') :readinBase(tree, name, delim) {
    tree->Branch(remove_white_spaces(name).c_str(), &m_data);
  }
  virtual bool readBuffer(std::istream& in) {
    getline(in, m_buffer, m_delim);
    if (m_buffer.empty())
    {
      return false;
    }
    m_data = str2double(m_buffer);
  };
  double m_data;

};
namespace {
  register_reader<readin_doubles> ___double("D");
}
