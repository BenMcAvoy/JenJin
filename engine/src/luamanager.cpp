#define GLFW_INCLUDE_NONE

#include "jenjin/luamanager.h"
#include "jenjin/engine.h"
#include "jenjin/helpers.h"

#include <glm/ext/quaternion_common.hpp>
#include <glm/glm.hpp>

#include <sol/raii.hpp>
#include <sol/state_handling.hpp>
#include <sol/variadic_args.hpp>

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

using namespace Jenjin;

void Bindings(LuaManager *lm, sol::state &lua) {
  // ========================================================================
  // Logging
  // ========================================================================

  // This is a lambda function that takes a variadic number of arguments
  // and constructs a string from them to be used in the logging functions
  // It takes in basic types and handles some bound userdata types>

  auto construct_string = [&](sol::variadic_args va) {
    std::stringstream ss;

    int i = 0;
    for (auto v : va) {
      if (v.get_type() == sol::type::number) {
        ss << v.get<int>();
      } else if (v.get_type() == sol::type::string) {
        ss << v.get<std::string>();
      } else if (v.get_type() == sol::type::boolean) {
        ss << (v.get<bool>() ? "true" : "false");
      } else if (v.get_type() == sol::type::nil) {
        ss << "nil";
      } else if (v.get_type() == sol::type::userdata) {
        auto ud = v.get<sol::userdata>();
        if (ud.is<glm::vec2>()) {
          auto vec = ud.as<glm::vec2>();
          ss << "vec2(" << vec.x << ", " << vec.y << ")";
        } else if (ud.is<glm::vec3>()) {
          auto vec = ud.as<glm::vec3>();
          ss << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        } else {
          ss << "unknown userdata";
        }
      }

      // Add a comma if there are more arguments
      if (i++ < va.size() - 1) {
        ss << ", ";
      }
    }

    return ss.str();
  };

  lua.set_function("trace", [&](sol::variadic_args va) {
    spdlog::trace("[LUA] {}", construct_string(va));
  });

  lua.set_function("debug", [&](sol::variadic_args va) {
    spdlog::debug("[LUA] {}", construct_string(va));
  });

  lua.set_function("print", [&](sol::variadic_args va) {
    spdlog::info("[LUA] {}", construct_string(va));
  });

  lua.set_function("info", [&](sol::variadic_args va) {
    spdlog::info("[LUA] {}", construct_string(va));
  });

  lua.set_function("warn", [&](sol::variadic_args va) {
    spdlog::warn("[LUA] {}", construct_string(va));
  });

  lua.set_function("error", [&](sol::variadic_args va) {
    spdlog::error("[LUA] {}", construct_string(va));
  });

  lua.set_function("critical", [&](sol::variadic_args va) {
    spdlog::critical("[LUA] {}", construct_string(va));
  });

  // ========================================================================
  // Userdata
  // ========================================================================

  lua.new_usertype<glm::vec2>(
      "vec2", sol::constructors<glm::vec2(), glm::vec2(float, float)>(), "x",
      &glm::vec2::x, "y", &glm::vec2::y,

      sol::meta_function::addition,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs + rhs; },
          [](const glm::vec2 &lhs, float rhs) { return lhs + rhs; },
          [](float lhs, const glm::vec2 &rhs) { return lhs + rhs; }),

      sol::meta_function::subtraction,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs - rhs; },
          [](const glm::vec2 &lhs, float rhs) { return lhs - rhs; },
          [](float lhs, const glm::vec2 &rhs) { return lhs - rhs; }),

      sol::meta_function::multiplication,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs * rhs; },
          [](const glm::vec2 &lhs, float rhs) { return lhs * rhs; },
          [](float lhs, const glm::vec2 &rhs) { return lhs * rhs; }),

      sol::meta_function::division,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs / rhs; },
          [](const glm::vec2 &lhs, float rhs) { return lhs / rhs; },
          [](float lhs, const glm::vec2 &rhs) { return lhs / rhs; }),

      sol::meta_function::equal_to,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs == rhs; },
          [](const glm::vec2 &lhs, float rhs) { return lhs == glm::vec2(rhs); },
          [](float lhs, const glm::vec2 &rhs) {
            return glm::vec2(lhs) == rhs;
          }),

      sol::meta_function::less_than,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) {
            return (lhs.x < rhs.x) && (lhs.y < rhs.y);
          },
          [](const glm::vec2 &lhs, float rhs) {
            return (lhs.x < rhs) && (lhs.y < rhs);
          },
          [](float lhs, const glm::vec2 &rhs) {
            return (lhs < rhs.x) && (lhs < rhs.y);
          }),

      sol::meta_function::less_than_or_equal_to,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) {
            return (lhs.x <= rhs.x) && (lhs.y <= rhs.y);
          },
          [](const glm::vec2 &lhs, float rhs) {
            return (lhs.x <= rhs) && (lhs.y <= rhs);
          },
          [](float lhs, const glm::vec2 &rhs) {
            return (lhs <= rhs.x) && (lhs <= rhs.y);
          }),

      sol::meta_function::to_string,
      [](const glm::vec2 &vec) {
        std::stringstream ss;
        ss << "vec2(" << vec.x << ", " << vec.y << ")";
        return ss.str();
      },

      sol::meta_function::index,
      [](const glm::vec2 &vec, const std::string &key) {
        if (key == "x") {
          return vec.x;
        } else if (key == "y") {
          return vec.y;
        } else if (key == "r") {
          return vec.x;
        } else if (key == "g") {
          return vec.y;
        } else if (key == "s") {
          return vec.x;
        } else if (key == "t") {
          return vec.y;
        } else {
          return -1.0f;
        }
      },

      "Distance",
      [](const glm::vec2 &lhs, const glm::vec2 &rhs) {
        return glm::distance(lhs, rhs);
      });

  // glm::vec3
  lua.new_usertype<glm::vec3>(
      "vec3", sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
      "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z,

      // Operators
      // +, -, *, /, ==, <, >, <=, >= are our targets
      sol::meta_function::addition,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) { return lhs + rhs; },
          [](const glm::vec3 &lhs, float rhs) { return lhs + rhs; },
          [](float lhs, const glm::vec3 &rhs) { return lhs + rhs; }),

      sol::meta_function::subtraction,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) { return lhs - rhs; },
          [](const glm::vec3 &lhs, float rhs) { return lhs - rhs; },
          [](float lhs, const glm::vec3 &rhs) { return lhs - rhs; }),

      sol::meta_function::multiplication,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) { return lhs * rhs; },
          [](const glm::vec3 &lhs, float rhs) { return lhs * rhs; },
          [](float lhs, const glm::vec3 &rhs) { return lhs * rhs; }),

      sol::meta_function::division,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) { return lhs / rhs; },
          [](const glm::vec3 &lhs, float rhs) { return lhs / rhs; },
          [](float lhs, const glm::vec3 &rhs) { return lhs / rhs; }),

      sol::meta_function::equal_to,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) { return lhs == rhs; },
          [](const glm::vec3 &lhs, float rhs) { return lhs == glm::vec3(rhs); },
          [](float lhs, const glm::vec3 &rhs) {
            return glm::vec3(lhs) == rhs;
          }),

      sol::meta_function::less_than,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) {
            return (lhs.x < rhs.x) && (lhs.y < rhs.y) && (lhs.z < rhs.z);
          },
          [](const glm::vec3 &lhs, float rhs) {
            return (lhs.x < rhs) && (lhs.y < rhs) && (lhs.z < rhs);
          },
          [](float lhs, const glm::vec3 &rhs) {
            return (lhs < rhs.x) && (lhs < rhs.y) && (lhs < rhs.z);
          }),

      sol::meta_function::less_than_or_equal_to,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) {
            return (lhs.x <= rhs.x) && (lhs.y <= rhs.y) && (lhs.z <= rhs.z);
          },
          [](const glm::vec3 &lhs, float rhs) {
            return (lhs.x <= rhs) && (lhs.y <= rhs) && (lhs.z <= rhs);
          },
          [](float lhs, const glm::vec3 &rhs) {
            return (lhs <= rhs.x) && (lhs <= rhs.y) && (lhs <= rhs.z);
          }),

      sol::meta_function::to_string,
      [](const glm::vec3 &vec) {
        std::stringstream ss;
        ss << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return ss.str();
      },

      sol::meta_function::index,
      [](const glm::vec3 &vec, const std::string &key) {
        if (key == "x") {
          return vec.x;
        } else if (key == "y") {
          return vec.y;
        } else if (key == "z") {
          return vec.z;
        } else if (key == "r") {
          return vec.x;
        } else if (key == "g") {
          return vec.y;
        } else if (key == "b") {
          return vec.z;
        } else if (key == "s") {
          return vec.x;
        } else if (key == "t") {
          return vec.y;
        } else if (key == "p") {
          return vec.z;
        } else {
          return -1.0f;
        }
      },

      "Distance",
      [](const glm::vec3 &lhs, const glm::vec3 &rhs) {
        return glm::distance(lhs, rhs);
      });

  // ========================================================================
  // Math
  // ========================================================================
  lua.set_function("dot", [](const glm::vec2 &lhs, const glm::vec2 &rhs) {
    return glm::dot(lhs, rhs);
  });

  lua.set_function("dot", [](const glm::vec3 &lhs, const glm::vec3 &rhs) {
    return glm::dot(lhs, rhs);
  });

  lua.set_function("cross", [](const glm::vec3 &lhs, const glm::vec3 &rhs) {
    return glm::cross(lhs, rhs);
  });

  lua.set_function("normalize",
                   [](const glm::vec2 &vec) { return glm::normalize(vec); });

  lua.set_function("normalize",
                   [](const glm::vec3 &vec) { return glm::normalize(vec); });

  lua.set_function("length",
                   [](const glm::vec2 &vec) { return glm::length(vec); });

  lua.set_function("length",
                   [](const glm::vec3 &vec) { return glm::length(vec); });

  lua.set_function("distance", [](const glm::vec2 &lhs, const glm::vec2 &rhs) {
    return glm::distance(lhs, rhs);
  });

  lua.set_function("distance", [](const glm::vec3 &lhs, const glm::vec3 &rhs) {
    return glm::distance(lhs, rhs);
  });

  // ========================================================================
  // Jenjin userdata
  // ========================================================================

  lua.new_usertype<Scene>("Scene", sol::no_constructor, "AddGameObject",
                          &Scene::AddGameObject, "GetGameObject",
                          &Scene::GetGameObject, "SetGameObjectTexture",
                          &Scene::SetGameObjectTexture, "RemoveGameObject",
                          &Scene::RemoveGameObject);

  lua.new_usertype<GameObject>(
      /*"GameObject", sol::constructors<GameObject()>(),*/
      "GameObject",
      sol::factories([]() { return std::make_shared<GameObject>(); }),

      "GetPosition", &GameObject::GetPosition, "GetScale",
      &GameObject::GetScale, "GetRotation", &GameObject::GetRotation,
      "SetPosition", &GameObject::SetPosition, "SetScale",
      &GameObject::SetScale, "SetRotation", &GameObject::SetRotation,
      "GetColor", &GameObject::GetColor, "SetColor", &GameObject::SetColor,
      "Translate", &GameObject::Translate, "Scale", &GameObject::Scale,
      "Rotate", &GameObject::Rotate,

      sol::meta_function::index,
      [](GameObject &gobj, const std::string &key) {
        return gobj.dataStore.DynamicGet(key);
      },

      sol::meta_function::new_index,
      [](GameObject &gobj, const std::string &key, sol::stack_object value) {
        gobj.dataStore.DynamicSet(key, std::move(value));
      },

      sol::meta_function::length,
      [](GameObject &gobj) { return gobj.dataStore.entries.size(); });

  lua.new_usertype<GameObject::Transform>(
      "Transform",
      sol::constructors<GameObject::Transform(),
                        GameObject::Transform(glm::vec2, glm::vec2, float)>(),
      "position", &GameObject::Transform::position, "scale",
      &GameObject::Transform::scale, "rotation",
      &GameObject::Transform::rotation);

  lua.new_usertype<Engine>("Engine", sol::no_constructor, "GetCurrentScene",
                           &Engine::GetCurrentScene);

  // ========================================================================
  // Utility tables
  // ========================================================================
  auto input_tb = lua.create_named_table("input");
  input_tb.set_function("GetMousePosition", [&]() {
    if (!EngineRef) {
      spdlog::error("[LUA] No Jenjin instance initialized while calling "
                    "`GetMousePosition`");
      return glm::vec2(-1, -1);
    }

    auto scene = EngineRef->GetCurrentScene();
    if (!scene) {
      spdlog::error("[LUA] No current scene while calling `GetMousePosition`");
      return glm::vec2(-1, -1);
    }

    auto target = scene->GetTarget();
    if (!scene) {
      spdlog::error("[LUA] No target while calling `get_key_down`");
      return glm::vec2(-1, -1);
    }

    return target->GetMousePosition();
  });

  input_tb.set_function("IsKeyDown", [&](const std::string &key) {
    static std::unordered_map<std::string, int> keys = {};

    if (keys.empty()) {
      spdlog::info("[LUA] Initializing key map");

      for (int i = 0; i < GLFW_KEY_LAST; i++) {
        auto keyName = glfwGetKeyName(i, 0);
        if (keyName) {
          keys[keyName] = i;
        }
      }
    }

    auto context = glfwGetCurrentContext();

    if (context) {
      char *key_lower = (char *)key.c_str();
      std::transform(key_lower, key_lower + strlen(key_lower), key_lower,
                     ::tolower);
      return glfwGetKey(context, keys[key_lower]) == GLFW_PRESS;
    } else {
      spdlog::error("[LUA] No current context while calling `GetKeyDown`");
      return false;
    }
  });

  // Globals table (this->dataStore), it needs to be bound to a `globals`
  // variable in the lua script and have a metatable that allows indexing and
  // new indexing of the table
  lua.new_usertype<DataStore>(
      "DataStore", sol::no_constructor, sol::meta_function::index,
      [](DataStore &ds, const std::string &key) { return ds.DynamicGet(key); },
      sol::meta_function::new_index,
      [](DataStore &ds, const std::string &key, sol::stack_object value) {
        ds.DynamicSet(key, std::move(value));
      },
      sol::meta_function::length,
      [](DataStore &ds) { return ds.entries.size(); });
  lua.set("globals", lm->dataStore);

  auto helpers_tb = lua.create_named_table("helpers");
  (void)helpers_tb; // TODO: Add helper functions (used to have mesh creation
                    // functions, but custom meshes are no longer supported
                    // as rects are enough)
}

LuaManager::LuaManager() {
  lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table,
                     sol::lib::math);

  Bindings(this, lua);
}

void LuaManager::LoadDirectory(const std::string &directory) {
  for (const auto &entry : std::filesystem::directory_iterator(directory)) {
    if (entry.is_regular_file() && entry.path().extension() == ".lua") {
      LoadFile(entry.path().string());
    }
  }
}

void LuaManager::LoadFile(const std::string &file) {
  spdlog::info("[LUA] Loading file {}", file);

  auto res = lua.safe_script_file(file, sol::script_pass_on_error);

  if (!res.valid()) {
    sol::error err = res;
    spdlog::error("[LUA] {}", err.what());
  }

  sol::optional<sol::function> readyt =
      lua.get<sol::optional<sol::function>>("READY");
  sol::optional<sol::function> updatet =
      lua.get<sol::optional<sol::function>>("UPDATE");

  if (!readyt || !updatet) {
    spdlog::error("[LUA] `READY` or `UPDATE` function not found in {}", file);
    return;
  }

  auto ready = readyt.value();
  auto update = updatet.value();

  if (ready.valid() && ready.get_type() == sol::type::function &&
      update.valid() && update.get_type() == sol::type::function) {
    functions[file] = {ready, update};
  }
}

void LuaManager::ReloadScripts(const std::string &directory) {
  functions.clear();
  this->LoadDirectory(directory);
}

void LuaManager::Execute(const std::string &script) {
  auto res = lua.safe_script(script, sol::script_pass_on_error);

  if (!res.valid()) {
    sol::error err = res;
    spdlog::error("[LUA] {}", err.what());
  }
}

void LuaManager::ExecuteFile(const std::string &file) {
  auto res = lua.safe_script_file(file, sol::script_pass_on_error);

  if (!res.valid()) {
    sol::error err = res;
    spdlog::error("[LUA] {}", err.what());
  }
}

void LuaManager::Ready() {
  lua["scene"] = EngineRef->GetCurrentScene();
  lua["target"] = EngineRef->GetCurrentScene()->GetTarget();
  lua["engine"] = EngineRef;

  for (auto &func : functions) {
    auto &ready = func.second.first;
    auto &update = func.second.second;

    if (ready) {
      auto res = ready();

      if (!res.valid()) {
        sol::error err = res;
        spdlog::error("[LUA] {}", err.what());
      }
    }
  }
}

void LuaManager::Update() {
  for (auto &func : functions) {
    auto &ready = func.second.first;
    auto &update = func.second.second;

    if (update) {
      auto res = update();

      if (!res.valid()) {
        sol::error err = res;
        spdlog::error("[LUA] {}", err.what());
      }
    }
  }
}
