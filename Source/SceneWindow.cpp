#include "Application.h"
#include "SceneWindow.h"
#include "Window.h"

#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleResource.h"

#include "WindowAbout.h"
#include "WindowAssetExplorer.h"
#include "WindowConfiguration.h"
#include "WindowConsole.h"
#include "WindowHirearchy.h"
#include "WindowInspector.h"
#include "WindowPlay.h"
#include "WindowNodeEditor.h"

#include "Dependencies/ImGui/imgui.h"
#include "Dependencies/ImGui/imgui_internal.h"

SceneWindow::SceneWindow(const char* name, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID)
	: name(name), 
	displayName(name),
	frameWindowClass(frameWindowClass), 
	windowClass(windowClass), 
	ID(ID)
{
	windows.clear();
	windowStrID = displayName + std::string("###") + name + ("_") + std::to_string(ID);
}

SceneWindow::~SceneWindow()
{
	for (uint i = 0; i < windows.size(); ++i)
		delete windows[i];

	windows.clear();
}

void SceneWindow::SetUp()
{
	for (uint i = 0; i < windows.size(); ++i)
		windows[i]->SetUp();
}

void SceneWindow::Draw()
{
	if (pendingLoadLayout)
		return;

	ImGuiWindowFlags frameWindow_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if (!isDockable)
		frameWindow_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	//Window settings and display
	ImGui::SetNextWindowClass(frameWindowClass);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	bool isActive = true;
	bool isOpen = ImGui::Begin(windowStrID.c_str(), isDockable ? &isActive : nullptr, frameWindow_flags);
	ImGui::PopStyleVar();

	if (!isActive)
		requestClose = true;

	//Adding a dock space to dock all child windows
	std::string dockName = windowStrID + std::string("_DockSpace");
	ImGuiID dockspace_id = ImGui::GetID(dockName.c_str());
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0, windowClass);

	MainMenuBar();

	ImGui::End();

	if (!isOpen)
		return;

	for (uint i = 0; i < windows.size(); ++i)
	{
		if (windows[i]->IsActive())
			windows[i]->Draw();
	}
}

Window* SceneWindow::GetWindow(const char* name) const
{
	for (uint i = 0; i < windows.size(); ++i)
		if (windows[i]->name == name) return windows[i];

	return nullptr;
}
