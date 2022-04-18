#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__
#include "SceneWindow.h"
#include "Color.h"
#include "Globals.h"
#include "Dependencies/ImGui/ColorTextEditor/TextEditor.h"


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
class WindowNodeEditor;

class MainWindow : public SceneWindow
{
public:
	MainWindow(ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID);
	~MainWindow();

	static inline const char* GetName() { return "MainWindow"; };

	virtual void MainMenuBar() override;
	void MenuItem_File(std::string menuAction);
	void MenuItem_Save(std::string menuAction);
	void MenuItem_Load(std::string menuAction);
	void MenuItem_View();
	void MenuItem_Add();
	void MenuItem_Tools();
	void MenuItem_Help();



public:

	WindowAbout* aboutWindow = nullptr;
	WindowAssetExplorer* explorerWindow = nullptr;
	WindowConfiguration* configWindow = nullptr;
	WindowConsole* consoleWindow = nullptr;
	WindowHirearchy* hirearchyWindow = nullptr;
	WindowInspector* inspectorWindow = nullptr;
	WindowPlay* playWindow = nullptr;
	WindowNodeEditor* nodeEditorWindow = nullptr;

	Color frustumColor = Color(1.0f, 1.0f, 0.2f, 0.75f);
	Color NormalColor = Color(.8f, .8f, 0.0f, 0.75f);
	Color AABBColor = Color(1.0f, 0.2f, 0.2f, 0.75f);
	Color OBBColor = Color(0.2f, 0.2f, 1.0f, 0.75f);
	ImVec4 ExplorerIconsTint;

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

	bool isUserTyping = false;

	bool enableObject = true;

	bool allowSaveOrLoad = false;

};

#endif //__MAIN_WINDOW_H__