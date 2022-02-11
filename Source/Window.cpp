#include "Window.h"

Window::Window(const char* name, bool isActive): 
name(name),
isActive(isActive),
isHovered(false),
isClicked(false)
{
}

Window::~Window()
{
}

void Window::Start()
{
}

void Window::Draw()
{
}

void Window::CleanUp()
{
}

void Window::Enable()
{
	isActive = true;
}

void Window::Disable()
{
	isActive = false;
}

const char* Window::GetName() const
{
	return name;
}

void Window::SetIsHovered()
{
	if (ImGui::IsWindowHovered())
	{
		isHovered = true;
	}
	else
	{
		isHovered = false;
	}
}

void Window::SetIsClicked(const bool& setTo)
{
}
