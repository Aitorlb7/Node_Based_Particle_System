#include "Application.h"
#include "ParticleModule.h"
#include "EmitterInstance.h"
#include "Random.h"
#include "GameObject.h"

#include "ModuleCamera3D.h"

#include "ComponentParticleSystem.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "Dependencies/MathGeoLib/include/Math/float3x3.h"

#pragma region Emitter Base

void EmitterBase::Spawn(EmitterInstance* emitter, Particle* particle)
{
	ComponentTransform* emitterTransform = (ComponentTransform*)emitter->component->gameObject->GetComponent(ComponentType::Transform);
	particle->position += emitterTransform->GetPosition() + emitterOrigin;
}

void EmitterBase::Update(EmitterInstance* emitter)
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

void EmitterSpawn::Spawn(EmitterInstance* emitter, Particle* particle)
{
	//TODO Create Spawn
}

void EmitterSpawn::Update(EmitterInstance* emitter)
{
	currentTimer += App->GetDT();
	if (currentTimer > spawnRatio)
	{
		currentTimer -= spawnRatio;
		emitter->SpawnParticle();
	}
}

#pragma endregion

#pragma region Emitter Area

void EmitterArea::Spawn(EmitterInstance* emitter, Particle* particle)
{
}

void EmitterArea::Update(EmitterInstance* emitter)
{
}

#pragma endregion

#pragma region Particle Position

void ParticlePosition::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->position += Random::GenerateRandomFloat3(initialPosition1, initialPosition2);

}

void ParticlePosition::Update(EmitterInstance* emitter)
{
}

#pragma endregion

#pragma region Particle Rotation

void ParticleRotation::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->rotation = math::Lerp(initialRotation1, initialRotation2, Random::GenerateRandomInt());
}

void ParticleRotation::Update(EmitterInstance* emitter)
{
}

#pragma endregion

#pragma region Particle Size

void ParticleSize::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->size = math::Lerp(initialSize, finalSize, Random::GenerateRandomInt());
}

void ParticleSize::Update(EmitterInstance* emitter)
{
}

#pragma endregion

#pragma region Particle Color

void ParticleColor::Spawn(EmitterInstance* emitter, Particle* particle)
{
	particle->color = Random::GenerateRandomFloat4(initialColor, finalColor);
}

void ParticleColor::Update(EmitterInstance* emitter)
{
}

#pragma endregion

#pragma region Particle Lifetime

void ParticleLifetime::Spawn(EmitterInstance* emitter, Particle* particle)
{
	float lifetime = Random::GenerateRandomIntRange(lifetime1,lifetime2);
	particle->normalizedLifetime = 1.0f / lifetime;
	particle->maxLifetime = math::Max(lifetime1, lifetime2);
	particle->relativeLifetime = 0.0f;
}

void ParticleLifetime::Update(EmitterInstance* emitter)
{
	for (uint i = 0; i < emitter->activeParticles; ++i)
	{
		uint particleIndex = emitter->particleIndices[i];
		Particle* particle = &emitter->particles[particleIndex];

		particle->relativeLifetime += particle->normalizedLifetime * App->GetDT();
	}
}

#pragma endregion

#pragma region Particle Velocity

void ParticleVelocity::Spawn(EmitterInstance* emitter, Particle* particle)
{
	/*float3 direction = float3(Random::GenerateRandomIntRange(initialVelocity1.x, initialVelocity2.x), 
							  Random::GenerateRandomIntRange(initialVelocity1.y, initialVelocity2.y), 
							  Random::GenerateRandomIntRange(initialVelocity1.z, initialVelocity2.z));*/

	float3 direction = Random::GenerateRandomFloat3(initialVelocity1.xyz(), initialVelocity2.xyz());

	particle->velocity = float4(direction, initialVelocity1.w);
}

void ParticleVelocity::Update(EmitterInstance* emitter)
{
	for (int i = emitter->activeParticles - 1; i >= 0; --i)
	{
		Particle* particle = &emitter->particles[emitter->particleIndices[i]];

		particle->position += particle->velocity.Float3Part() * particle->velocity.w * App->GetDT();
	}
}

#pragma endregion
