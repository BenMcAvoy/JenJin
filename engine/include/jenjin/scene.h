#pragma once

#include "jenjin/camera.h"
#include "jenjin/gameobject.h"
#include "jenjin/luamanager.h"
#include "jenjin/shader.h"
#include "jenjin/target.h"
#include "jenjin/texture.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <memory>

namespace Jenjin {
class Scene {
public:
  Scene();
  ~Scene() { spdlog::trace("Scene::~Scene()"); }

  void SetTarget(Target *target);

  /*void AddGameObject(std::shared_ptr<GameObject> gameObject);*/
  /*void RemoveGameObject(GameObject *gameObject);*/
  /*void RemoveGameObject(std::shared_ptr<GameObject> gameObject);*/

  /*void SetGameObjectTexture(GameObject *gameObject,*/
  /*                          const std::string &texturePath);*/

  void AddGameObject(const std::string &name,
                     std::shared_ptr<GameObject> gameObject);
  void RemoveGameObject(std::string name);

  void RenameGameObject(std::string oldName, std::string newName);
  void RenameGameObject(std::shared_ptr<GameObject> gameObject,
                        std::string newName);

  void SetGameObjectTexture(std::shared_ptr<GameObject> gameObject,
                            const std::string &texturePath);

  void Build();

  void Update();
  void Render();

  std::shared_ptr<GameObject> GetGameObject(const std::string &name);

  Camera *GetCamera() { return &camera; }

  std::unordered_map<std::string, std::shared_ptr<GameObject>> &
  GetGameObjects() {
    return gameObjects;
  }

  Target *GetTarget() { return target; }
  LuaManager *GetLuaManager() { return &luaManager; }

  void Save(const std::string &path);
  void Save(std::ofstream &file);
  void Load(const std::string &path);
  void Load(std::ifstream &file);

private:
  GLuint vao, vbo, ebo = 0;

  std::unordered_map<std::string, std::shared_ptr<GameObject>> gameObjects;
  std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

  Shader shader = Shader("resources/shaders/default_vert.glsl",
                         "resources/shaders/default_frag.glsl");

  Target *target = nullptr;

  Camera camera = Camera(&shader, glm::vec2(800, 600));

  LuaManager luaManager;
};
} // namespace Jenjin
