#include "Application.h"
#include "ParticleModule.h"
#include "EmitterInstance.h"
#include "Random.h"
#include "GameObject.h"

#include "ModuleCamera3D.h"

#include "ComponentParticleSystem.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "Node.h"
#include "PinFloat.h"
#include "PinFloat3.h"
#include "PinFloat4.h"
#include "PinInt.h"
#include "PinBool.h"

#include "Dependencies/MathGeoLib/include/Math/float3x3.h"

#pragma region Emitter Base

void EmitterBase::Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode)
{
	ComponentTransform* emitterTransform = (ComponentTransform*)emitter->component->gameObject->GetComponent(ComponentType::Transform);
	particle->position += emitterTransform->GetPosition() + emitterOrigin;
}

void EmitterBase::Update(EmitterInstance* emitter, Node* emitterNode)
{
	ComponentTransform* cameraTransform = nullptr;
	if (App->camera->gameCamera)
	{
		cameraTransform = (ComponentTransform*)App->camera->gameCamera->gameObject->GetComponent(ComponentType::Transform);
	}

	for (unsigned int i = 0; i < emitter->activeParticles; ++i)
	{
		unsigned int particleIndex = emitter->particleIndices[i];
		Particle* particle = &emitter->particles[particleIndex];

		particle->worldRotation = GetParticleAlignment(particle->position, cameraTransform->GetLocalTransform());
		particle->distanceToCamera = float3(cameraTransform->GetLocalTransform().TranslatePart() - particle->position).LengthSq();
	}
}

void EmitterBase::UpdateWithNode(EmitterInstance* emitter, Node* emitterNode)
{
	PinBool* pin = (PinBool*)emitterNode->GetInputPinByName("Active");

	if (pin)
	{
		emitter->isActive = pin->pinBool;
	}

}

Quat EmitterBase::GetParticleAlignment(const float3& position, const float4x4& cameraTransform)
{
	float3 fwd, up, right;
	float3 direction = float3(cameraTransform.TranslatePart() - position).Normalized();

	switch (alignment)
	{
	case(Alignment::Screen):
		fwd = cameraTransform.WorldZ().Normalized().Neg();
		up = cameraTransform.WorldY().Normalized();
		right = up.Cross(fwd).Normalized();
		break;
	case(Alignment::Camera):
		fwd = float3(cameraTransform.TranslatePart() - position).Normalized();
		right = cameraTransform.WorldY().Normalized().Cross(fwd).Normalized();
		up = fwd.Cross(right).Normalized();
		break;
	case(Alignment::AxisXY):
	{
		right = float3::unitX;
		fwd = direction.Cross(right).Normalized();
		up = fwd.Cross(right).Normalized();
		break;
	}
	case(Alignment::AxisXZ):
	{
		right = float3::unitX;
		up = direction.Cross(right).Normalized();
		fwd = right.Cross(up).Normalized();
		break;
	}
	case(Alignment::AxisYX):
	{
		up = float3::unitY;
		fwd = up.Cross(direction).Normalized();
		right = up.Cross(fwd).Normalized();
		break;
	}
	case(Alignment::AxisYZ):
	{
		up = float3::unitY;
		right = up.Cross(direction).Normalized();
		fwd = right.Cross(up).Normalized();
		break;
	}
	case(Alignment::AxisZX):
	{
		fwd = float3::unitZ;
		up = direction.Cross(fwd).Normalized();
		right = up.Cross(fwd).Normalized();
		break;
	}
	case(Alignment::AxisZY):
	{
		fwd = float3::unitZ;
		right = direction.Cross(fwd).Normalized();
		up = fwd.Cross(right).Normalized();
		break;
	}
	}
	return Quat(float3x3(right, up, fwd));
}

#pragma endregion

#pragma region Emitter Spawn

void EmitterSpawn::Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode)
{
	//TODO Create Spawn
	/*PinFloat* pin = (PinFloat*)emitterNode->GetInputPinByName("Spawn");

	spawnRatio = pin->floatNum;*/

}

void EmitterSpawn::Update(EmitterInstance* emitter, Node* emitterNode)
{

	currentTimer += App->GetDT();
	if (currentTimer > spawnRatio)
	{
		currentTimer -= spawnRatio;
		emitter->SpawnParticle();
	}
}

void EmitterSpawn::UpdateWithNode(EmitterInstance* emitter, Node* emitterNode)
{
	PinFloat* pin = (PinFloat*)emitterNode->GetInputPinByName("Spawn");

	if (pin)
	{
		spawnRatio = pin->pinFloat;
	}

}

#pragma endregion

#pragma region Emitter Area

void EmitterArea::Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode)
{
}

void EmitterArea::Update(EmitterInstance* emitter, Node* emitterNode)
{
}

void EmitterArea::UpdateWithNode(EmitterInstance* emitter, Node* emitterNode)
{
}

#pragma endregion

#pragma region Particle Position

void ParticlePosition::Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode)
{
	particle->position += initialPosition;

	
	//particle->position += Random::GenerateRandomFloat3(initialPosition1, initialPosition2);


}

void ParticlePosition::Update(EmitterInstance* emitter, Node* emitterNode)
{

}

void ParticlePosition::UpdateWithNode(EmitterInstance* emitter, Node* emitterNode)
{

	PinFloat3* pin = (PinFloat3*)emitterNode->GetInputPinByName("Initial Position");

	if (pin)
	{
		initialPosition = pin->pinFloat3;
	}

}

#pragma endregion

#pragma region Particle Rotation

void ParticleRotation::Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode)
{
	particle->rotation = initialRotation;

	
	//particle->rotation = math::Lerp(initialRotation1, initialRotation2, Random::GenerateRandomInt());
}

void ParticleRotation::Update(EmitterInstance* emitter, Node* emitterNode)
{
}

void ParticleRotation::UpdateWithNode(EmitterInstance* emitter, Node* emitterNode)
{
	PinFloat* pin = (PinFloat*)emitterNode->GetInputPinByName("Rotation");

	if (pin)
	{
		initialRotation = pin->pinFloat;
	}
}

#pragma endregion

#pragma region Particle Size

void ParticleSize::Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode)
{
	particle->size = size;
	
	//particle->size = math::Lerp(initialSize, finalSize, Random::GenerateRandomInt());
}

void ParticleSize::Update(EmitterInstance* emitter, Node* emitterNode)
{
}

void ParticleSize::UpdateWithNode(EmitterInstance* emitter, Node* emitterNode)
{
	PinFloat* pin = (PinFloat*)emitterNode->GetInputPinByName("Size");

	if (pin)
	{
		size = pin->pinFloat;
	}
}

#pragma endregion

#pragma region Particle Color

void ParticleColor::Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode)
{
	//particle->color = Random::GenerateRandomFloat4(initialColor, finalColor);

	particle->color = initialColor;

}

void ParticleColor::Update(EmitterInstance* emitter, Node* emitterNode)
{
}

void ParticleColor::UpdateWithNode(EmitterInstance* emitter, Node* emitterNode)
{
	PinFloat3* pin = (PinFloat3*)emitterNode->GetInputPinByName("Color");

	if (pin)
	{
		initialColor = float4(pin->pinFloat3, initialColor.w);
	}
}

#pragma endregion

#pragma region Particle Lifetime

void ParticleLifetime::Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode)
{
	//float lifetime = Random::GenerateRandomIntRange(lifetime1,lifetime2);
	//particle->normalizedLifetime = 1.0f / lifetime;
	//particle->maxLifetime = math::Max(lifetime1, lifetime2);
	//particle->relativeLifetime = 0.0f;


	particle->normalizedLifetime = 1.0f / lifetime;
	particle->maxLifetime = lifetime;
	particle->relativeLifetime = 0.0f;
}

void ParticleLifetime::Update(EmitterInstance* emitter, Node* emitterNode)
{
	for (uint i = 0; i < emitter->activeParticles; ++i)
	{
		uint particleIndex = emitter->particleIndices[i];
		Particle* particle = &emitter->particles[particleIndex];

		particle->relativeLifetime += particle->normalizedLifetime * App->GetDT();
	}
}

void ParticleLifetime::UpdateWithNode(EmitterInstance* emitter, Node* emitterNode)
{
	PinFloat* pin = (PinFloat*)emitterNode->GetInputPinByName("Lifetime");

	if (pin)
	{
		lifetime = pin->pinFloat;
	}
}

#pragma endregion

#pragma region Particle Velocity

void ParticleVelocity::Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode)
{
	particle->velocity = float4(initialVelocity);
}

void ParticleVelocity::Update(EmitterInstance* emitter, Node* emitterNode)
{
	for (int i = emitter->activeParticles - 1; i >= 0; --i)
	{
		Particle* particle = &emitter->particles[emitter->particleIndices[i]];

		particle->position += particle->velocity.Float3Part() * particle->velocity.w * App->GetDT();
	}
}

void ParticleVelocity::UpdateWithNode(EmitterInstance* emitter, Node* emitterNode)
{

	PinFloat4* pin = (PinFloat4*)emitterNode->GetInputPinByName("Velocity");
	
	if (pin)
	{
		initialVelocity = pin->pinFloat4;
	}

}

#pragma endregion
