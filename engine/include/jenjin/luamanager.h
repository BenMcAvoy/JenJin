#pragma once

#include "jenjin/datastore.h"

#include <sol/forward.hpp>
#include <sol/sol.hpp>
#include <unordered_map>

namespace Jenjin {
class LuaManager {
public:
  LuaManager();

  // Used to manage loading and executing Lua scripts
  // from files and directories
  void LoadDirectory(const std::string &directory);
  void LoadFile(const std::string &file);
  void ReloadScripts(const std::string &directory);

  // One-shot Lua script execution
  void Execute(const std::string &script);
  void ExecuteFile(const std::string &file);

  // Utilities for calling Lua functions collected from files
  // NOTE: This is not related to one-shot `Execute`/`ExecuteFile`
  // calls. This is for `LoadFile`, `LoadDirectory`, etc.
  void Ready();
  void Update();

  // Getters
  sol::state *GetLuaState() { return &lua; }

  DataStore dataStore;

private:
  sol::state lua;

  std::unordered_map<std::string,
                     std::pair<sol::safe_function, sol::safe_function>>
      functions = {};
};
} // namespace Jenjin
