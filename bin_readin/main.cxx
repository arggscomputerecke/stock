#include "tclap/CmdLine.h"
#include <string>
#include "TFile.h"
#include "TTree.h"
#include <fstream>
#include <math.h>
#include "TDatime.h"
#include <memory>
#include <algorithm>
#include "reader_base.hh"

using namespace std;
using namespace TCLAP;





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


  auto processors = create_processors(buffer, str_format.getValue(),&tree);

  int i = 0;
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