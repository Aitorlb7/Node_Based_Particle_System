#include "Window.h"

class Component;
class ComponentCamera;
class ComponentMaterial;
class ComponentMesh;
class ComponentTransform;
class ResourceShader;
class TextEditor;

class WindowInspector : public Window
{
public:

	WindowInspector(bool isActive);
	~WindowInspector();

	void Start() override;

	void Draw() override;

	void DrawComponent(Component* component);
	void DrawTransform(ComponentTransform* component);
	void DrawMesh(ComponentMesh* component);
	void DrawMaterial(ComponentMaterial* component);
	void DrawCamera(ComponentCamera* component);

	void CleanUp() override;

private:

	bool enableObject = true;
};