#include "WindowPlay.h"
#include "SceneWindow.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleResource.h"
#include "ModuleFileSystem.h"

#include "ResourceScene.h"

#include "Dependencies/ImGui/imgui.h"
WindowPlay::WindowPlay(SceneWindow* parent, ImGuiWindowClass* windowClass, int ID, bool isActive): Window(parent, windowClass, ID, "Play Window", isActive)
{
}

WindowPlay::~WindowPlay()
{
}

void WindowPlay::Start()
{
}

void WindowPlay::Draw()
{
	if (!isActive) return;

	ImGui::Begin("Play Window");
	std::string name = App->scene->GameTime.running ? "Pause" : "Play";

	if (ImGui::Button(name.c_str()))
	{

		if (App->scene->GameTime.running == false)
		{
			scene = App->resources->CreateNewResource("", ResourceType::Scene, "tempScene");
			scene->libraryFile = SCENES_PATH;
			scene->libraryFile.append(scene->name.c_str());
			scene->libraryFile.append(".scene");

			scene->assetsFile = SCENES_FOLDER;
			scene->assetsFile.append(scene->name.c_str());
			scene->assetsFile.append(".scene");

			App->resources->SaveResource(scene);

			App->scene->GameTime.Resume();
		}
		else if (App->scene->GameTime.running == true)
		{
			App->scene->GameTime.Pause();
		}
	}

	ImGui::SameLine();

	if (App->scene->GameTime.Read() > 0) {

		if (ImGui::Button("Stop"))
		{
			std::map<uint32, Resource*>::iterator it = App->resources->importedResources.begin();
			for (; it != App->resources->importedResources.end(); it++)
			{
				if (it->second->type == ResourceType::Scene && it->second->name == "tempScene" && it->second->libraryFile == "Library/Scenes/tempScene.scene")
				{
					App->resources->LoadResource(it->second->UID);

					std::string sceneToDelete = it->second->assetsFile;
					App->fileSystem->Remove(sceneToDelete.c_str());

					sceneToDelete = SCENES_PATH;
					sceneToDelete.append(scene->name.c_str());
					sceneToDelete.append(".scene");

					App->fileSystem->Remove(sceneToDelete.c_str());
				}
			}

			App->scene->GameTime.Restart();
		}
	}

	ImGui::SameLine(100);

	ImGui::Text("GameTime:");

	ImGui::SameLine();

	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%.2f", (App->scene->GameTime.Read() / 1000.0f));

	ImGui::End();
}

void WindowPlay::CleanUp()
{
}
