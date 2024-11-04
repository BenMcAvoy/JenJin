#pragma once

#include <sol/sol.hpp>

#include <string>
#include <unordered_map>

namespace Jenjin {
struct DataStore {
  std::unordered_map<std::string, sol::object> entries;

  void DynamicSet(std::string key, sol::stack_object value);
  sol::object DynamicGet(std::string key);
};
} // namespace Jenjin
