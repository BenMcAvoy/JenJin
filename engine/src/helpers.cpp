#define GLFW_INCLUDE_NONE

#include "jenjin/helpers.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <IconsFontAwesome6.h>

#include <spdlog/spdlog.h>

namespace Jenjin::Helpers {
GLFWwindow *CreateWindow(int width, int height, const char *title) {
  if (!glfwInit()) {
    return nullptr;
  }

  GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  return window;
}

void CheckWindow(GLFWwindow *window) {
  if (!window) {
    const char *error;
    int code = glfwGetError(&error);
    spdlog::error("Failed to create window: {} ({})", error, code);
    exit(EXIT_FAILURE);
  }
}

void InitiateImGui(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 16.0f);

  float base = 16.0f;
  float icon = base * 2.0f / 3.0f;

  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
  ImFontConfig config;
  config.MergeMode = true;
  config.PixelSnapH = true;
  config.GlyphMinAdvanceX = icon;
  io.Fonts->AddFontFromFileTTF("./resources/fonts/fa-solid-900.ttf", base,
                               &config, icon_ranges);

  ImGui::StyleColorsDark();
  ImGuiStyle &style = ImGui::GetStyle();

  style.DockingSeparatorSize = 1.0f;
  style.FrameRounding = 4.0f;
  style.ChildRounding = 4.0f;
  style.PopupRounding = 2.0f;

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");
}
} // namespace Jenjin::Helpers
