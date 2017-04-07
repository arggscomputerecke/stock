#include "tclap/CmdLine.h"
#include <string>
#include "TFile.h"
#include "TTree.h"
#include <fstream>
#include <math.h>
#include "TDatime.h"


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
  if (j != string::npos && k != string::npos){
    lastslash = std::max(j, k);
  }
  else if (j != string::npos)
  {
    lastslash = j;
  }
  else if(k != string::npos){
    lastslash = k;
  }
  

  auto t =  buffer.substr(lastslash + 1,i-lastslash-1);
  return t;
}
int main(int argc, char **argv) {
  CmdLine cmd("ProcessFile", ' ', "0.1");
  ValueArg<std::string> FileNameArg("i", "inFile", "csv file", true, "", "string");
  cmd.add(FileNameArg);


  ValueArg<std::string> outFile("o", "outFile", "output ROOT file", true, "", "string");
  cmd.add(outFile);
  cmd.parse(argc, argv);  //terminates on error

  


  TFile outF(outFile.getValue().c_str(), "update");


  TTree tree(remove_path_and_extension(FileNameArg.getValue()).c_str(),"data"); 
  unsigned date = 0;
    double Open, High, Low, Close, Volume, AdjClose;


  tree.Branch("Date", &date);
  tree.Branch("Open", &Open);
  tree.Branch("High", &High);
  tree.Branch("Low", &Low);
  tree.Branch("Close", &Close);
  tree.Branch("Volume", &Volume);
  tree.Branch("AdjClose", &AdjClose);


  std::ifstream in(FileNameArg.getValue().c_str());

  std::string buffer;

  getline(in, buffer);

  while (in.good())
  {
    getline(in, buffer, ',');
    if (buffer.empty())
    {
      break;
    }
    date = date2int(buffer);
    
    getline(in, buffer, ',');
    Open = str2double(buffer);

    getline(in, buffer, ',');
    High= str2double(buffer);
    getline(in, buffer, ',');
    Low = str2double(buffer);
    getline(in, buffer, ',');
    Close = str2double(buffer);
    getline(in, buffer, ',');
    Volume = str2double(buffer);
    getline(in, buffer, '\n');
    AdjClose = str2double(buffer);

    tree.Fill();

  }
  tree.Write();
  outF.Write("", TObject::kOverwrite);
  outF.Close();
  return 0;
}