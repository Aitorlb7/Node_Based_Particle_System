#include "Application.h"
#include "MainWindow.h"

#include "ModuleScene.h"
#include "ModuleResource.h"
#include "ModuleEditor.h"

#include "ResourceScene.h"

#include "WindowAbout.h"
#include "WindowAssetExplorer.h"
#include "WindowConfiguration.h"
#include "WindowConsole.h"
#include "WindowHirearchy.h"
#include "WindowInspector.h"
#include "WindowPlay.h"
#include "WindowNodeEditor.h"

MainWindow::MainWindow(ImGuiWindowClass* parent, ImGuiWindowClass* windowClass, int ID) : SceneWindow(GetName(), parent, windowClass, ID)
{
	windows.push_back(new WindowAbout(this,windowClass,ID,false));
	windows.push_back(new WindowAssetExplorer(this, windowClass, ID, true));
	windows.push_back(new WindowConfiguration(this, windowClass, ID, false));
	windows.push_back(new WindowConsole(this, windowClass, ID, true));
	windows.push_back(new WindowHirearchy(this, windowClass, ID, true));
	windows.push_back(new WindowInspector(this, windowClass, ID, true));
	windows.push_back(new WindowPlay(this, windowClass, ID, true));
	windows.push_back(new WindowNodeEditor(this, windowClass, ID, false));
}

MainWindow::~MainWindow()
{
}


void MainWindow::MainMenuBar()
{
	std::string menuAction = "";
	if (ImGui::BeginMainMenuBar())
	{
		MenuItem_File(menuAction);
		MenuItem_Save(menuAction);
		MenuItem_Load(menuAction);
		MenuItem_View();
		MenuItem_Add();
		MenuItem_Tools();
		MenuItem_Help();

		ImGui::EndMenuBar();
		ImGui::End();

	}
}

void MainWindow::MenuItem_File(std::string menuAction)
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save Scene"))
		{
			scene = App->resources->CreateNewResource("", ResourceType::Scene, "scene");
			menuAction = "Save Scene";
		}
		if (ImGui::MenuItem("Load Scene"))
		{

			std::map<uint32, Resource*>::iterator it = App->resources->importedResources.begin();
			for (; it != App->resources->importedResources.end(); it++)
			{
				if (it->second->type == ResourceType::Scene)
				{
					scenesInMemory.push_back((ResourceScene*)it->second);
				}
			}

			if (!scenesInMemory.empty()) selectedScene = scenesInMemory.front()->name;

			menuAction = "Load Scene";
		}
		//if (ImGui::MenuItem("Exit")) ret = false;

		ImGui::EndMenu();
	}

	//Set Next window Pos and Size
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos, ImGuiCond_Appearing, ImVec2(0.5, 0.5));
	ImGui::SetNextWindowSize(ImVec2(250.0f, 150.0f));
}

void MainWindow::MenuItem_Save(std::string menuAction)
{
	//Set next Pop based on a string
	if (menuAction == "Save Scene") ImGui::OpenPopup("Save Scene Options");


	if (ImGui::BeginPopupModal("Save Scene Options", NULL, ImGuiWindowFlags_NoResize))
	{

		//ImGui::Spacing();
		//ImGui::TextColored(sceneTextColor, scene->libraryFile.c_str());
		ImGui::Spacing();

		if (ImGui::InputText("Scene Name:", (char*)sceneName.c_str(), 64, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			scene->libraryFile = SCENES_PATH;
			scene->libraryFile.append(sceneName.c_str());
			scene->libraryFile.append(".scene");

			scene->assetsFile = SCENES_FOLDER;
			scene->assetsFile.append(sceneName.c_str());

			scene->name = sceneName.c_str();

			//sceneTextColor = ImVec4(0, 0.8, 0, 1);

			App->resources->SaveResource(scene);

			ImGui::CloseCurrentPopup();
			//allowSaveOrLoad = true;

		}


		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "Press 'Enter' to save the scene.");
		ImGui::Spacing();

		//ImGui::Spacing();
		//if (allowSaveOrLoad)
		//{
		//	if (ImGui::Button("Save"))
		//	{
		//		App->resources->SaveResource(scene);
		//		allowSaveOrLoad = false;
		//		ImGui::CloseCurrentPopup();
		//	}
		//}


		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}



		ImGui::EndPopup();
	}

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos, ImGuiCond_Appearing, ImVec2(0.5, 0.5));
	ImGui::SetNextWindowSize(ImVec2(250.0f, 150.0f));
}

void MainWindow::MenuItem_Load(std::string menuAction)
{
}

void MainWindow::MenuItem_View()
{

	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem("Configuration", "", configWindow->isActive)) configWindow->Enable();
		if (ImGui::MenuItem("Particle Node Editor", "", nodeEditorWindow->isActive)) nodeEditorWindow->Enable();
		if (ImGui::MenuItem("Console", "", consoleWindow->isActive)) consoleWindow->Enable();
		if (ImGui::MenuItem("Hierarchy", "", hirearchyWindow->isActive)) hirearchyWindow->Enable();
		if (ImGui::MenuItem("Inspector", "", inspectorWindow->isActive)) inspectorWindow->Enable();

		ImGui::EndMenu();
	}
}

void MainWindow::MenuItem_Add()
{
	if (ImGui::BeginMenu("Add"))
	{
		if (ImGui::MenuItem("Create Game Object")) App->scene->CreateGameObject("New_Empty_", App->scene->root_object);

		if (App->scene->selected_object != nullptr)
		{
			if (ImGui::MenuItem("Create Children Object")) App->scene->CreateGameObject("New_EmptyChildren_", App->scene->selected_object);
		}

		ImGui::EndMenu();

	}
}

void MainWindow::MenuItem_Tools()
{
	if (ImGui::BeginMenu("Tools"))
	{
		if (ImGui::MenuItem("Set Guizmo: Translate (W)", "W"))	App->scene->gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		if (ImGui::MenuItem("Set Guizmo: Rotate (E)", "E"))		App->scene->gizmoOperation = ImGuizmo::OPERATION::ROTATE;
		if (ImGui::MenuItem("Set Guizmo: Scale (R)", "R"))		App->scene->gizmoOperation = ImGuizmo::OPERATION::SCALE;

		ImGui::EndMenu();
	}
}

void MainWindow::MenuItem_Help()
{
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("ImGui Demo"))App->editor->show_demo_window = !App->editor->show_demo_window;
		if (ImGui::MenuItem("Documentation")) App->editor->RequestBrowser("https://github.com/paufiol/AnotherSmallEngine/blob/master/README.md");
		if (ImGui::MenuItem("Latest Release")) App->editor->RequestBrowser("https://github.com/paufiol/AnotherSmallEngine");
		if (ImGui::MenuItem("Report a bug")) App->editor->RequestBrowser("https://github.com/paufiol/AnotherSmallEngine/issues");
		if (ImGui::MenuItem("About", "", aboutWindow->isActive)) aboutWindow->Enable();
		ImGui::EndMenu();
	}
}

