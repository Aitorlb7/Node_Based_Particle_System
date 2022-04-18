#include "Window.h"

class SceneWindow;

class WindowAbout : public Window
{
public:

	WindowAbout(SceneWindow* frameWindowClass, ImGuiWindowClass* windowClass, int ID, bool isActive);
	~WindowAbout();

	void Start() override;

	void Draw() override;

	void RequestBrowser(const char* path);

	void CleanUp() override;

private:

	char label[32];

};