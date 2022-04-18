#include "Window.h"
#include "SceneWindow.h"

#include "Dependencies/ImGui/imgui.h"


Window::Window(SceneWindow* parent, ImGuiWindowClass* windowClass, int ID, std::string name, bool isActive) :
windowParent(parent),
name(name),
isActive(isActive),
isHovered(false),
isClicked(false),
windowClass(windowClass), 
ID(ID)
{
	IDstring = name + ("##") + std::to_string(ID);
}

Window::~Window()
{
}

void Window::Start()
{
}

void Window::SetUp()
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

std::string Window::GetName() const
{
	return name;
}

void Window::OnResize(float2 newSize)
{
	windowSize = newSize;
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
