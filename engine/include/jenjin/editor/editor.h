#pragma once

#include "jenjin/gameobject.h"
#include "jenjin/scene.h"

#include <imgui.h>

namespace Jenjin {
namespace Editor {
class Manager {
public:
	Manager();

	void menu();
	void welcome();

	void dockspace();

	void controls(Jenjin::Scene* scene);
	void hierarchy(Jenjin::Scene* scene);
	void inspector(Jenjin::Scene* scene);
	void scene(Jenjin::Scene* scene);
	void viewport(Jenjin::Scene* scene);
	void explorer(Jenjin::Scene* scene);
	void code(Jenjin::Scene* scene);

	void backup_prompts(Jenjin::Scene* scene);

	void show_all(Jenjin::Scene* scene);

	struct {
		std::string projectPath;
		std::string openScenePath;
		std::string liveScenePath;
		std::string scriptsPath;
	} paths;

private:
	Jenjin::GameObject* selectedObject = nullptr;
	bool selectedCamera = false;

	char renameGameObjectBuffer[256] = { 0 };
};
}
}