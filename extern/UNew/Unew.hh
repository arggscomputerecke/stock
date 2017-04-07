#ifndef Unew_h__
#define Unew_h__

#include <memory>
#include <chrono>
#include <iostream>
#define Unew __unew_start << new 
enum _unew_ {
  __unew_start
};

template <typename T>
auto operator<<(_unew_, T* in_) {
  return std::unique_ptr<T>(in_);
}

#endif // Unew_h__
