#include "Window.h"

class WindowConsole : public Window
{
public:

	WindowConsole(bool isActive);
	~WindowConsole();

	void Start() override;

	void Draw() override;

	void AddLog(std::string text);

	void CleanUp() override;

private:

	std::vector<std::string> log_record;

	bool scrollDown = true;

};