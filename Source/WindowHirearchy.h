#include "Window.h"

class GameObject;
class SceneWindow;

class WindowHirearchy : public Window
{
public:

	WindowHirearchy(SceneWindow* parent, ImGuiWindowClass* windowClass, int ID, bool isActive);
	~WindowHirearchy();

	void Start() override;

	void Draw() override;

	void DrawHierarchyLevel(GameObject* rootObject);

	void CleanUp() override;

private:

	GameObject* childObject = nullptr;

};