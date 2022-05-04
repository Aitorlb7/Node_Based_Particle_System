#include "Application.h"
#include "EmitterInstance.h"
#include "Emitter.h"
#include "ParticleModule.h"

#include "ModuleRenderer3D.h"
#include "ResourceMaterial.h"
#include "Dependencies/MathGeoLib/include/Math/float4x4.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"
#include <map>
EmitterInstance::EmitterInstance() : 
	emitterNode(nullptr),
	isActive(false)
{
}

void EmitterInstance::Init(Emitter* emitterReference, ComponentParticleSystem* component)
{
	//TODO Reallocate to Modules creator
	if (App->editor->nodeEditorWindow)
	{
		emitterNode = App->editor->nodeEditorWindow->CreateParticleSystem();
	}

	this->emitterReference = emitterReference;
	this->component = component;

	particles.resize(emitterReference->maxParticleCount);
	particleIndices = new unsigned int[emitterReference->maxParticleCount];

	for (uint i = 0; i < emitterReference->maxParticleCount; ++i)
	{
		particleIndices[i] = i;
	}

	//if (!this->emitterReference->material) this->emitterReference->material = new ResourceMaterial();
}

void EmitterInstance::Update()
{
	emitterTime += App->GetDT();

	KillDeadParticles();
	UpdateModules();
}

void EmitterInstance::DrawParticles()
{
	for (unsigned int i = 0; i < activeParticles; ++i)
	{
		App->renderer3D->AddParticle(&particles[particleIndices[i]], emitterReference->material);
	}
}

void EmitterInstance::Reset()
{
	emitterTime = 0.0f;
	KillAllParticles();
}

void EmitterInstance::SpawnParticle()
{
	if (activeParticles == particles.size()) return;		//Ignoring spawn call by now when no more particles are available

	unsigned int particleIndex = particleIndices[activeParticles];
	particles[particleIndex].position = float3::zero;

	for (int i = 0; i < emitterReference->modules.size(); ++i)
	{
		emitterReference->modules[i]->Spawn(this, &particles[particleIndex], emitterNode);
	}

	++activeParticles;
}

void EmitterInstance::UpdateWithNode()
{
	for (int i = 0; i < emitterReference->modules.size(); ++i)
	{
		emitterReference->modules[i]->UpdateWithNode(this, emitterNode);
	}

	emitterNode->updateEmitter = false;
}

void EmitterInstance::KillDeadParticles()
{
	for (int i = activeParticles - 1; i >= 0; --i)
	{
		unsigned int particleIndex = particleIndices[i];
		Particle* particle = &particles[particleIndex];

		//Sending the particle to the end of the list (nice unreal trick here)
		if (particle->normalizedLifetime >= 1.0f)
		{
			particleIndices[i] = particleIndices[activeParticles - 1]; //We swap the last alive particle with the new dead one
			particleIndices[activeParticles - 1] = particleIndex;
			--activeParticles;
		}
	}
}

void EmitterInstance::KillAllParticles()
{
	activeParticles = 0;
}

void EmitterInstance::UpdateModules()
{
	for (int i = 0; i < emitterReference->modules.size(); ++i)
	{
		emitterReference->modules[i]->Update(this, emitterNode);
	}
}
