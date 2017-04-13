#include "tclap/CmdLine.h"
#include <string>
#include "TFile.h"
#include "TTree.h"
#include <fstream>
#include <math.h>
#include "TDatime.h"
#include <memory>


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

class readinBase {
public:
  readinBase(TTree* tree, char delim) :m_tree(tree), m_delim(delim) {}
  virtual bool readBuffer(std::istream&) = 0;
  TTree *m_tree;
  std::string m_buffer;
  const char m_delim;
};

class readin_doubles :public readinBase {
public:
  readin_doubles(TTree * tree, const char* name, char delim = ',') :readinBase(tree, delim) {
    tree->Branch(name, &m_data);
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

class readin_dates :public readinBase {
public:
  readin_dates(TTree * tree, const char* name, char delim = ',') :readinBase(tree, delim) {
    tree->Branch(name, &m_data);
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

int main(int argc, char **argv) {
  CmdLine cmd("ProcessFile", ' ', "0.1");
  ValueArg<std::string> FileNameArg("i", "inFile", "csv file", true, "", "string");
  cmd.add(FileNameArg);


  ValueArg<std::string> outFile("o", "outFile", "output ROOT file", true, "", "string");
  cmd.add(outFile);
  cmd.parse(argc, argv);  //terminates on error




  TFile outF(outFile.getValue().c_str(), "Recreate");


  TTree tree(remove_path_and_extension(FileNameArg.getValue()).c_str(), "data");

  std::vector<std::shared_ptr<readinBase>> processors;
  processors.push_back( std::make_shared<readin_dates>( &tree,"Date" ));
  processors.push_back(std::make_shared<readin_doubles>(&tree, "Open"));
  processors.push_back(std::make_shared<readin_doubles>(&tree, "High"));
  processors.push_back(std::make_shared<readin_doubles>(&tree, "Low"));
  processors.push_back(std::make_shared<readin_doubles>(&tree, "Close"));
  processors.push_back(std::make_shared<readin_doubles>(&tree, "Volume"));
  processors.push_back(std::make_shared<readin_doubles>(&tree, "AdjClose",'\n'));
  


  std::ifstream in(FileNameArg.getValue().c_str());

  std::string buffer;

  getline(in, buffer);

  while (in.good())
  {
    for (auto&e:processors)
    {
      e->readBuffer(in);
    }

    tree.Fill();

  }
  tree.Write();
  outF.Write("", TObject::kOverwrite);
  outF.Close();
  return 0;
}