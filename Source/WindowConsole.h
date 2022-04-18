#include "Window.h"

class SceneWindow;

class WindowConsole : public Window
{
public:

	WindowConsole(SceneWindow* parent, ImGuiWindowClass* windowClass, int ID, bool isActive);
	~WindowConsole();

	void Start() override;

	void Draw() override;

	void AddLog(std::string text);

	void CleanUp() override;

private:

	std::vector<std::string> log_record;

	bool scrollDown = true;

};