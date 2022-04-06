#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ModuleResource.h"
#include "ModuleFileSystem.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentCamera.h"
#include "ComponentMaterial.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

#include "WindowAbout.h"
#include "WindowAssetExplorer.h"
#include "WindowConfiguration.h"
#include "WindowConsole.h"
#include "WindowHirearchy.h"
#include "WindowInspector.h"
#include "WindowPlay.h"
#include "WindowNodeEditor.h"

#include "ResourceScene.h"
#include "ResourceMaterial.h"
#include "ResourceShader.h"
#include "Resource.h"

#include "JsonConfig.h"

#include "ImporterScene.h"
#include "ImporterMaterials.h"
#include "ImporterShader.h"

#include "Dependencies/ImGUI/imgui.h"
#include "Dependencies/ImGUI/imgui_internal.h"
#include "Dependencies/ImGUI/imgui_impl_sdl.h"
#include "Dependencies/ImGUI/imgui_impl_opengl3.h"
#include "Dependencies/Devil/Include/ilut.h"
#include "Dependencies/ImGuizmo/ImGuizmo.h"

#include <map>

#include "Dependencies/Glew/include/GL/glew.h"
#include "Dependencies/SDL/include/SDL_opengl.h"

#include <fstream>

ModuleEditor::ModuleEditor(bool start_enabled) : Module(start_enabled)
{
	aboutWindow = new WindowAbout(false);
	explorerWindow = new WindowAssetExplorer(true);
	configWindow = new WindowConfiguration(false);
	consoleWindow = new WindowConsole(true);
	hirearchyWindow = new WindowHirearchy(true);
	inspectorWindow = new WindowInspector(true);
	playWindow = new WindowPlay(true);
	nodeEditorWindow = new WindowNodeEditor(false);

	AddWindow(aboutWindow);
	AddWindow(explorerWindow);
	AddWindow(configWindow);
	AddWindow(consoleWindow);
	AddWindow(hirearchyWindow);
	AddWindow(inspectorWindow);
	AddWindow(playWindow);
	AddWindow(nodeEditorWindow);
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Start()
{
	LOG("Init Editor");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImFont* font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 15);

	ImGui::StyleColorsDark(); 
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init(NULL);

	std::vector<Window*>::iterator item = windows.begin();
	for (item; item != windows.end(); ++item){ 
		(*item)->Start();
	}

	editor.SetPalette(TextEditor::GetDarkPalette());

	return true;
}

update_status ModuleEditor::PreUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	
	ImGui::NewFrame();

	return ret;
}


update_status ModuleEditor::Update(float dt)
{

	Docking();

	if (!MainMenuBar()) return UPDATE_STOP;	
	SetupStyleFromHue();

	if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
	DropTargetWindow();
	TextEditorWindow();	

	GUIisHovered();
	ImGui::End();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	App->scene->ImGuizmoHandling();
	return ret;
}

bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	std::vector<Window*>::iterator item = windows.begin();
	for (item; item != windows.end(); ++item) {
		(*item)->CleanUp();
	}
	
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();
	return true;
}

void ModuleEditor::AddWindow(Window* window)
{
	windows.push_back(window);
}

void ModuleEditor::DrawGUI()
{
	
	ImGuiIO& io = ImGui::GetIO();


	std::vector<Window*>::iterator item = windows.begin();
	for (item; item != windows.end(); ++item) {
		(*item)->Draw();
	}

	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::GUIisHovered()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.WantCaptureMouse ? GUIhovered = true : GUIhovered = false;
	io.WantCaptureKeyboard ? isUserTyping = true : isUserTyping = false;
}


void  ModuleEditor::SetupStyleFromHue()
{
	static int hue = 140;
	static float col_main_sat = 180.f / 255.f;
	static float col_main_val = 161.f / 255.f;

	static float col_area_sat = 124.f / 255.f;
	static float col_area_val = 100.f / 255.f;

	static float col_back_sat = 59.f / 255.f;
	static float col_back_val = 40.f / 255.f;

	ImGui::Begin("Style");
	ImGui::SliderInt("master hue", &hue, 0, 255);

	float dummy;
	ImVec4 rgb;

	ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_main_sat, col_main_val, rgb.x, rgb.y, rgb.z);
	ImGui::ColorEdit3("main", &rgb.x);
	ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_main_sat, col_main_val);

	ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_area_sat, col_area_val, rgb.x, rgb.y, rgb.z);
	ImGui::ColorEdit3("area", &rgb.x);
	ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_area_sat, col_area_val);

	ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_back_sat, col_back_val, rgb.x, rgb.y, rgb.z);
	ImGui::ColorEdit3("back", &rgb.x);
	ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_back_sat, col_back_val);

	ImGui::Separator();
	
	//TODO: This should be serialised; Maybe in a Loop 
	ImVec4 temp_frustum;
	temp_frustum.w = frustumColor.a;
	temp_frustum.x = frustumColor.r; 
	temp_frustum.y = frustumColor.g;
	temp_frustum.z = frustumColor.b; 
	ImGui::ColorEdit3("Frustum", &temp_frustum.x);
	frustumColor = Color(float(temp_frustum.x), float(temp_frustum.y), float(temp_frustum.z), float(temp_frustum.w));
	
	ImVec4 temp_normal;
	temp_normal.w = NormalColor.a;
	temp_normal.x = NormalColor.r;
	temp_normal.y = NormalColor.g;
	temp_normal.z = NormalColor.b;
	ImGui::ColorEdit3("Normal", &temp_normal.x);
	NormalColor = Color(float(temp_normal.x), float(temp_normal.y), float(temp_normal.z), float(temp_normal.w));
	
	ImVec4 temp_AABB;
	temp_AABB.w = AABBColor.a;
	temp_AABB.x = AABBColor.r;
	temp_AABB.y = AABBColor.g;
	temp_AABB.z = AABBColor.b;
	ImGui::ColorEdit3("AABB", &temp_AABB.x);
	AABBColor = Color(float(temp_AABB.x), float(temp_AABB.y), float(temp_AABB.z), float(temp_AABB.w));

	ImVec4 temp_OBB;
	temp_OBB.w = OBBColor.a;
	temp_OBB.x = OBBColor.r;
	temp_OBB.y = OBBColor.g;
	temp_OBB.z = OBBColor.b;
	ImGui::ColorEdit3("OBB", &temp_OBB.x);
	OBBColor = Color(float(temp_OBB.x), float(temp_OBB.y), float(temp_OBB.z), float(temp_OBB.w));

	ImGui::Separator();

	ImGui::ShowStyleEditor();
	ImGui::End();

	ImGuiStyle& style = ImGui::GetStyle();

	ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
	ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
	ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
	ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);

	ExplorerIconsTint = ImVec4(col_main.x + 0.2f, col_main.y + 0.2f, col_main.z + 0.2f, 1.0f);

	style.Colors[ImGuiCol_Text] =					ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] =			ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
	style.Colors[ImGuiCol_WindowBg] =				ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_Border] =					ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] =			ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] =				ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] =			ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] =			ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TitleBg] =				ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
	style.Colors[ImGuiCol_TitleBgCollapsed] =		ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
	style.Colors[ImGuiCol_TitleBgActive] =			ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_MenuBarBg] =				ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
	style.Colors[ImGuiCol_ScrollbarBg] =			ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] =			ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] =	ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] =	ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark] =				ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] =				ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
	style.Colors[ImGuiCol_SliderGrabActive] =		ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Button] =					ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] =			ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] =			ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Header] =					ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] =			ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] =			ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] =				ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] =		ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] =		ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines] =				ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] =		ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] =			ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] =	ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] =			ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
	style.Colors[ImGuiCol_DragDropTarget] =			ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
	style.Colors[ImGuiCol_ModalWindowDarkening] =	ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	style.Colors[ImGuiCol_Tab] =					ImVec4(col_area.x, col_area.y, col_area.z, 0.62f);
	style.Colors[ImGuiCol_TabActive] =				ImVec4(col_area.x, col_area.y, col_area.z, 0.92f);
	style.Colors[ImGuiCol_TabHovered] =				ImVec4(col_area.x, col_area.y, col_area.z, 0.92f);
	style.Colors[ImGuiCol_TabUnfocused] =			ImVec4(col_area.x, col_area.y, col_area.z, 0.92f);
	style.Colors[ImGuiCol_TabUnfocusedActive] =		ImVec4(col_area.x, col_area.y, col_area.z, 0.92f);
	style.Colors[ImGuiCol_SeparatorHovered] =		ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
	style.Colors[ImGuiCol_SeparatorActive] =		ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
	style.Colors[ImGuiCol_DockingPreview] =			ImVec4(col_main.x, col_main.y, col_main.z, 0.62f);
	style.Colors[ImGuiCol_NavHighlight] =			ImVec4(col_text.x, col_text.y, col_text.z, 0.62f);
	style.Colors[ImGuiCol_NavWindowingHighlight] =	ImVec4(col_text.x, col_text.y, col_text.z, 0.62f);
	style.Colors[ImGuiCol_NavWindowingDimBg] =		ImVec4(col_text.x, col_text.y, col_text.z, 0.62f);
}

void ModuleEditor::DropTargetWindow()
{
	if (show_dropTarget_window)
	{
		ImGui::SetNextWindowSize({ 500, 400 });
		ImGui::SetNextWindowPos({ 300, 50 });

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;

		ImGui::Begin("DropTarget", &show_dropTarget_window, flags);
		
		//ImGui::Rect
		//ImGui::
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2);
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2);
		ImGui::Text("Drop asset here:");

		ImGui::SetCursorPosX(0);
		ImGui::SetCursorPosY(0);
		ImGui::InvisibleButton("Drop asset here:", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()));
		
		if (ImGui::BeginDragDropTarget())
		{
			ResourceMaterial* material = new ResourceMaterial();
			ComponentMaterial* compMaterial;
		
			//ImGuiDragDropFlags_
				
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_AcceptBeforeDelivery))
			{
				uint32 UID = *(const uint32*)payload->Data;
				Resource* resource = App->resources->GetResourceInMemory(UID);

				switch (resource->type)
				{
				case ResourceType::Model:
					App->resources->LoadResource(UID);
					break;
				case ResourceType::Texture:

					compMaterial = (ComponentMaterial*)App->scene->selected_object->GetComponent(ComponentType::Material);
					compMaterial->GetMaterial()->SetTexture((ResourceTexture*)App->resources->LoadResource(UID));
					break;
				case ResourceType::Shader:

					if ((ComponentMaterial*)App->scene->selected_object->GetComponent(ComponentType::Material) == nullptr)
					{
						break;
					}

					compMaterial = (ComponentMaterial*)App->scene->selected_object->GetComponent(ComponentType::Material);
					compMaterial->GetMaterial()->SetShader((ResourceShader*)App->resources->LoadResource(UID));

					break;
				}
				// Else make a pop up Error
			}
			ImGui::EndDragDropTarget();
			show_dropTarget_window = false;
		}
		ImGui::End();
	}
}

void ModuleEditor::TextEditorWindow()
{

	if (show_texteditor_window) 
	{
		if (show_saveeditor_popup)
		{
			ImGui::OpenPopup("Save Previous File"); 
			show_saveeditor_popup = false;
		}

		if (ImGui::BeginPopupModal("Save Previous File", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Do you want to save changes before closing the editor? \n\n");
			if (ImGui::Button("Save Changes"))
			{
				std::string textToSave = editor.GetText();
				App->fileSystem->Remove(fileToEdit.c_str());
				App->fileSystem->Save(fileToEdit.c_str(), textToSave.c_str(), editor.GetText().size());

				glDetachShader(shaderToRecompile->shaderProgramID, shaderToRecompile->vertexID);
				glDetachShader(shaderToRecompile->shaderProgramID, shaderToRecompile->fragmentID);
				glDeleteProgram(shaderToRecompile->shaderProgramID);

				Importer::ShaderImporter::Import(shaderToRecompile->assetsFile.c_str(), shaderToRecompile);

				show_saveeditor_popup = false;
				show_texteditor_window = true;

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Don't Save"))
			{
				show_saveeditor_popup = false;
				show_texteditor_window = true;

				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if (ImGui::Begin("Text Editor", &show_texteditor_window, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse))
		{

			//Update
			auto cpos = editor.GetCursorPosition();
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Save"))
					{
						std::string textToSave = editor.GetText();

						App->fileSystem->Remove(fileToEdit.c_str());
						App->fileSystem->Save(fileToEdit.c_str(), textToSave.c_str(), editor.GetText().size());
					
						glDetachShader(shaderToRecompile->shaderProgramID, shaderToRecompile->vertexID);
						glDetachShader(shaderToRecompile->shaderProgramID, shaderToRecompile->fragmentID);
						glDeleteProgram(shaderToRecompile->shaderProgramID);

						Importer::ShaderImporter::Import(shaderToRecompile->assetsFile.c_str(), shaderToRecompile);
					}
			
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit"))
				{
					bool ro = editor.IsReadOnly();
					if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
						editor.SetReadOnly(ro);
					ImGui::Separator();

					if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
						editor.Undo();
					if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
						editor.Redo();

					ImGui::Separator();

					if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
						editor.Copy();
					if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
						editor.Cut();
					if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
						editor.Delete();
					if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
						editor.Paste();

					ImGui::Separator();

					if (ImGui::MenuItem("Select all", nullptr, nullptr))
						editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("View")) //Might not be necessary? 
				{
					if (ImGui::MenuItem("Dark palette"))
						editor.SetPalette(TextEditor::GetDarkPalette());
					if (ImGui::MenuItem("Light palette"))
						editor.SetPalette(TextEditor::GetLightPalette());
					if (ImGui::MenuItem("Retro blue palette"))
						editor.SetPalette(TextEditor::GetRetroBluePalette());
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
				editor.IsOverwrite() ? "Ovr" : "Ins",
				editor.CanUndo() ? "*" : " ",
				editor.GetLanguageDefinition().mName.c_str(), fileToEdit.c_str());

			editor.Render("TextEditor");
			ImGui::End();
		}
	}
}

void ModuleEditor::CallTextEditor(ResourceMaterial* resource)
{
	//Only Handles GLSL
	TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::GLSL();

	fileToEdit = resource->GetShader()->assetsFile;
	editor.SetShowWhitespaces(false);

	std::ifstream t(fileToEdit.c_str());
	if (t.good())
	{
		std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		editor.SetText(str);
	}

	show_saveeditor_popup = true;
	show_texteditor_window = true;

	shaderToRecompile = resource->GetShader();
}

bool ModuleEditor::MainMenuBar()
{
	bool ret = true;
	std::string menuAction = "";
	if (ImGui::BeginMainMenuBar())
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

				if(!scenesInMemory.empty()) selectedScene = scenesInMemory.front()->name;

				menuAction = "Load Scene";
			}
			if(ImGui::MenuItem("Exit")) ret = false;

			ImGui::EndMenu(); 
		}
		
		//Set Next window Pos and Size
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos , ImGuiCond_Appearing, ImVec2(0.5, 0.5));
		ImGui::SetNextWindowSize(ImVec2(250.0f, 150.0f));

		//Set next Pop based on a string
		if (menuAction == "Save Scene") ImGui::OpenPopup("Save Scene Options");
		

		if (ImGui::BeginPopupModal("Save Scene Options", NULL, ImGuiWindowFlags_NoResize))
		{
			
			//ImGui::Spacing();
			//ImGui::TextColored(sceneTextColor, scene->libraryFile.c_str());
			ImGui::Spacing();
			
			if (ImGui::InputText("Scene Name:", (char*)sceneName.c_str(), 64, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				scene->libraryFile = SCENES_PATH ;
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

		if (menuAction == "Load Scene") ImGui::OpenPopup("Load Scene Options");

		if (ImGui::BeginPopupModal("Load Scene Options", NULL, ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("Choose the secne to load");
			ImGui::Spacing();
			
			if (ImGui::BeginCombo("Scenes", selectedScene.c_str(), ImGuiComboFlags_PopupAlignLeft))
			{
				for (uint i = 0; i < scenesInMemory.size(); i++)
				{
					const bool is_selected = (scenesInMemory[i]->name == selectedScene);
					if (ImGui::Selectable(scenesInMemory[i]->name.c_str(), is_selected))
					{	
						sceneToLoad = scenesInMemory[i];
						selectedScene = scenesInMemory[i]->name;
						allowSaveOrLoad = true;
					}

				}
				
				ImGui::EndCombo();
			}

			ImGui::Spacing();

			if (allowSaveOrLoad)
			{
				if (ImGui::Button("Load"))
				{
					if (sceneToLoad) App->resources->LoadResource(sceneToLoad->UID);
					sceneToLoad = nullptr;
					scenesInMemory.clear();
					allowSaveOrLoad = false;
					ImGui::CloseCurrentPopup();
				}
			}


			ImGui::Spacing();

			if (ImGui::Button("Cancel"))
			{
				scenesInMemory.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Configuration","",configWindow->isActive)) configWindow->Enable();
			if (ImGui::MenuItem("Particle Node Editor", "", nodeEditorWindow->isActive)) nodeEditorWindow->Enable();
			if (ImGui::MenuItem("Console", "", consoleWindow->isActive)) consoleWindow->Enable();
			if (ImGui::MenuItem("Hierarchy", "", hirearchyWindow->isActive)) hirearchyWindow->Enable();
			if (ImGui::MenuItem("Inspector", "", inspectorWindow->isActive)) inspectorWindow->Enable();

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Add"))
		{	
			if (ImGui::MenuItem("Create Game Object")) App->scene->CreateGameObject("New_Empty_", App->scene->root_object);
			
			if (App->scene->selected_object != nullptr)
			{
				if (ImGui::MenuItem("Create Children Object")) App->scene->CreateGameObject("New_EmptyChildren_", App->scene->selected_object);
			}

			ImGui::EndMenu();

		}
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Set Guizmo: Translate (W)","W"))	App->scene->gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			if (ImGui::MenuItem("Set Guizmo: Rotate (E)","E"))		App->scene->gizmoOperation = ImGuizmo::OPERATION::ROTATE;
			if (ImGui::MenuItem("Set Guizmo: Scale (R)","R"))		App->scene->gizmoOperation = ImGuizmo::OPERATION::SCALE;

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("ImGui Demo"))show_demo_window = !show_demo_window;
			if (ImGui::MenuItem("Documentation")) RequestBrowser("https://github.com/paufiol/AnotherSmallEngine/blob/master/README.md");
			if (ImGui::MenuItem("Latest Release")) RequestBrowser("https://github.com/paufiol/AnotherSmallEngine");
			if (ImGui::MenuItem("Report a bug")) RequestBrowser("https://github.com/paufiol/AnotherSmallEngine/issues");
			if (ImGui::MenuItem("About", "", aboutWindow->isActive)) aboutWindow->Enable();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
		ImGui::End();

	}
	return ret;
}



void ModuleEditor::Docking()
{
	ImGuiWindowFlags window = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetWindowViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Docking", &dockingWindow, window);
	ImGui::PopStyleVar(3);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id = ImGui::GetID("Dockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
}

void ModuleEditor::RequestBrowser(const char* path)
{
	ShellExecuteA(0, "Open", path, 0, "", 5);
}


