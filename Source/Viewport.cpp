#include "Application.h"
#include "Viewport.h"
#include "Component.h"
#include "GameObject.h"

#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "ModuleResource.h"

#include "ResourceMaterial.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "Dependencies/ImGui/imgui.h"
#include "Dependencies/ImGui/imgui_internal.h"

Viewport::Viewport(SceneWindow* parent, std::string name, ImGuiWindowClass* windowClass, int ID) : Window(parent, windowClass, ID, name, isActive )
{
	ImGuizmo::Enable(true);
}

Viewport::~Viewport()
{
}

void Viewport::PrepareUpdate()
{
	currentCamera->SetAspectRatio(textureSize.x / textureSize.y);

	/*if (cameraSettingsNeedUpdate)
	{
		

		cameraSettingsNeedUpdate = false;
	}*/
}

void Viewport::Draw()
{
	ImGui::SetNextWindowClass(windowClass);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
	if (!ImGui::Begin(IDstring.c_str(), &isActive, flags) || scene == nullptr)
	{
		ImGui::End();
		ImGui::PopStyleVar();
		return;
	}
	ImGui::PopStyleVar();

	//TODO: Can we check for resizing generically for all windows?
	float2 currentSize = float2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
	if (!currentSize.Equals(windowSize) || hideTabBar != (!ImGui::GetCurrentWindow()->DockNode || ImGui::GetCurrentWindow()->DockNode->IsHiddenTabBar()))
		OnResize(currentSize);

	DrawScene();
	DrawWorldAxisGizmo();
	DisplaySceneStats();

	ImGui::End();
}

void Viewport::OnResize(float2 newSize)
{
	//Getting window size - some margins - separator (7)
	Window::OnResize(newSize);

	ImGuiContext& g = *ImGui::GetCurrentContext();

	float tabBarSize = 0;
	if (!hideTabBar)
		tabBarSize = g.FontSize + g.Style.FramePadding.y * 2.0f;

	textureSize.x = windowSize.x - 1; //<-- -4 why??? It's not FramePadding nor WindowPadding...
	textureSize.y = windowSize.y - tabBarSize - 1; //For some reason we need to subtract it again...

}

void Viewport::DrawScene()
{
	gizmoRectOrigin = float2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 25.0f); //<-- We need to subtract 25.0f which is the size of the header bar...
	sceneTextureScreenPosition = float2(gizmoRectOrigin.x, App->window->Height() - gizmoRectOrigin.y - textureSize.y);

	ImGui::Image((ImTextureID)currentCamera->GetRenderTarget(), ImVec2(textureSize.x, textureSize.y), ImVec2(0, 1), ImVec2(1, 0));


	if (ImGui::BeginDragDropTarget())
	{
		ResourceMaterial* material = new ResourceMaterial();
		ComponentMaterial* compMaterial;

		//ImGuiDragDropFlags_

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
		{
			uint32 UID = *(const uint32*)payload->Data;
			Resource* resource = App->resources->GetResourceInMemory(UID);

			switch (resource->type)
			{
			case ResourceType::Model:
				App->resources->LoadResource(UID);
				break;
			case ResourceType::Texture:

				compMaterial = (ComponentMaterial*)App->scene->selected_object->GetComponent(ComponentType::Material);
				compMaterial->GetMaterial()->SetTexture((ResourceTexture*)App->resources->LoadResource(UID));
				break;
			case ResourceType::Shader:

				if ((ComponentMaterial*)App->scene->selected_object->GetComponent(ComponentType::Material) == nullptr)
				{
					break;
				}

				compMaterial = (ComponentMaterial*)App->scene->selected_object->GetComponent(ComponentType::Material);
				compMaterial->GetMaterial()->SetShader((ResourceShader*)App->resources->LoadResource(UID));

				break;
			}
			// Else make a pop up Error
		}
		ImGui::EndDragDropTarget();
	}
}

void Viewport::DrawWorldAxisGizmo()
{
}

void Viewport::DisplaySceneStats()
{
}
