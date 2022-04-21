#include "Window.h"
#include "Dependencies/MathGeoLib/include/Math/float2.h"
#include "Dependencies/ImGuizmo/ImGuizmo.h"
#include "Dependencies/ImGui/ImGui.h"

class ResourceTexture;

class WindowViewport : public Window
{
public:

	WindowViewport(bool isActive);
	~WindowViewport();

	void Start() override;

	void Draw() override;

	void CleanUp() override;


public:
	float2	GetWorldMousePosition();
	//float2	GetScreenMousePosition();
	float2	GetWorldMouseMotion();

	inline float2 GetSceneTextureSize() const { return float2(textureSize.x, textureSize.y); };
	inline bool	UsingGuizmo() const { ImGuizmo::IsUsing(); };
	inline bool	HoveringGuizmo() const { ImGuizmo::IsOver(); };

	inline void SetImGuizmoOperation(ImGuizmo::OPERATION newOperation) { guizmoOperation = newOperation; };

private:
	void	DrawScene();
	void	DrawGame();

	void	AdaptTextureToWindowSize();
	void	AdaptTextureToGameWindowSize();
	void	DrawSceneTexture();

	void	DragAndDropTarget();

	void	HandleGuizmos();

private:

	ImVec2					textureSize;
	ImVec2					textureOrigin;
	ImVec2					cursorPos;

	ImGuizmo::OPERATION		guizmoOperation;
	ImGuizmo::MODE			guizmoMode;

	bool					usingGuizmo;

	bool					sceneFocused;

};