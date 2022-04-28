#include "ComponentParticleSystem.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"

ComponentParticleSystem::ComponentParticleSystem(GameObject* gameObject) : Component(gameObject)
{
	rParticleSystem = new ResourceParticleSystem();
	type = ComponentType::ParticleSystem;

	//TODO Reallocate to Modules creator
	if (App->editor->nodeEditorWindow)
	{
		Node* defaultEmiiter = App->editor->nodeEditorWindow->CreateEmiterNode();
		emittersNodes.push_back(defaultEmiiter);
	}
}

ComponentParticleSystem::ComponentParticleSystem(GameObject* gameObject, const char* meshPath, ResourceParticleSystem* _mesh) : Component(gameObject)
{
	rParticleSystem = new ResourceParticleSystem();
	type = ComponentType::ParticleSystem;
}

ComponentParticleSystem::~ComponentParticleSystem()
{
}

void ComponentParticleSystem::Enable()
{
}

void ComponentParticleSystem::Update()
{
	for (uint i = 0; i < emitters.size(); ++i)
	{
		emitters[i].Update();

		//TODO: This shoudnt be here

		emitters[i].DrawParticles();
	}
}

void ComponentParticleSystem::Reset()
{
	for (uint i = 0; i < emitters.size(); ++i)
	{
		emitters[i].Reset();
	}
}

void ComponentParticleSystem::Disable()
{
}

void ComponentParticleSystem::CleanUp()
{
	rParticleSystem = nullptr;
	delete rParticleSystem;
}

void ComponentParticleSystem::SetResourceProperties(ResourceParticleSystem* particleSystem)
{
	Reset();
	emitters.clear();

	for (uint i = 0; i < particleSystem->emitters.size(); ++i)
	{
		emitters.push_back(EmitterInstance());
		emitters.back().Init(&particleSystem->emitters[i], this);
	}
}

void ComponentParticleSystem::SetParticleSystem(ResourceParticleSystem* particleSystem)
{
	this->rParticleSystem = particleSystem;
}

void ComponentParticleSystem::SetPath(std::string path)
{
	this->path = path;
}

void ComponentParticleSystem::SetResourceID(uint32 ID)
{
	rParticleSystem->UID = ID;
}
