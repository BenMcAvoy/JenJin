#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Jenjin {
namespace Helpers {
GLFWwindow *CreateWindow(int width, int height, const char *title);
void CheckWindow(GLFWwindow *window);
void InitiateImGui(GLFWwindow *window);
} // namespace Helpers
} // namespace Jenjin
