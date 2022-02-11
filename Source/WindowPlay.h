#include "Window.h"

class Resource;

class WindowPlay : public Window
{
public:

	WindowPlay(bool isActive);
	~WindowPlay();

	void Start() override;

	void Draw() override;

	void CleanUp() override;

private:

	Resource* scene = nullptr;

};