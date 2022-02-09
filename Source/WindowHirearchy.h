#include "Window.h"

class GameObject;

class WindowHirearchy : public Window
{
public:

	WindowHirearchy(bool isActive);
	~WindowHirearchy();

	void Draw() override;

	void DrawHierarchyLevel(GameObject* rootObject);

	void CleanUp() override;

private:

	GameObject* childObject = nullptr;

};