#define GLFW_INCLUDE_NONE

#include "jenjin/editor/editor.h"
#include "jenjin/editor/utils.h"
#include "jenjin/editor/widgets.h"
#include "jenjin/engine.h"
#include "jenjin/gameobject.h"
#include "jenjin/helpers.h"
#include "jenjin/scene.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <IconsFontAwesome6.h>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

static char codeBuffer[1024 * 16] = {0};

static char codeFile[260];

using namespace Jenjin::Editor;

Manager::Manager() {}

void Manager::menu() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (this->hasProjectOpen) {
        if (ImGui::MenuItem("New Scene")) {
          auto scene = std::make_shared<Jenjin::Scene>();
          Jenjin::EngineRef->AddScene(scene, true);
        }

        if (ImGui::MenuItem("Open Scene")) {
          Jenjin::EngineRef->GetCurrentScene()->Load("main.jenscene");
        }

        if (ImGui::MenuItem("Save Scene")) {
          Jenjin::EngineRef->GetCurrentScene()->Save("main.jenscene");
        }
      }

      if (ImGui::MenuItem("Exit")) {
        glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
      }

      ImGui::EndMenu();
    }

    if (this->hasProjectOpen) {
      if (ImGui::BeginMenu("Scripts")) {
        if (ImGui::MenuItem("Reload")) {
          auto luaManager =
              Jenjin::EngineRef->GetCurrentScene()->GetLuaManager();

          luaManager->ReloadScripts("scripts/");
          luaManager->Ready();
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Game")) {
        if (ImGui::MenuItem(this->running ? "Stop" : "Play")) {
          this->running = !this->running;

          auto scene = Jenjin::EngineRef->GetCurrentScene();

          if (this->running) {
            scene->Save("live.jenscene");

            Jenjin::EngineRef->GetCurrentScene()
                ->GetLuaManager()
                ->LoadDirectory(("scripts/"));

            spdlog::info("Readying lua manager");
            scene->GetLuaManager()->Ready();
          } else {
            scene->Load("live.jenscene");
          }
        }

        ImGui::EndMenu();
      }
    }

    auto toDisplay = fmt::format("FPS: {:.2f}", ImGui::GetIO().Framerate);
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() -
                         ImGui::CalcTextSize(toDisplay.c_str()).x - 10);
    ImGui::Text("%s", toDisplay.c_str());

    ImGui::EndMainMenuBar();
  }
}

void Manager::dockspace() {
  static ImGuiDockNodeFlags dockspace_flags =
      ImGuiDockNodeFlags_PassthruCentralNode;
  static ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

  // Position and resize the dockspace window
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);

  // Style it so that it isn't visible, doesn't have rounding, etc
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags |=
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  window_flags |= ImGuiWindowFlags_NoBackground;

  // Create the actual dockspace window
  ImGui::Begin("DockSpace", nullptr, window_flags);
  ImGui::PopStyleVar(3);

  // Create a dockspace layout
  ImGuiIO &io = ImGui::GetIO();
  ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

  static auto first_time = true;
  if (first_time) {
    first_time = false;

    // Reset layout
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id,
                              dockspace_flags | ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

    // Dock the welcome window in the center
    ImGui::DockBuilderDockWindow("Welcome", dockspace_id);

    auto dock_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left,
                                                 0.25f, nullptr, &dockspace_id);
    auto dock_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right,
                                                  0.6f, nullptr, &dockspace_id);
    auto dock_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down,
                                                 0.2f, nullptr, &dockspace_id);
    auto middle = dockspace_id;

    static auto hierarchy_title = ICON_FA_SITEMAP " Hierarchy";
    ImGui::DockBuilderDockWindow(hierarchy_title, dock_left);

    static auto explorer_title = ICON_FA_FOLDER " Explorer";
    ImGui::DockBuilderDockWindow(explorer_title, dock_down);

    static auto console_title = ICON_FA_TERMINAL " Console";
    ImGui::DockBuilderDockWindow(console_title, dock_down);

    static auto inspector_title = ICON_FA_EYE " Inspector";
    ImGui::DockBuilderDockWindow(inspector_title, dock_right);

    static auto code_title = ICON_FA_CODE " Code";
    ImGui::DockBuilderDockWindow(code_title, middle);

    static auto viewport_title = ICON_FA_VIDEO " Viewport";
    ImGui::DockBuilderDockWindow(viewport_title, middle);

    ImGui::DockBuilderFinish(dockspace_id);
  }

  ImGui::End();
}

void Manager::viewport(Jenjin::Scene *scene) {
  static auto title = ICON_FA_VIDEO " Viewport";
  ImGui::Begin(title, nullptr,
               ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoScrollWithMouse);

  ImVec2 size = ImGui::GetContentRegionAvail();
  static auto target = Jenjin::EngineRef->GetCurrentScene()->GetTarget();
  target->Resize(reinterpret_cast<glm::vec2 &>(size));

  if (renderTexture == -1) {
    ImGui::GetWindowDrawList()->AddText(ImGui::GetCursorScreenPos(),
                                        ImColor(255, 255, 255, 255),
                                        "No render target");
    ImGui::End();

    return;
  }

  ImGui::GetWindowDrawList()->AddImage(
      (ImTextureID)renderTexture, ImVec2(ImGui::GetCursorScreenPos()),
      ImVec2(ImGui::GetCursorScreenPos().x + size.x,
             ImGui::GetCursorScreenPos().y + size.y),
      ImVec2(0, 1), ImVec2(1, 0));

  std::string res_text =
      std::to_string((int)size.x) + "x" + std::to_string((int)size.y);
  ImGui::GetWindowDrawList()->AddText(
      ImVec2(ImGui::GetCursorScreenPos().x + 10,
             ImGui::GetCursorScreenPos().y + 10),
      ImColor(255, 255, 255, 255), res_text.c_str());

  ImGui::End();
}

void Manager::hierarchy(Jenjin::Scene *scene) {
  if (scene == nullptr) {
    spdlog::error("Scene is nullptr");
    return;
  }

  std::vector<std::shared_ptr<Jenjin::GameObject>> *gameObjects =
      Jenjin::EngineRef->GetCurrentScene()->GetGameObjects();

  static auto title = ICON_FA_SITEMAP " Hierarchy";
  ImGui::Begin(title);

  static char name[128] = {0};
  // Press or Ctrl + Shift + A
  if (ImGui::Button("+", ImVec2(40, 40)) ||
      (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)) &&
       ImGui::GetIO().KeyCtrl && ImGui::GetIO().KeyShift)) {
    memset(name, 0, sizeof(name));
    ImGui::OpenPopup("Add GameObject");
  }

  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SetNextWindowSize(ImVec2(300, 200));
  if (ImGui::BeginPopupModal("Add GameObject")) {
    ImGui::SetItemDefaultFocus();
    ImGui::InputText("Name", name, sizeof(name));

    if (ImGui::Button("Create##Add") ||
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
      auto gameObject = std::make_shared<Jenjin::GameObject>(
          name, Jenjin::Helpers::CreateQuad(2.0f, 2.0f));
      scene->AddGameObject(gameObject);
      scene->Build();

      selectedCamera = false;
      selectedObject = gameObjects->back().get();

      memset(this->renameGameObjectBuffer, 0,
             sizeof(this->renameGameObjectBuffer));
      memcpy(this->renameGameObjectBuffer, selectedObject->name.c_str(),
             selectedObject->name.size());

      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel##Add") ||
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  static auto cameraText = ICON_FA_VIDEO " Camera";
  if (ImGui::Selectable(cameraText, selectedCamera)) {
    selectedCamera = !selectedCamera;
    if (selectedCamera)
      selectedObject = nullptr;
  }

  for (std::shared_ptr<Jenjin::GameObject> gameObject : *gameObjects) {
    bool isSelected = selectedObject == gameObject.get();

    const auto text = fmt::format("{} {}", ICON_FA_CUBE, gameObject->name);
    if (ImGui::Selectable(text.c_str(), isSelected)) {
      if (isSelected) {
        selectedObject = nullptr;
      } else {
        selectedCamera = false;
        selectedObject = gameObject.get();
        memset(this->renameGameObjectBuffer, 0,
               sizeof(this->renameGameObjectBuffer));
        memcpy(this->renameGameObjectBuffer, selectedObject->name.c_str(),
               selectedObject->name.size());
      }
    }
  }

  // Autoscroll if the user goes to the bottom
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
    ImGui::SetScrollHereY(1.0f);
  }

  ImGui::End();
}

void Manager::inspector(Jenjin::Scene *scene) {
  if (!selectedObject && !selectedCamera) {
    return;
  }

  static auto appearance_title = ICON_FA_PALETTE " Appearance";
  static auto title = ICON_FA_EYE " Inspector";
  ImGui::Begin(title);

  if (selectedCamera) {
    static auto transform_title =
        ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT " Transform";
    if (ImGui::CollapsingHeader(transform_title,
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Indent();
      Jenjin::Editor::Widgets::cameraWidget(scene->GetCamera());
      ImGui::Unindent();
    }

    ImGui::ItemSize(ImVec2(0, 10));

    if (ImGui::CollapsingHeader(appearance_title)) {
      ImGui::Indent();
      ImGui::ColorPicker3(
          "Background",
          glm::value_ptr(*scene->GetCamera()->GetBackgroundPointer()));
      ImGui::Unindent();
    }
  }

  if (selectedObject) {
    static auto transform_title =
        ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT " Transform";
    if (ImGui::CollapsingHeader(transform_title,
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Indent();
      Jenjin::Editor::Widgets::transformWidget(&selectedObject->transform);
      ImGui::Unindent();
    }

    ImGui::ItemSize(ImVec2(0, 10));

    if (ImGui::CollapsingHeader(appearance_title)) {
      ImGui::Indent();
      ImGui::ColorPicker3("Color", glm::value_ptr(selectedObject->color));
      ImGui::Unindent();
    }

    ImGui::ItemSize(ImVec2(0, 10));

    static auto manage_title = ICON_FA_JAR " Manage";
    if (ImGui::CollapsingHeader(manage_title, ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Indent();

      float itemWidth = ImGui::GetContentRegionAvail().x;

      ImGui::SetNextItemWidth(itemWidth);
      ImGui::InputText("##RenameInput", renameGameObjectBuffer,
                       sizeof(renameGameObjectBuffer));

      static auto text = ICON_FA_PEN " Rename";
      if (ImGui::Button(text, ImVec2(itemWidth, 0))) {
        selectedObject->SetName(renameGameObjectBuffer);
      }

      static auto delete_text = ICON_FA_TRASH " Delete";
      if (ImGui::Button(delete_text, ImVec2(itemWidth, 0))) {
        scene->RemoveGameObject(selectedObject);
        selectedObject = nullptr;
      }

      ImGui::Unindent();
    }
  }

  ImGui::Unindent();

  ImGui::End();
}

void Manager::explorer(Jenjin::Scene *scene) {
  static auto title = ICON_FA_FOLDER " Explorer";
  ImGui::Begin(title);

  for (auto file : std::filesystem::directory_iterator(".")) {
    if (file.is_directory()) {
      bool shouldBeOpenByDefault =
          file.path().filename().string() == "textures";
      if (ImGui::TreeNode(file.path().filename().string().c_str())) {
        for (auto subfile : std::filesystem::directory_iterator(file.path())) {
          if (subfile.is_regular_file()) {
            auto ext = subfile.path().extension();

            if (ext == ".jenscene") {
              auto scene_text = fmt::format("{} {}", ICON_FA_CUBE,
                                            subfile.path().filename().string());
              if (ImGui::Selectable(scene_text.c_str())) {
                scene->Load(subfile.path().string());
              }
            } else if (ext == ".lua") {
              auto script_text =
                  fmt::format("{} {}", ICON_FA_FILE_CODE,
                              subfile.path().filename().string());
              if (ImGui::Selectable(script_text.c_str())) {
                std::ifstream file(subfile.path().string());
                if (file.is_open()) {
                  const std::string script =
                      std::string(std::istreambuf_iterator<char>(file),
                                  std::istreambuf_iterator<char>());

                  memcpy(codeBuffer, script.c_str(), script.size());
                  codeBuffer[script.size()] = '\0';
                }
                file.close();

                memcpy(codeFile, subfile.path().string().c_str(),
                       subfile.path().string().length());
                codeFile[strlen(subfile.path().string().c_str())] = '\0';
              }
            } else if (ext == ".png" || ext == ".jpg") {
              auto texture_text = fmt::format(
                  "{} {}", ICON_FA_IMAGE, subfile.path().filename().string());
              if (ImGui::Selectable(texture_text.c_str())) {
                if (selectedObject) {
                  scene->SetGameObjectTexture(selectedObject,
                                              subfile.path().string());
                } else {
                  spdlog::warn("No object selected");

                  // TODO: Notificaiton system
                }
              }
            } else {
              auto file_text = fmt::format("{} {}", ICON_FA_FILE,
                                           subfile.path().filename().string());
              if (ImGui::Selectable(file_text.c_str())) {
                spdlog::warn("No handler for file type: {}", ext.string());
              }
            }
          }
        }
        ImGui::TreePop();
      }
    }
  }

  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 256);

  static bool demo_tools;
  ImGui::Checkbox("Demo Tools", &demo_tools);
  if (demo_tools)
    ImGui::ShowDemoWindow(&demo_tools);

  ImGui::End();
}

void Manager::backup_prompts(Jenjin::Scene *scene) {
  if (glfwWindowShouldClose(glfwGetCurrentContext())) {
    if (!ImGui::IsPopupOpen("Exit")) {
      glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_FALSE);
      ImGui::OpenPopup("Exit");
    }
  }

  // promtp with exit, save and exit, cancel
  if (ImGui::BeginPopupModal("Exit", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Do you want to save before exiting?");
    ImGui::Separator();

    if (ImGui::Button("Save and Exit")) {
      Jenjin::EngineRef->GetCurrentScene()->Save("main.jenscene");
      glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
    }

    ImGui::SameLine();

    if (ImGui::Button("Exit")) {
      glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
      glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_FALSE);
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void Manager::code(Jenjin::Scene *scene) {
  static auto title = ICON_FA_CODE " Code";
  static bool showUnsaved = false;

  ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
  if (showUnsaved)
    flags |= ImGuiWindowFlags_UnsavedDocument;

  // TODO: Tabs at the top for each file (NOTE: This should probably be a class
  // once this is implemented)
  ImGui::Begin(title, nullptr, flags);

  static auto save = [&]() {
    std::ofstream file(codeFile);
    file << codeBuffer;
    file.close();

    Jenjin::EngineRef->GetCurrentScene()->GetLuaManager()->ReloadScripts(
        "scripts/");
    Jenjin::EngineRef->GetCurrentScene()->GetLuaManager()->Ready();

    showUnsaved = false;
  };

  if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)) &&
      ImGui::GetIO().KeyCtrl) {
    save();
  }

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      static auto save_label = ICON_FA_FLOPPY_DISK " Save";
      if (ImGui::MenuItem(save_label, "Ctrl+S"))
        save();

      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }

  auto codePosition = ImGui::GetWindowPos();
  static auto *target = Jenjin::EngineRef->GetCurrentScene()->GetTarget();
  target->SetWindowPosition(codePosition);

  // TODO: completions on tab
  if (ImGui::InputTextMultiline("##Code", codeBuffer, sizeof(codeBuffer),
                                ImVec2(ImGui::GetWindowWidth() - 20,
                                       ImGui::GetWindowHeight() - 40 - 32 - 24),
                                ImGuiInputTextFlags_AllowTabInput)) {
    showUnsaved = true;
  }

  ImGui::BeginChild("InfoChild", ImVec2(0, 0), true);
  ImGui::Text("%s", codeFile);
  ImGui::EndChild();

  ImGui::End();
}

void Manager::console(Jenjin::Scene *scene) {
  static auto sink = Jenjin::EngineRef->GetLogSink();

  if (sink == nullptr) {
    ImGui::Begin("Console", nullptr);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
    ImGui::Text("No sink set");
    ImGui::PopStyleColor();
    ImGui::End();
    return;
  }

  static auto title = ICON_FA_TERMINAL " Console";
  ImGui::Begin(title, nullptr, ImGuiWindowFlags_MenuBar);

  if (ImGui::BeginMenuBar()) {
    if (ImGui::MenuItem("Clear")) {
      spdlog::info("Clearing logs");
      sink->ClearLogs();
    }

    ImGui::EndMenuBar();
  }

  if (ImGui::CollapsingHeader("Error")) {
    auto &error_logs = sink->GetErrorLogs();
    for (auto &log : sink->GetErrorLogs()) {
      ImGui::Text("(%d) %s", log.second, log.first.c_str());
    }
  }

  if (ImGui::CollapsingHeader("Info")) {
    auto &info_logs = sink->GetInfoLogs();
    for (auto &log : sink->GetInfoLogs()) {
      ImGui::Text("(%d) %s", log.second, log.first.c_str());
    }
  }

  if (ImGui::CollapsingHeader("Warn")) {
    auto &warn_logs = sink->GetWarnLogs();
    for (auto &log : sink->GetWarnLogs()) {
      ImGui::Text("(%d) %s", log.second, log.first.c_str());
    }
  }

  if (ImGui::CollapsingHeader("Trace")) {
    auto &trace_logs = sink->GetTraceLogs();
    for (auto &log : sink->GetTraceLogs()) {
      ImGui::Text("(%d) %s", log.second, log.first.c_str());
    }
  }


  ImGui::End();
}

void Manager::show_all(Jenjin::Scene *scene) {
  bool isRunning = this->running;
  if (isRunning) {
    scene->Update();

    auto bg = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    auto red_bg = ImVec4((bg.x + 0.01) * 1.5f, bg.y, bg.z, bg.w);
    auto red_bg_dark =
        ImVec4((bg.x + 0.01) * 1.5, bg.y * 0.9, bg.z * 0.9, bg.w);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, red_bg);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, red_bg_dark);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, red_bg);
  }

  if (!this->hasProjectOpen) {
    menu();
    dockspace();
    welcome();

    return;
  }

  menu();
  dockspace();
  if (isRunning)
    ImGui::PopStyleColor(3);

  hierarchy(scene);
  inspector(scene);
  backup_prompts(scene);
  explorer(scene);
  viewport(scene);
  code(scene);
  console(scene);
}

void Manager::welcome() {
  ImGui::Begin("Welcome", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

  static std::string selectedPreExisting = "";

  int pad = ImGui::GetStyle().WindowPadding.y * 2 -
            ImGui::GetFrameHeightWithSpacing();
  int spad = ImGui::GetStyle().WindowPadding.y * 2 +
             ImGui::GetStyle().ItemSpacing.y * 2;

  ImGui::BeginChild(
      "WelcomeChild",
      ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x * 2,
             ImGui::GetWindowHeight() - pad - 64 - spad -
                 ImGui::GetStyle().WindowPadding.y),
      true);

  auto open_project = [&]() {
    auto file = std::string(selectedPreExisting);
    auto jendir = Jenjin::Editor::get_jendir();
    this->hasProjectOpen = true;

    std::filesystem::current_path(jendir + "/Projects/" + file);
    spdlog::info("Changed cwd to {}", std::filesystem::current_path().string());

    Jenjin::EngineRef->GetCurrentScene()->Load("main.jenscene");

    // Load all the lua files
    Jenjin::EngineRef->GetCurrentScene()->GetLuaManager()->LoadDirectory(
        "scripts/");
  };

  for (auto file : std::filesystem::directory_iterator(
           Jenjin::Editor::get_jendir() + "/Projects")) {
    if (file.is_directory()) {
      bool isSelected = selectedPreExisting == file.path().filename().string();
      const auto text =
          fmt::format("{} {}", ICON_FA_FOLDER, file.path().filename().string());

      if (ImGui::Selectable(text.c_str(), isSelected, ImGuiSelectableFlags_None,
                            ImVec2(ImGui::GetWindowWidth() -
                                       ImGui::GetStyle().WindowPadding.x * 2,
                                   0))) {
        selectedPreExisting = file.path().filename().string();

        if (isSelected) {
          open_project();
        }
      }
    }
  }

  ImGui::EndChild();

  ImGui::BeginChild(
      "CreateProject",
      ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x * 2,
             64 - spad),
      true);

  if (selectedPreExisting.empty()) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
  }

  static auto open_project_text = ICON_FA_FOLDER_OPEN " Open Project";
  if (ImGui::Button(open_project_text) && !selectedPreExisting.empty()) {
    open_project();
  }

  if (selectedPreExisting.empty())
    ImGui::PopStyleColor(3);

  ImGui::SameLine();

  static auto new_project_text = ICON_FA_FOLDER_PLUS " New Project";
  if (ImGui::Button(new_project_text)) {
    ImGui::OpenPopup("New Project");
  }

  ImGui::SameLine();

  static auto text = ICON_FA_TRASH " Delete Project";
  auto width = ImGui::CalcTextSize(text).x;
  ImGui::SetCursorPosX(ImGui::GetWindowWidth() - width -
                       ImGui::GetStyle().WindowPadding.x -
                       ImGui::GetStyle().ItemSpacing.x);
  if (ImGui::Button(text)) {
    ImGui::OpenPopup("DeleteProject");
  }

  if (ImGui::BeginPopupModal("New Project", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    static char projectName[128] = {0};
    ImGui::InputText("Project Name", projectName, sizeof(projectName));
    if (ImGui::Button("Create##NewProject") ||
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
      std::string jendir = Jenjin::Editor::get_jendir();
      std::string projectPath = jendir + "/Projects/" + projectName;

      Jenjin::Editor::ensure_dir(projectPath);
      Jenjin::Editor::ensure_dir(projectPath + "/scripts");
      Jenjin::Editor::ensure_dir(projectPath + "/textures");

      this->hasProjectOpen = true;
      std::filesystem::current_path(projectPath);
      spdlog::info("Changed cwd to {}",
                   std::filesystem::current_path().string());

      Jenjin::EngineRef->GetCurrentScene()->GetGameObjects()->clear();
      Jenjin::EngineRef->GetCurrentScene()->Save("main.jenscene");
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel##NewProject") ||
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  if (ImGui::BeginPopupModal("DeleteProject", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Are you sure you want to delete this project?");
    ImGui::Separator();
    if (ImGui::Button("Delete##DeleteProject")) {
      std::string jendir = Jenjin::Editor::get_jendir();
      std::string projectPath = jendir + "/Projects/" + selectedPreExisting;
      std::filesystem::remove_all(projectPath);
      selectedPreExisting = "";
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel##DeleteProject")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  ImGui::EndChild();
  ImGui::End();
}
