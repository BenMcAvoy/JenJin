#pragma once

#include "jenjin/datastore.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <sol/sol.hpp>

#include <string>

namespace Jenjin {
class GameObject {
public:
  GameObject() {}
  ~GameObject() {}

  // Transform
  struct Transform {
    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 scale = glm::vec2(1.0f);
    float rotation = 0.0f;
  } transform;

  // Appearance
  glm::vec3 color = glm::vec3(1.0f);

  std::string texturePath = "";
  DataStore dataStore;

  // Getters
  glm::vec2 GetPosition() { return transform.position; }
  glm::vec2 GetScale() { return transform.scale; }
  float GetRotation() { return transform.rotation; }
  glm::vec3 GetColor() { return color; }

  // Setters
  void SetPosition(glm::vec2 position) { transform.position = position; }
  void SetScale(glm::vec2 scale) { transform.scale = scale; }
  void SetRotation(float rotation) { transform.rotation = rotation; }
  void SetColor(glm::vec3 color) { this->color = color; }

  // Modifiers
  void Translate(glm::vec2 translation) { transform.position += translation; }
  void Scale(glm::vec2 scale) { transform.scale *= scale; }
  void Rotate(float rotation) { transform.rotation += rotation; }
};
} // namespace Jenjin
