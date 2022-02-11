#ifndef __ModuleEditor_H__
#define __ModuleEditor_H__
#include "Module.h"
#include "Dependencies/ImGui/imgui.h"
#include "ResourceTexture.h"
#include "PathNode.h"
#include "Globals.h"
#include "Timer.h"
#include "Color.h"

#include "Dependencies/ImGui/ColorTextEditor/TextEditor.h"

using namespace std;

class GameObject;
class Resource;
class ResourceTexture;
class ResourceShader;
class ResourceMaterial;
class ResourceScene;

class Window;
class WindowAbout;
class WindowAssetExplorer;
class WindowConfiguration;
class WindowConsole;
class WindowHirearchy;
class WindowInspector;
class WindowPlay;

class ModuleEditor : public Module 
{
public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();

	bool Start() override;
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp() override;

	void AddWindow(Window* window);

	void DrawGUI();

	void Docking();
	void RequestBrowser(const char*);
	bool MainMenuBar();

	void SetupStyleFromHue();

	void DropTargetWindow();
	void TextEditorWindow();

	void CallTextEditor(ResourceMaterial* resource);

	void GUIisHovered();



public:

	WindowAbout* aboutWindow = nullptr;
	WindowAssetExplorer* explorerWindow = nullptr;
	WindowConfiguration* configWindow = nullptr;
	WindowConsole* consoleWindow = nullptr;
	WindowHirearchy* hirearchyWindow = nullptr;
	WindowInspector* inspectorWindow = nullptr;
	WindowPlay* playWindow = nullptr;

	Color frustumColor = Color(1.0f, 1.0f, 0.2f, 0.75f);
	Color NormalColor = Color(.8f, .8f, 0.0f, 0.75f);
	Color AABBColor = Color(1.0f, 0.2f, 0.2f, 0.75f);
	Color OBBColor = Color(0.2f, 0.2f, 1.0f, 0.75f);
	ImVec4 ExplorerIconsTint;

	bool show_demo_window = false;
	bool show_dropTarget_window = false;
	bool show_texteditor_window = false;
	bool show_saveeditor_popup = false;

	Resource* scene = nullptr;
	ResourceScene* sceneToLoad = nullptr;
	std::vector<ResourceScene*> scenesInMemory;
	std::string sceneName;
	std::string selectedScene;
	ImVec4 sceneTextColor = ImVec4(0.8, 0, 0, 1);

	ResourceShader* shaderToRecompile = nullptr;

	uint32 Hovered_UID = 0;

	TextEditor editor;
	std::string fileToEdit;

	bool dockingWindow = true;

	bool GUIhovered = false;
	bool isUserTyping = false; 

	bool enableObject = true;

	bool allowSaveOrLoad = false;


private: 
	std::vector<Window*> windows;

};

#endif //__ModuleEditor_H__