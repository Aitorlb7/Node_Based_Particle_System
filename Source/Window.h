#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <string>
#include <vector>
#include "Module.h"
#include "Dependencies/ImGui/imgui.h"

class Window
{
public:
	Window(const char* name, bool isActive = true);
	virtual ~Window();

	virtual bool	Draw(ImGuiIO& io);
	virtual bool	CleanUp();

	void Enable();
	void Disable();

	const char*	GetName() const;
	inline bool IsActive() const { return isActive; }
	inline bool IsHovered() const { return isHovered; }
	inline bool IsClicked() const { return isClicked; }

	void SetIsHovered();
	void SetIsClicked(const bool& setTo);

private:
	const char* name;
	bool isActive = true;
	bool isHovered = false;
	bool isClicked = false;
};

#endif // !__WINDOW_H__
