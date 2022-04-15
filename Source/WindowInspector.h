#include "Window.h"

class Component;
class ComponentCamera;
class ComponentMaterial;
class ComponentMesh;
class ComponentTransform;
class ComponentParticleSystem;
class ResourceShader;
class ResourceParticleSystem;
class TextEditor;
class Emitter;
class ParticleModule;

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
	void DrawParticleSystem(ComponentParticleSystem* component);

	std::string GetModuleName(const ParticleModule* module) const;

	void CleanUp() override;

private:

	bool enableObject = true;

	ResourceParticleSystem* rParticleSystem;
	Emitter* selectedEmitter;
	ParticleModule* selectedModule;
};