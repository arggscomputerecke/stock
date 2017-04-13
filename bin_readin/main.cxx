#include "tclap/CmdLine.h"
#include <string>
#include "TFile.h"
#include "TTree.h"
#include <fstream>
#include <math.h>
#include "TDatime.h"
#include <memory>
#include <algorithm>


using namespace std;
using namespace TCLAP;


unsigned date2int(const std::string& date) {
  int year = atoi(date.substr(0, 4).c_str());
  int month = atoi(date.substr(5, 2).c_str());
  int day = atoi(date.substr(8, 2).c_str());
  TDatime d(year, month, day, 0, 0, 0);
  return d.Get();
}

double str2double(const std::string& date) {
  return atof(date.c_str());

}
std::string remove_path_and_extension(const std::string& buffer) {
  auto i = buffer.find_last_of('.');
  auto j = buffer.find_last_of('/');
  auto k = buffer.find_last_of('\\');

  int lastslash = 0;
  if (j != string::npos && k != string::npos) {
    lastslash = std::max(j, k);
  }
  else if (j != string::npos)
  {
    lastslash = j;
  }
  else if (k != string::npos) {
    lastslash = k;
  }


  auto t = buffer.substr(lastslash + 1, i - lastslash - 1);
  return t;
}
std::string remove_white_spaces(const std::string& name) {
  std::string ret(name);
  std::replace(ret.begin(), ret.end(), ' ', '_'); 
  return ret;
}

class readinBase {
public:
  readinBase(TTree* tree, const std::string& name, char delim) :m_tree(tree), m_name(name), m_delim(delim) {}
  virtual bool readBuffer(std::istream&) = 0;
  TTree *m_tree;
  std::string m_buffer;
  std::string m_name;
  const char m_delim;
};

typedef readinBase*(*reader_creator)(TTree * tree, const std::string& name, char delim);
using r_map = std::map<std::string, reader_creator>;

r_map& get_register() {
  static r_map map;
  return map;
}

void register_readin(const std::string& type, reader_creator fun) {
  get_register()[type] = fun;
}
std::shared_ptr<readinBase> create_readin(const std::string& type, TTree * tree, const std::string& name, char delim = ',') {
  return std::shared_ptr<readinBase>(  get_register()[type](tree, name, delim));
}

class readin_doubles :public readinBase {
public:
  readin_doubles(TTree * tree, const std::string& name, char delim = ',') :readinBase(tree,name, delim) {
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
  readinBase* create_readin_doubles(TTree * tree, const std::string& name, char delim) {
    return new readin_doubles(tree, name, delim);
  }
  class __register_readin_doubles {
  public:
    __register_readin_doubles() {
      register_readin("D", &create_readin_doubles);
    }
  } ___regD;
}


class readin_dates :public readinBase {
public:
  readin_dates(TTree * tree, const std::string& name, char delim = ',') :readinBase(tree,name, delim) {
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
  readinBase* create_readin_dates(TTree * tree, const std::string& name, char delim) {
    return new readin_dates(tree, name, delim);
  }
  class __register_readin_dates {
  public:
    __register_readin_dates() {
      register_readin("T", &create_readin_dates);
    }
  } ___regT;
}




std::vector<std::string> split(const std::string& input, char delimiter = ',') {
  vector<string> result;
  std::stringstream ss;
  ss.str(input);
  std::string item;
  while (std::getline(ss, item, delimiter)) {
    if (item.empty())
    {
      continue;
    }
   result.push_back(item);
  }
  return result;

}

std::vector<std::shared_ptr<readinBase>> create_processors(const std::string& header, const std::string& formating,TTree *tree) {
  
  
  auto s_formating = split(formating,'%');
  s_formating.back() += '\n';
  
  auto s_header = split(header, s_formating[0].back());

  if (s_formating.size()!=s_header.size()){
    throw std::exception("size of header and size of formating string not identical");
  }
 
  std::vector<std::shared_ptr<readinBase>> processors;
  for (int i =0 ;i<s_formating.size();++i)
  {
    processors.push_back(create_readin(s_formating[i].substr(0,1), tree, s_header.at(i), s_formating[i].back()));
  }
  return processors;
}

int main(int argc, char **argv) {
  CmdLine cmd("ProcessFile", ' ', "0.1");
  ValueArg<std::string> FileNameArg("i", "inFile", "csv file", true, "", "string");
  cmd.add(FileNameArg);


  ValueArg<std::string> outFile("o", "outFile", "output ROOT file", true, "", "string");
  cmd.add(outFile);

  ValueArg<std::string> str_format("f", "formating", "gives an indication on how to interpret the individual columns", true, "", "string");
  cmd.add(str_format);
  cmd.parse(argc, argv);  //terminates on error

  TFile outF(outFile.getValue().c_str(), "Recreate");


  TTree tree(remove_path_and_extension(FileNameArg.getValue()).c_str(), "data");


  


  std::ifstream in(FileNameArg.getValue().c_str());

  std::string buffer;
  getline(in, buffer);
  auto s = split(buffer);

  auto processors = create_processors(buffer, str_format.getValue(),&tree);

  int i = 0;
  while (in.good())
  {
    for (auto&e:processors)
    {
      e->readBuffer(in);
    }
    i++;
    tree.Fill();

  }
  tree.Write();
  outF.Write("", TObject::kOverwrite);
  outF.Close();
  return 0;
}