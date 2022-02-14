#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Globals.h"

class Window
{
public:
	Window(const char* name, bool isActive = true);
	virtual ~Window();


	virtual void	Start();
	virtual void	Draw();
	virtual void	CleanUp();

	void Enable();
	void Disable();

	const char*	GetName() const;
	inline bool IsActive() const { return isActive; }
	inline bool IsHovered() const { return isHovered; }
	inline bool IsClicked() const { return isClicked; }

	void SetIsHovered();
	void SetIsClicked(const bool& setTo);

public:
	const char* name;
	bool isActive = true;
	bool isHovered = false;
	bool isClicked = false;
};

#endif // !__WINDOW_H__
