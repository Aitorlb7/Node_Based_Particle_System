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

	void Draw() override;

	void DrawComponent(Component* component);
	void DrawTransform(ComponentTransform* component);
	void DrawMesh(ComponentMesh* component);
	void DrawMaterial(ComponentMaterial* component);
	void DrawCamera(ComponentCamera* component);

	void CallTextEditor(ResourceMaterial* resource);

	void CleanUp() override;

private:
	ResourceShader* shaderToRecompile = nullptr;
	bool enableObject = true;

	TextEditor editor;
	std::string fileToEdit;
};