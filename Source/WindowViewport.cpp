#include "WindowViewport.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"

#include "GameObject.h"

#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "imgui_internal.h"

#define MARGIN_OFFSET 22.5f

WindowViewport::WindowViewport(bool isActive) : Window("Viewport Window", isActive),
textureOrigin({ 0.0f, 0.0f }),
textureSize({ 0.0f, 0.0f }),
cursorPos({ 0.0f, 0.0f }),
guizmoOperation(ImGuizmo::OPERATION::TRANSLATE),
guizmoMode(ImGuizmo::MODE::WORLD),
usingGuizmo(false),
sceneFocused(false)
{
}

WindowViewport::~WindowViewport()
{
}

void WindowViewport::Start()
{
}

void WindowViewport::Draw()
{
	if (!isActive) return;

	ImGui::Begin("Viewport", &isActive);

	/*if ((EngineApp->gameState != GameState::PLAY) && !sceneFocused)
	{
		ImGui::FocusWindow(ImGui::GetCurrentWindow());
		ImGui::FocusWindow(nullptr);

		sceneFocused = true;
		gameFocused = false;
	}

	if (sceneFocused)
	{
		SetIsHovered();
	}*/

	AdaptTextureToWindowSize();

	DrawSceneTexture();

	HandleGuizmos();

	DragAndDropTarget();


	ImGui::End();
}

void WindowViewport::CleanUp()
{
}

float2 WindowViewport::GetWorldMousePosition()
{

	float2 screenMousePos = float2(App->input->GetMouseX(), App->window->Height() - App->input->GetMouseY()) - float2(textureOrigin.x, textureOrigin.y + MARGIN_OFFSET);
	float2 normScreenPos = float2(screenMousePos.x / textureSize.x, screenMousePos.y / textureSize.y);
	float2 worldMousePos = float2(normScreenPos.x * App->window->Width(), normScreenPos.y * App->window->Height());

	return worldMousePos;
}

float2 WindowViewport::GetScreenMousePosition()
{

	float2 normWorldPos = float2(App->input->GetMouseX() / App->window->Width(), App->input->GetMouseY() / App->window->Height());
	float2 screenMousePos = float2(normWorldPos.x * textureSize.x, normWorldPos.y * textureSize.y);
	screenMousePos += float2(textureOrigin.x, textureOrigin.y);

	return screenMousePos;
}

float2 WindowViewport::GetWorldMouseMotion()
{
	float2 winMouseMotion = float2((float)App->input->GetMouseXMotion(), (float)App->input->GetMouseYMotion());
	float2 winSize = float2((float)App->window->Width(), (float)App->window->Height());

	float2 localMotion = float2(winMouseMotion.x / textureSize.x, winMouseMotion.y / textureSize.y);
	float2 worldMouseMotion = float2(localMotion.x * winSize.x, localMotion.y * winSize.y);

	return worldMouseMotion;
}

void WindowViewport::AdaptTextureToWindowSize()
{
	textureSize = ImVec2((float)App->window->Width(), (float)App->window->Height());
	ImVec2 winSize = ImGui::GetWindowSize() * 0.975f;									

	float widthRatio = (textureSize.x / winSize.x);										
	float heightRatio = (textureSize.y / winSize.y);									

	textureSize = textureSize.x > winSize.x ? textureSize / widthRatio : textureSize;

	textureSize = textureSize.y > winSize.y ? textureSize / heightRatio : textureSize;

}

void WindowViewport::DrawSceneTexture()
{
	cursorPos = (ImGui::GetWindowSize() - textureSize) * 0.5f;
	ImGui::SetCursorPos(cursorPos);

	ImVec2 screenCursorPos = ImGui::GetCursorScreenPos();

	if (screenCursorPos.x > 1920)														
	{
		screenCursorPos.x = screenCursorPos.x - 1920;
	}

	textureOrigin = screenCursorPos + ImVec2(0, textureSize.y);		
	textureOrigin.y = (float)App->window->Height() - textureSize.y;	

	ImGui::Image((ImTextureID)App->renderer3D->GetSceneRenderTexture(), textureSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
}

void WindowViewport::DragAndDropTarget()
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset"))
			App->editor->HandleDragAndDrop(payload);

		ImGui::EndDragDropTarget();
	}

}

void WindowViewport::HandleGuizmos()
{
	usingGuizmo = false;

	if (isHovered)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_STATE::KEY_DOWN)
		{
			guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			guizmoMode = ImGuizmo::MODE::WORLD;
		}
		else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_STATE::KEY_DOWN)
		{
			guizmoOperation = ImGuizmo::OPERATION::ROTATE;
			guizmoMode = ImGuizmo::MODE::WORLD;
		}
		else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_STATE::KEY_DOWN)
		{
			guizmoOperation = ImGuizmo::OPERATION::SCALE;
			guizmoMode = ImGuizmo::MODE::LOCAL;
		}
	}

	if (App->scene->selected_object == nullptr)
		return;

	ComponentTransform* selectedTransform = (ComponentTransform*)App->scene->selected_object->GetComponent(ComponentType::Transform);

	float4x4 viewMatrix = App->camera->currentCamera->frustum.ViewMatrix();
	float4x4 projectionMatrix = App->camera->currentCamera->frustum.ProjectionMatrix();
	float4x4 worldTransform = selectedTransform->GetGlobalTransform();
	viewMatrix.Transpose();																				// MathGeoLib works with Row-Major matrices and ImGuizmo works with
	projectionMatrix.Transpose();																		// Column-Major matrices. Hence the need to transpose them.
	worldTransform.Transpose();																			// ----------------------------------------------------------------

	ImGuizmo::SetDrawlist();

	ImVec2 originPos = ImVec2(textureOrigin.x, App->window->Height() - textureOrigin.y - textureSize.y);

	ImGuizmo::SetRect(textureOrigin.x, originPos.y, textureSize.x, textureSize.y);

	ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), guizmoOperation, guizmoMode, worldTransform.ptr());

	if (ImGuizmo::IsUsing())
	{
		usingGuizmo = true;

		worldTransform = worldTransform.Transposed();
		selectedTransform->SetLocalTransform(worldTransform);
		//SET GLOBAL??
	}

}
