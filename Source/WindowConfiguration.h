#include "Window.h"

class WindowConfiguration : public Window
{
public:

	WindowConfiguration(bool isActive);
	~WindowConfiguration();

	void Start() override;

	void Draw() override;

	const char* GetSystemCaps();

	int GetBudget();

	int GetUsage();

	int GetAvailable();

	int GetReserved();

	void CleanUp() override;

public:
	std::string caps;

	float brightness = 0;
	int fpsCap = 0;
	char title[25];

	bool fullscreen = false;
	bool resizable = true;
	bool borderless = false;
	bool full_desktop = false;

	bool depthtest = false;
	bool cullface = true;
	bool lighting = true;
	bool colormaterial = false;
	bool texture2D = true;
	bool cubemap = false;
	bool polygonssmooth = false;

	bool drawNormals = false;
	bool drawWireframe = false;
	bool drawCheckerTex = false;
	bool drawTexture = true;


	int window_width = 0;
	int window_height = 0;



};