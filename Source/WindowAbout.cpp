#include "WindowAbout.h"
#include "Application.h"
#include "SceneWindow.h"

#include "Dependencies/ImGui/imgui.h"

WindowAbout::WindowAbout(SceneWindow* parent, ImGuiWindowClass* windowClass, int ID, bool isActive) : Window(parent,windowClass,ID,"About Window", isActive)
{
}

WindowAbout::~WindowAbout()
{
}

void WindowAbout::Start()
{
}

void WindowAbout::Draw()
{
	if (!isActive) return;

	ImGui::Begin("About", &isActive);
	ImGui::Text("ASE - Another Small Engine");
	ImGui::Text("Engine developed for academic purpouses.");
	ImGui::Text("By Pau Fiol & Aitor Luque");
	sprintf(label, "Github Repository (Link)");
	if (ImGui::Selectable(label, true))	RequestBrowser("https://github.com/paufiol/AnotherSmallEngine");
	ImGui::Separator();
	ImGui::Text("Libraries used:");
	ImGui::BulletText("SDL");
	ImGui::BulletText("Glew");
	ImGui::BulletText("OpenGL");
	ImGui::BulletText("ImGui");
	ImGui::BulletText("MathGeoLib");
	ImGui::BulletText("Assimp");
	ImGui::Separator();
	ImGui::Text("GNU License:");
	sprintf(label, "Click here to see the full License");
	if (ImGui::Selectable(label, true))	RequestBrowser("https://github.com/paufiol/AnotherSmallEngine/blob/master/LICENSE.txt");

	ImGui::End();
}

void WindowAbout::RequestBrowser(const char* path)
{
	ShellExecuteA(0, "Open", path, 0, "", 5);
}
void WindowAbout::CleanUp()
{
}
