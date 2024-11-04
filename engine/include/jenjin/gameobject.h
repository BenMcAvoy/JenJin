#pragma once

#include "jenjin/datastore.h"
#include "jenjin/mesh.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <sol/sol.hpp>

#include <string>

namespace Jenjin {
class GameObject {
public:
  GameObject(std::string name, Mesh mesh) : mesh(mesh), name(name) {}
  ~GameObject() { spdlog::trace("GameObject::~GameObject(\"{}\")", name); }

  std::string name;

  // Transform
  struct Transform {
    glm::vec2 position;
    glm::vec2 scale;
    float rotation;
  } transform = {glm::vec2(0.0f), glm::vec2(1.0f), 0.0f};

  // Appearance
  glm::vec3 color = glm::vec3(1.0f);

  Mesh mesh;
  std::string texturePath = "";

  int meshReferenceID = -1;

  bool mixColor = false;

  DataStore dataStore;

  // TODO: hierarchy
#ifdef false
  GameObject *parent = nullptr;
  std::vector<GameObject *> children = {};
#endif

  // Getters
  std::string GetName() { return name; }
  glm::vec2 GetPosition() { return transform.position; }
  glm::vec2 GetScale() { return transform.scale; }
  float GetRotation() { return transform.rotation; }
  glm::vec3 GetColor() { return color; }

  // Setters
  void SetName(std::string name) { this->name = name; }
  void SetPosition(glm::vec2 position) { transform.position = position; }
  void SetScale(glm::vec2 scale) { transform.scale = scale; }
  void SetRotation(float rotation) { transform.rotation = rotation; }
  void SetColor(glm::vec3 color) { this->color = color; }

  // Pointer getters
  std::string *GetNamePointer() { return &name; }
  glm::vec2 *GetPositionPointer() { return &transform.position; }
  glm::vec2 *GetScalePointer() { return &transform.scale; }
  float *GetRotationPointer() { return &transform.rotation; }

  // Modifiers
  void Translate(glm::vec2 translation) { transform.position += translation; }
  void Scale(glm::vec2 scale) { transform.scale *= scale; }
  void Rotate(float rotation) { transform.rotation += rotation; }
};
} // namespace Jenjin
