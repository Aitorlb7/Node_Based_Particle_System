#ifndef __SCENE_WINDOW_H__
#define __SCENE_WINDOW_H__

#include "Globals.h"

#include <vector>
#include <string>


class Window;
class Scene;
class Config;
class Resource;

struct ImGuiWindow;
struct ImGuiDockNode;
struct ImGuiWindowClass;

typedef unsigned int ImGuiID;

class SceneWindow
{
public:
	SceneWindow(const char* name, ImGuiWindowClass* frameWindowClass, ImGuiWindowClass* windowClass, int ID);
	virtual ~SceneWindow();

	virtual void SetUp();
	void Draw();

	virtual void MainMenuBar() = 0;

	inline const char* GetName() const { return name.c_str(); }
	inline const char* GetWindowStrID() const { return windowStrID.c_str(); }

	inline uint GetID() const { return ID; };
	Window* GetWindow(const char* name) const;
	inline const std::vector<Window*> GetWindows() const { return windows; }

public:
	bool requestClose = false;

	bool pendingLoadLayout = false;

	bool checkedForSave = false;

protected:
	std::string name;
	std::string displayName;
	std::string windowStrID;
	std::vector<Window*> windows;
	bool isDockable = true;
	ImGuiWindowClass* frameWindowClass = nullptr;
	ImGuiWindowClass* windowClass = nullptr;
	int ID = -1;
	Scene* scene = nullptr;

};

#endif //__SCENE_WINDOW_H__

