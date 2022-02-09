#include "Window.h"

class WindowConsole : public Window
{
public:

	WindowConsole(bool isActive);
	~WindowConsole();

	void Draw() override;

	void CleanUp() override;

private:

	std::vector<std::string> log_record;

	bool scrollDown = true;

};