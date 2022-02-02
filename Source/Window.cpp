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

bool Window::Draw(ImGuiIO& io)
{
	return true;
}

bool Window::CleanUp()
{
	return true;
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
	return nullptr;
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
