#include "WindowConsole.h"
#include "SceneWindow.h"
#include "Dependencies/ImGui/imgui.h"

WindowConsole::WindowConsole(SceneWindow* parent, ImGuiWindowClass* windowClass, int ID, bool isActive): Window(parent, windowClass, ID, "Console Window", isActive)
{
}

WindowConsole::~WindowConsole()
{
}

void WindowConsole::Start()
{
}

void WindowConsole::Draw()
{
	if (!isActive) return;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));

	ImGui::Begin("Console", &isActive);

	for (int i = 0; i < log_record.size(); i++)
	{
		ImVec4 textColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		if (strstr(log_record[i].c_str(), "ERROR") != nullptr)
		{
			textColor = { 1.0f, 0.0f, 0.3f, 0.7f };
		}

		ImGui::PushStyleColor(ImGuiCol_Text, textColor);

		ImGui::Text("%s", log_record[i].c_str());

		ImGui::PopStyleColor();
	}

	ImGui::PopStyleVar();

	if (scrollDown) ImGui::SetScrollHere(1.0f);
	scrollDown = false;

	ImGui::End();
}


void WindowConsole::AddLog(std::string text) {

	if (&log_record != NULL) {
		log_record.push_back(text);
		scrollDown = true;
	}
}

void WindowConsole::CleanUp()
{
	log_record.clear();
}
