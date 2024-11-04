#define GLFW_INCLUDE_NONE

#include "jenjin/targets/editor.h"
#include "jenjin/engine.h"
#include "jenjin/framebuffer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <IconsFontAwesome6.h>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

using namespace Jenjin::Targets;

EditorTarget::EditorTarget() {
  manager.renderTexture = this->renderTexture.texture;
}

void EditorTarget::PreRender() {
  Resize(GetSize());

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  manager.show_all(Jenjin::EngineRef->GetCurrentScene());

  renderTexture.Bind();

  static auto bg =
      Jenjin::EngineRef->GetCurrentScene()->GetCamera()->GetBackgroundPointer();
  glClearColor(bg->r, bg->g, bg->b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, width, height);
}

void EditorTarget::Render() { renderTexture.Unbind(); }

void EditorTarget::PostRender() {
  ImGui::EndFrame();
  ImGui::Render();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

glm::vec2 EditorTarget::GetSize() { return glm::vec2(width, height); }

void EditorTarget::Resize(glm::vec2 size) {
  this->width = size.x;
  this->height = size.y;
  renderTexture.Resize(width, height);
  Jenjin::EngineRef->GetCurrentScene()->GetCamera()->Resize(size);
}

void EditorTarget::SetWindowPosition(ImVec2 pos) { this->pos = pos; }

glm::vec2 positionToNDC(glm::vec2 pos, Jenjin::Camera *camera) {
  // Get camera position
  glm::vec2 cameraPos =
      glm::vec2(camera->GetPosition().x, camera->GetPosition().y);
  glm::vec2 newPos = pos + cameraPos;

  // Get projection matrix (with viewport
  auto viewproj = camera->GetViewProjection();

  auto aspect = camera->size.x / camera->size.y;

  // Get the NDC coordinates
  auto ndc = glm::vec2(viewproj * glm::vec4(newPos, 0.0f, 1.0f));
  ndc.x *= aspect;
  return ndc;
}

glm::vec2 EditorTarget::GetMousePosition() {
  static auto ctx = glfwGetCurrentContext();
  static double gx, gy;
  glfwGetCursorPos(ctx, &gx, &gy);

  static ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;

  auto lx = gx - (this->pos.x + windowPadding.x);
  auto ly = this->height - (gy - (this->pos.y + windowPadding.y * 4 - 2));

  auto centered =
      glm::vec2(lx - (float)this->width / 2, ly - (float)this->height / 2);

  // Convert it to NDC
  static auto camera = Jenjin::EngineRef->GetCurrentScene()->GetCamera();
  auto ndc = positionToNDC(centered, camera);

  return glm::vec2(ndc);
  /*return glm::vec2(lx, ly);*/
}

// We handle window resizing ourselves
bool EditorTarget::RespondsToWindowResize() { return false; }
