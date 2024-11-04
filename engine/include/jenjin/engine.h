#pragma once

#include "jenjin/log.h"
#include "jenjin/luamanager.h"
#include "jenjin/scene.h"
#include "jenjin/target.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Jenjin {
class Engine {
public:
  Engine(GLFWwindow *window);
  ~Engine();

  void AddScene(std::shared_ptr<Scene> scene, bool defaultScene = false);

  void Render(Target *target); // Render directly to the target

  GLFWwindow *glfwContext;

  Scene *GetCurrentScene() { return currentScene; }

  void SetLogSink(Jenjin::LogSink *sink);
  Jenjin::LogSink *GetLogSink() { return logSink; }

private:
  std::vector<std::shared_ptr<Scene>> scenes = {};
  Scene *currentScene = nullptr;

  LuaManager luaManager;

  Jenjin::LogSink *logSink = nullptr;
};

extern Engine *EngineRef;
} // namespace Jenjin
