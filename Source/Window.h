#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Globals.h"
#include "Dependencies/MathGeoLib/include/Math/float2.h"

class SceneWindow;
struct ImGuiWindowClass;

class Window
{
public:
	Window(SceneWindow* parent, ImGuiWindowClass* windowClass, int ID, std::string name, bool isActive = true);
	virtual ~Window();


	virtual void	Start();
	virtual void	SetUp();
	virtual void	Draw();
	virtual void	CleanUp();

	void Enable();
	void Disable();

	std::string	GetName() const;
	inline bool IsActive() const { return isActive; }
	inline bool IsHovered() const { return isHovered; }
	inline bool IsClicked() const { return isClicked; }
	virtual void OnResize(float2 newSize);

	inline void SetActive(bool active) { this->isActive = active; }
	void SetIsHovered();
	void SetIsClicked(const bool& setTo);

public:
	int ID = -1;
	std::string IDstring;
	bool hideTabBar;

	std::string name;
	SceneWindow* windowParent = nullptr;
	bool showDebugInfo = false;
	float2 windowSize = { 0, 0 };
	ImGuiWindowClass* windowClass = nullptr;
	bool isActive = true;
	bool isHovered = false;
	bool isClicked = false;
};

#endif // !__WINDOW_H__
