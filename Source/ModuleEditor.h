#ifndef __ModuleEditor_H__
#define __ModuleEditor_H__
#include "Module.h"
#include "Color.h"
#include "Globals.h"
#include "Dependencies/ImGui/ColorTextEditor/TextEditor.h"


class GameObject;
class Resource;
class ResourceTexture;
class ResourceShader;
class ResourceMaterial;
class ResourceScene;

class SceneWindow;

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

	void DrawGUI();

	void RequestBrowser(const char*);

	void SetupStyleFromHue();

	void TextEditorWindow();

	void CallTextEditor(ResourceMaterial* resource);

	void GUIisHovered();

	SceneWindow* GetSceneWindow(const char* windowName);

public:
	std::vector<SceneWindow*> windowScenes;

	ImGuiWindowClass* parentWindowClass = nullptr;
	ImGuiWindowClass* windowClass = nullptr;


	Color frustumColor = Color(1.0f, 1.0f, 0.2f, 0.75f);
	Color NormalColor = Color(.8f, .8f, 0.0f, 0.75f);
	Color AABBColor = Color(1.0f, 0.2f, 0.2f, 0.75f);
	Color OBBColor = Color(0.2f, 0.2f, 1.0f, 0.75f);
	ImVec4 ExplorerIconsTint;

	ResourceShader* shaderToRecompile = nullptr;

	uint32 Hovered_UID = 0;

	TextEditor editor;
	std::string fileToEdit;
	bool show_demo_window = false;
	bool show_texteditor_window = false;
	bool show_saveeditor_popup = false;
	bool isUserTyping = false; 

	bool allowSaveOrLoad = false;


	bool GUIhovered = false;

	bool drawWireframe = false;

};

#endif //__ModuleEditor_H__