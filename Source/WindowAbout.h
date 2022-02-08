#include "Window.h"

class WindowAbout : public Window
{
public:

	WindowAbout(bool isActive);
	~WindowAbout();

	void Draw() override;

	void RequestBrowser(const char* path);

	void CleanUp() override;

private:

	char label[32];

};