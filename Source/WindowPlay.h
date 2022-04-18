#include "Window.h"

class Resource;
class SceneWindow;

class WindowPlay : public Window
{
public:

	WindowPlay(SceneWindow* parent, ImGuiWindowClass* windowClass, int ID, bool isActive);
	~WindowPlay();

	void Start() override;

	void Draw() override;

	void CleanUp() override;

private:

	Resource* scene = nullptr;

};