#define GLFW_INCLUDE_NONE

#include "jenjin/scene.h"
#include "jenjin/gameobject.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

#include <spdlog/spdlog.h>

#include <fstream>
#include <iostream>

using namespace Jenjin;

// Vertex data: position (x, y, z) and texture coordinates (u, v)
const float rect_verts[20] = {
    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // TL
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // TR
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // BL
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f  // BR
};

const unsigned short rect_indices[6] = {
    0, 1, 2, // TL, TR, BL
    1, 2, 3, // TR, BR, BL
};

Scene::Scene() {
  spdlog::trace("Scene::Scene()");

  spdlog::debug("Generating new buffers");
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rect_verts), rect_verts, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rect_indices), rect_indices,
               GL_STATIC_DRAW);

  // Set up position attribute (x, y, z)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
  glEnableVertexAttribArray(0);

  // Set up texture coordinate attribute (u, v)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5,
                        (void *)(sizeof(float) * 3));
  glEnableVertexAttribArray(1);

  spdlog::debug("Generated buffers (vao: {}, vbo: {}, ebo: {})", vao, vbo, ebo);
}

void Scene::SetTarget(Target *target) {
  spdlog::trace("Scene::SetTarget({})", (void *)target);

  this->target = target;
  this->camera.Resize(target->GetSize());
}

void Scene::AddGameObject(const std::string &name,
                          std::shared_ptr<GameObject> gameObject) {
  gameObjects.emplace(name, gameObject);
}

void Scene::RemoveGameObject(std::string name) { gameObjects.erase(name); }

void Scene::RenameGameObject(std::string oldName, std::string newName) {
  gameObjects[newName] = gameObjects[oldName];
  gameObjects.erase(oldName);
}

void Scene::RenameGameObject(std::shared_ptr<GameObject> gameObject,
                             std::string newName) {
  gameObjects[newName] = gameObject;

  // HACK: This is a hack to rename the game object in the editor
  // fix this properly
  for (auto &[name, go] : gameObjects) {
    if (go.get() == gameObject.get()) {
      gameObjects.erase(name);
      gameObjects[newName] = gameObject;
      break;
    }
  }
}

void Scene::SetGameObjectTexture(std::shared_ptr<GameObject> gameObject,
                                 const std::string &texturePath) {
  if (texturePath.empty()) {
    gameObject->texturePath = "";
    return;
  }

  if (this->textures.find(texturePath) == this->textures.end()) {
    auto alpha = texturePath.find(".png") != std::string::npos;
    this->textures[texturePath] =
        std::make_shared<Texture>(texturePath.c_str(), alpha);
  }

  gameObject->texturePath = texturePath;
}

void Scene::Update() { this->luaManager.Update(); }

void Scene::Render() {
  this->shader.use();
  this->camera.Use();
  this->camera.Update();
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, ebo);

  for (auto &[name, gameObject] : gameObjects) {
    glm::mat4 model = glm::mat4(1.0f);

    model =
        glm::translate(model, glm::vec3(gameObject->transform.position, 0.0f));

    model = glm::rotate(model, glm::radians(-gameObject->transform.rotation),
                        glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, glm::vec3(gameObject->transform.scale, 1.0f));

    shader.set("u_model", model);
    shader.set("u_color", gameObject->color);

    if (!gameObject->texturePath.empty()) {
      this->textures[gameObject->texturePath]->bind(0);
      shader.set("u_texture", 0);
    }

    bool hasTexture = !gameObject->texturePath.empty();
    shader.set("u_hasTexture", hasTexture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
  }
}

std::shared_ptr<GameObject> Scene::GetGameObject(const std::string &name) {
  if (this->gameObjects.find(name) != this->gameObjects.end()) {
    return this->gameObjects[name];
  } else {
    return nullptr;
  }
}

struct GOBJSAVABLE {
  Jenjin::GameObject::Transform transform;
  glm::vec3 color;

  char texturePath[128];
  char name[128];
};

void Scene::Save(const std::string &path) {
  spdlog::trace("Scene::Save({})", path);

  std::ofstream file(path, std::ios::binary);
  Save(file);
}

void Scene::Save(std::ofstream &file) {
  spdlog::trace("Scene::Save({})", (void *)&file);

  if (this->gameObjects.empty()) {
    spdlog::debug("No game objects to save");
    return;
  }

  for (auto &[name, go] : this->gameObjects) {
    GOBJSAVABLE gobj = {.transform = go->transform,
                        .color = go->color,

                        .texturePath = {0},
                        .name = {0}};

    strncpy(gobj.name, name.c_str(), sizeof(gobj.name));
    auto path = go->texturePath;
    auto pos = path.find("textures/");
    if (pos != std::string::npos) {
      const char *text = path.c_str() + pos;
      spdlog::debug("Texture path is in the textures directory: {}", text);
      strncpy(gobj.texturePath, text, sizeof(gobj.texturePath));
    } else {
      spdlog::warn("Texture path is not in the textures directory: {}", path);
      strncpy(gobj.texturePath, path.c_str(), sizeof(gobj.texturePath));
    }

    gobj.name[sizeof(gobj.name) - 1] = 0;
    gobj.texturePath[sizeof(gobj.texturePath) - 1] = 0;

    file.write(reinterpret_cast<char *>(&gobj), sizeof(GOBJSAVABLE));
  }
}

void Scene::Load(const std::string &path) {
  spdlog::trace("Scene::Load({})", path);

  std::ifstream file(path, std::ios::binary);
  this->Load(file);
}

void Scene::Load(std::ifstream &file) {
  spdlog::trace("Scene::Load({})", (void *)&file);

  file.seekg(0, std::ios::end);
  int size = file.tellg();
  file.seekg(0, std::ios::beg);

  spdlog::debug("File size: {}", size);

  this->gameObjects.clear();

  for (GOBJSAVABLE gobj;
       file.read(reinterpret_cast<char *>(&gobj), sizeof(GOBJSAVABLE));) {
    spdlog::debug("Reading GOBJSAVABLE from file");

    auto go = std::make_shared<GameObject>();
    go->transform = gobj.transform;
    go->color = gobj.color;
    go->texturePath = gobj.texturePath;

    // Load texture if it exists
    if (std::strlen(go->texturePath.data()) > 0) {
      this->SetGameObjectTexture(go, go->texturePath.data());
    }

    this->AddGameObject(gobj.name, go);
  }
}
