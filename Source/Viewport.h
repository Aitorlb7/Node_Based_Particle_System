#ifndef __VIEWPORT_H__
#define __VIEWPORT_H__

#include "Window.h"

#include "Dependencies/ImGuizmo/ImGuizmo.h"
#include "Dependencies/MathGeoLib/include/Math/float3.h"

class SceneWindow;
class Scene;
class ComponentCamera;
class ResourceTexture;

#define CAMERA_KEYBOARD_MULT 15.0f

class Viewport : public Window
{

public:
	Viewport(SceneWindow* parent, std::string name, ImGuiWindowClass* windowClass, int ID);
	~Viewport();

	virtual void PrepareUpdate();
	void Draw() override;

	virtual void OnResize(float2 newSize) override;

	//virtual void SetScene(Scene* scene);
	inline const ComponentCamera* GetCurrentCamera() const { return currentCamera; }


protected:

	void DrawScene();

private:

	void DrawWorldAxisGizmo();
	void DisplaySceneStats();

	

protected:
	//The scene linked to this viewport
	Scene* scene = nullptr;
	ComponentCamera* currentCamera = nullptr;

	// Scene texture management
		//Bottom-left corner of the scene image in OpenGL space coordinates (0y at the bottom of the screen)
	float2 sceneTextureScreenPosition;

	//The size of the screen texture in the window
	float2 textureSize;

	bool gridSnapActive = false;
	bool rotationSnapActive = false;
	bool scaleSnapActive = false;

	uint gridSnapIndex = 0;
	uint rotationSnapIndex = 0;
	uint scaleSnapIndex = 4;

	// Toolbar handling
	bool toolbarCollapsed = false;
	float2 gizmoRectOrigin;
	float toolbarHeight = 23.0f;
};



#endif // __VIEWPORT_H__
