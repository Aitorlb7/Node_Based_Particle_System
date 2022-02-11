#include "WindowHirearchy.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleScene.h"

WindowHirearchy::WindowHirearchy(bool isActive): Window("Hirearchy Window", isActive)
{
}

WindowHirearchy::~WindowHirearchy()
{
}

void WindowHirearchy::Start()
{
}

void WindowHirearchy::Draw()
{
	if (!isActive) return;

	ImGui::Begin("Hierarchy", &isActive);

	DrawHierarchyLevel(App->scene->root_object);

	ImGui::End();

}

void WindowHirearchy::DrawHierarchyLevel(GameObject* rootObject)
{
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (rootObject->children.empty()) treeFlags |= ImGuiTreeNodeFlags_Leaf;
	if (rootObject == App->scene->selected_object) treeFlags |= ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Framed;
	ImGui::AlignTextToFramePadding();

	if (ImGui::TreeNodeEx(rootObject->name.c_str(), treeFlags))
	{
		if (ImGui::IsItemClicked())						// To select Scene or the House needs to be opened
		{
			App->scene->SelectObject(rootObject);
			rootObject->selected = true;
		}
		if (rootObject != App->scene->root_object)
		{
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("Dragged_Object", rootObject, sizeof(GameObject));
				childObject = rootObject;
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Dragged_Object"))
				{
					rootObject->AddChildren(childObject);

					childObject = nullptr;
				}

				ImGui::EndDragDropTarget();
			}
		}
		if (!rootObject->children.empty())
		{
			for (uint i = 0; i < rootObject->children.size(); ++i)
			{
				DrawHierarchyLevel(rootObject->children[i]);
			}
		}

		ImGui::TreePop();
	}
};

void WindowHirearchy::CleanUp()
{
}
