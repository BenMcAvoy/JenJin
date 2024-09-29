#pragma once

#include "jenjin/mesh.h"

#include <glm/glm.hpp>

#include <string>

namespace Jenjin {
class GameObject {
public:
	GameObject(std::string name, Mesh mesh) : mesh(mesh), name(name) {}
	~GameObject() = default;

	std::string name;

	// Transform
	struct Transform {
		glm::vec2 position;
		glm::vec2 scale;
		float rotation;
	} transform = { glm::vec2(0.0f), glm::vec2(1.0f), 0.0f };

	// Appearance
	glm::vec3 color = glm::vec3(1.0f);

	// Mesh used to render in `Scene::render()`
	Mesh mesh;
	MeshReference* meshReference = nullptr;

	// TODO: hierarchy
#ifdef false
	GameObject* parent = nullptr;
	std::vector<GameObject*> children = {};
#endif

	// Getters
	std::string GetName() { return name; }
	glm::vec2 GetPosition() { return transform.position; }
	glm::vec2 GetScale() { return transform.scale; }
	float GetRotation() { return transform.rotation; }

	// Setters
	void SetName(std::string name) { this->name = name; }
	void SetPosition(glm::vec2 position) { transform.position = position; }
	void SetScale(glm::vec2 scale) { transform.scale = scale; }
	void SetRotation(float rotation) { transform.rotation = rotation; }

	// Pointers
	std::string* GetNamePointer() { return &name; }
	glm::vec2* GetPositionPointer() { return &transform.position; }
	glm::vec2* GetScalePointer() { return &transform.scale; }
	float* GetRotationPointer() { return &transform.rotation; }
};
}