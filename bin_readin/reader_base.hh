#ifndef reader_base_h__
#define reader_base_h__
#include <string>
#include <map>
#include <memory>
#include "TTree.h"


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

r_map& get_register();

template<typename T>
readinBase* __create_readinBase(TTree * tree, const std::string& name, char delim) {
  return new T(tree, name, delim);
}

template<typename T>
class register_reader {
public:
  register_reader(const std::string& type) {
    get_register()[type] = &__create_readinBase<T>;
  }

};

void register_readin(const std::string& type, reader_creator fun);
std::shared_ptr<readinBase> create_readin(const std::string& type, TTree * tree, const std::string& name, char delim = ',');

std::string remove_white_spaces(const std::string& name);
#endif // reader_base_h__
