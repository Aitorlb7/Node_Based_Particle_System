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

#include "PinFlow.h"
#include "NodeGravitationalField.h"

EmitterInstance::EmitterInstance() :
	emitterNode(nullptr),
	isActive(false),
	forceVector(float3::zero)
{
}

void EmitterInstance::Init(Emitter* emitterReference, ComponentParticleSystem* component)
{

	if (!emitterNode)
	{
		
		emitterNode = App->editor->nodeEditorWindow->CreateNode(NodeType::Emitter, "Emitter");

		PinFlow* pin = (PinFlow*)emitterNode->Outputs[0];

		pin->emitterInstance = this;
	}

	this->emitterReference = emitterReference;
	this->component = component;

	particles.resize(emitterReference->maxParticleOnScreen);
	particleIndices = new unsigned int[emitterReference->maxParticleOnScreen];

	for (uint i = 0; i < emitterReference->maxParticleOnScreen; ++i)
	{
		particleIndices[i] = i;
	}
;
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
	if (activeParticles == particles.size()) return;

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

void EmitterInstance::AddGravitationalField(NodeGravitationalField* gravitationalField)
{
	if (!gravitationalField) return;

	for (int i = 0; i < GravitationalFields.size(); ++i)
	{
		if (GravitationalFields[i] == gravitationalField) return;
	}

	GravitationalFields.push_back(gravitationalField);
}

void EmitterInstance::DeleteGravitationalField(NodeGravitationalField* gravitationalField)
{
	if (!gravitationalField) return;

	std::vector<NodeGravitationalField*>::iterator it= GravitationalFields.begin();

	for (; it != GravitationalFields.end(); ++it)
	{
		if (*it == gravitationalField)
		{
			GravitationalFields.erase(it);
			return;
		}
	}
}

void EmitterInstance::KillDeadParticles()
{
	for (int i = activeParticles - 1; i >= 0; --i)
	{
		unsigned int particleIndex = particleIndices[i];
		Particle* particle = &particles[particleIndex];

		if (particle->relativeLifetime >= 1.0f)
		{
			particleIndices[i] = particleIndices[activeParticles - 1]; 
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
