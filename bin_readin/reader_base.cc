#include "reader_base.hh"
#include <algorithm>

r_map& get_register()
{
  static r_map map;
  return map;
}

void register_readin(const std::string& type, reader_creator fun)
{
  get_register()[type] = fun;
}

std::shared_ptr<readinBase> create_readin(const std::string& type, TTree * tree, const std::string& name, char delim /*= ','*/)
{
  return std::shared_ptr<readinBase>(get_register()[type](tree, name, delim));
}

std::string remove_white_spaces(const std::string& name)
{
  std::string ret(name);
  std::replace(ret.begin(), ret.end(), ' ', '_');
  return ret;
}
