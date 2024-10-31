#define GLFW_INCLUDE_NONE

#include "jenjin/editor/widgets.h"
#include "jenjin/gameobject.h"
#include "jenjin/camera.h"

#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <IconsFontAwesome6.h>

#define LEFT_RIGHT ICON_FA_ARROWS_LEFT_RIGHT
#define UP_DOWN ICON_FA_ARROWS_UP_DOWN

#define ROTATE ICON_FA_ARROWS_SPIN

using namespace Jenjin::Editor;

static void drawButtonWithDrag(const std::string &buttonLabel,
                               const std::string &dragLabel, float *value,
                               const ImVec4 &buttonColor,
                               const ImVec4 &buttonHoveredColor,
                               const ImVec4 &buttonActiveColor,
                               const ImVec2 &buttonSize, int defaultValue = 0) {

  ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);

  if (ImGui::Button(buttonLabel.c_str(), buttonSize))
    *value = defaultValue;

  ImGui::PopStyleColor(3);

  ImGui::SameLine();
  ImGui::DragFloat(dragLabel.c_str(), value, 0.1f, 0.0f, 0.0f, "%.2f");
  ImGui::PopItemWidth();
  ImGui::SameLine();
}

static void start_widget(const std::string &label, int count = 2) {
	ImGui::ItemSize(ImVec2(0.0f, 2.0f));
  ImGui::PushID(label.c_str());
  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, 100.0f);
  ImGui::Text("%s", label.c_str());
  ImGui::NextColumn();

  // HACK: This is a hack to make the drag float buttons all the same size...
  // this needs to be revisited
  auto &s = ImGui::GetStyle();
  int added = count == 1 ? s.ItemSpacing.x * 2 + s.FramePadding.x + 1 : 0;
  ImGui::PushMultiItemsWidths(count, ImGui::CalcItemWidth() + added);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
}

static void end_widget() {
  ImGui::PopStyleVar();
  ImGui::Columns(1);
  ImGui::PopID();
}

void Widgets::transformWidget(Jenjin::GameObject::Transform *transform) {
  float lineHeight =
      GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
  ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

	static auto left_right_title = LEFT_RIGHT;
	static auto up_down_title = UP_DOWN;

  start_widget("Position");
  drawButtonWithDrag("X", "##X", &transform->position.x,
                     ImVec4{0.6f, 0.2f, 0.2f, 1.0f},
                     ImVec4{0.7f, 0.3f, 0.3f, 1.0f},
                     ImVec4{0.6f, 0.2f, 0.2f, 1.0f}, buttonSize);

  drawButtonWithDrag("Y", "##Y", &transform->position.y,
                     ImVec4{0.3f, 0.6f, 0.3f, 1.0f},
                     ImVec4{0.4f, 0.7f, 0.4f, 1.0f},
                     ImVec4{0.3f, 0.6f, 0.3f, 1.0f}, buttonSize);
  end_widget();

  start_widget("Scale");
  drawButtonWithDrag(left_right_title, "##ScaleW", &transform->scale.x,
                     ImVec4{0.6f, 0.2f, 0.2f, 1.0f},
                     ImVec4{0.7f, 0.3f, 0.3f, 1.0f},
                     ImVec4{0.6f, 0.2f, 0.2f, 1.0f}, buttonSize, 1);

  drawButtonWithDrag(up_down_title, "##ScaleH", &transform->scale.y,
                     ImVec4{0.3f, 0.6f, 0.3f, 1.0f},
                     ImVec4{0.4f, 0.7f, 0.4f, 1.0f},
                     ImVec4{0.3f, 0.6f, 0.3f, 1.0f}, buttonSize, 1);
  end_widget();

	static auto rotation_title = ROTATE;
  start_widget("Rotation", 1);
  drawButtonWithDrag(rotation_title, "##Rotation", &transform->rotation,
                     ImVec4{0.3f, 0.4f, 0.7f, 1.0f},
                     ImVec4{0.4f, 0.5f, 0.8f, 1.0f},
                     ImVec4{0.3f, 0.4f, 0.7f, 1.0f}, buttonSize);
  end_widget();
}

void Widgets::cameraWidget(Jenjin::Camera *camera) {
	float lineHeight =
			GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

	start_widget("Position");
	drawButtonWithDrag("X", "##X", &camera->GetPositionPointer()->x,
										 ImVec4{0.6f, 0.2f, 0.2f, 1.0f},
										 ImVec4{0.7f, 0.3f, 0.3f, 1.0f},
										 ImVec4{0.6f, 0.2f, 0.2f, 1.0f}, buttonSize);

	drawButtonWithDrag("Y", "##Y", &camera->GetPositionPointer()->y,
										 ImVec4{0.3f, 0.6f, 0.3f, 1.0f},
										 ImVec4{0.4f, 0.7f, 0.4f, 1.0f},
										 ImVec4{0.3f, 0.6f, 0.3f, 1.0f}, buttonSize);
	end_widget();

	start_widget("Rotation", 1);
	static auto rotation_title = ROTATE;
	drawButtonWithDrag(rotation_title, "##Rotation", camera->GetRotationPointer(),
										 ImVec4{0.3f, 0.4f, 0.7f, 1.0f},
										 ImVec4{0.4f, 0.5f, 0.8f, 1.0f},
										 ImVec4{0.3f, 0.4f, 0.7f, 1.0f}, buttonSize);
	end_widget();

	start_widget("Zoom", 1);
	auto zoom_title = *camera->GetZoomPointer() < 1.0f ? ICON_FA_COMPRESS : ICON_FA_EXPAND;
	drawButtonWithDrag(zoom_title, "##Zoom", camera->GetZoomPointer(),
										 ImVec4{0.3f, 0.4f, 0.7f, 1.0f},
										 ImVec4{0.4f, 0.5f, 0.8f, 1.0f},
										 ImVec4{0.3f, 0.4f, 0.7f, 1.0f}, buttonSize);
	end_widget();
}
