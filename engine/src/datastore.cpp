#include "jenjin/datastore.h"

using namespace Jenjin;

void DataStore::DynamicSet(std::string key, sol::stack_object value) {
  auto it = entries.find(key);
  if (it == entries.cend()) {
    entries.insert(it, {std::move(key), std::move(value)});
  } else {
    std::pair<const std::string, sol::object> &kvp = *it;
    sol::object &entry = kvp.second;
    entry = sol::object(std::move(value));
  }
}

sol::object DataStore::DynamicGet(std::string key) {
  auto it = entries.find(key);
  if (it == entries.cend()) {
    return sol::lua_nil;
  }

  return it->second;
}
