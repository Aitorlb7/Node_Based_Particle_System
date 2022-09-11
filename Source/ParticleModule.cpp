#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ParticleModule.h"
#include "EmitterInstance.h"
#include "Random.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"
#include "WindowNodeEditor.h"

#include "ComponentParticleSystem.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"

#include "ResourceMesh.h"
#include "ResourceMaterial.h"

#include "NodeGravitationalField.h"
#include "Node.h"
#include "PinFloat.h"
#include "PinFloat3.h"
#include "PinFloat4.h"
#include "PinFloat4Array.h"
#include "PinInt.h"
#include "PinBool.h"
#include "PinTexture.h"
#include "PinGameObject.h"

#include "Dependencies/MathGeoLib/include/Math/float3x3.h"

#pragma region Emitter Base

void EmitterBase::Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode)
{
	ComponentTransform* emitterTransform = (ComponentTransform*)emitter->component->gameObject->GetComponent(ComponentType::Transform);
	particle->position += emitterTransform->GetPosition() + emitterOrigin;

	particle->lerpCounter = 0.0f;


	ComponentTransform* cameraTransform = nullptr;

	if (App->camera->currentCamera)
	{
		cameraTransform = (ComponentTransform*)App->camera->currentCamera->gameObject->GetComponent(ComponentType::Transform);
	}
	for (unsigned int i = 0; i < emitter->activeParticles; ++i)
	{
		unsigned int particleIndex = emitter->particleIndices[i];
		Particle* particle = &emitter->particles[particleIndex];

		particle->worldRotation = GetParticleAlignment(particle->position, cameraTransform->GetLocalTransform());

		particle->distanceToCamera = float3(cameraTransform->GetLocalTransform().TranslatePart() - particle->position).LengthSq();
	}
}

void EmitterBase::Update(EmitterInstance* emitter, Node* emitterNode)
{
	//TODO: Remake unnecessary 

	ComponentTransform* cameraTransform = nullptr;

	if (App->camera->currentCamera)
	{
		cameraTransform = (ComponentTransform*)App->camera->currentCamera->gameObject->GetComponent(ComponentType::Transform);
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
	//TODO: Split into several Modules

	//Set Active
	PinBool* pinBool = (PinBool*)emitterNode->GetInputPinByName("Active");

	if (pinBool) emitter->isActive = pinBool->pinBool;

	//Set Texture
	PinTexture* pinTex = (PinTexture*)emitterNode->GetInputPinByName("Texture");

	if (pinTex) emitter->emitterReference->material->SetTexture(pinTex->pinTexture);

	//Set Alignment
	PinInt* pinint = (PinInt*)emitterNode->GetInputPinByName("Alignment");

	if (pinint) alignment = (Alignment)pinint->pinInt;


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
	if (spawnRatio < 1.0f)
	{
		for (int i = 0; i < (1.0f - spawnRatio) * 10.f; ++i)
		{
			emitter->SpawnParticle();
		}

		return;
	}


	currentTimer += App->GetDT();
	if (currentTimer > spawnRatio)
	{
		currentTimer -= spawnRatio;
		emitter->SpawnParticle();
	}
}

void EmitterSpawn::UpdateWithNode(EmitterInstance* emitter, Node* emitterNode)
{
	PinFloat* pin = (PinFloat*)emitterNode->GetInputPinByName("Spawn Rate");

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
	PinGameObject* pinGameObject = (PinGameObject*)emitterNode->GetInputPinByName("Spawn From Model");

	if (!pinGameObject->gameObject)
	{
		particle->position += initialPosition;
		return;
	}

	ResourceMesh* mesh = ((ComponentMesh*)pinGameObject->gameObject->GetComponent(ComponentType::Mesh))->GetMesh();

	particle->position = pinGameObject->gameObject->transform->GetPosition();

	int size = mesh->size[ResourceMesh::index];

	int randomIndex = Random::GenerateRandomIntRange(0, size - 3);


	particle->position.x += (mesh->vertices[(mesh->indices[randomIndex] * 3) + 1] * pinGameObject->gameObject->transform->GetScale().x);
	particle->position.y += (mesh->vertices[(mesh->indices[randomIndex] * 3) + 2] * pinGameObject->gameObject->transform->GetScale().y);
	particle->position.z += (mesh->vertices[(mesh->indices[randomIndex] * 3)] * pinGameObject->gameObject->transform->GetScale().z);

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
	PinFloat4Array* pinFloatArray = (PinFloat4Array*)emitterNode->GetInputPinByName("Color");

	Pin* linkedPin = App->editor->nodeEditorWindow->GetPinLinkedTo(pinFloatArray->ID);

	if (linkedPin)
	{
		particle->color = pinFloatArray->float4Array[0];
	}
	else
	{
		particle->color = float4::one;
	}



}

void ParticleColor::Update(EmitterInstance* emitter, Node* emitterNode)
{
}

void ParticleColor::UpdateWithNode(EmitterInstance* emitter, Node* emitterNode)
{
	PinFloat4Array* pinFloatArray = (PinFloat4Array*)emitterNode->GetInputPinByName("Color");

	Pin* linkedPin = App->editor->nodeEditorWindow->GetPinLinkedTo(pinFloatArray->ID);

	if (!pinFloatArray || !linkedPin) return;

	if (linkedPin->Name == "ColorOvertime" && !pinFloatArray->float4Array[0].Equals(pinFloatArray->float4Array[1]))
	{
		for (unsigned int i = 0; i < emitter->activeParticles; ++i)
		{
			emitter->particles[emitter->particleIndices[i]].color = Lerp(pinFloatArray->float4Array[0], pinFloatArray->float4Array[1], emitter->particles[emitter->particleIndices[i]].relativeLifetime);
		}
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
	PinFloat4Array* pinFloat4Array = (PinFloat4Array*)emitterNode->GetInputPinByName("Velocity");

	if (pinFloat4Array)
	{
		particle->velocity = float4(Random::GenerateRandomFloat4(pinFloat4Array->float4Array[0], pinFloat4Array->float4Array[1]));
	}
	else
	{
		particle->velocity = float4(initialVelocity);
	}

}

void ParticleVelocity::Update(EmitterInstance* emitter, Node* emitterNode)
{
	for (int i = emitter->activeParticles - 1; i >= 0; --i)
	{
		Particle* particle = &emitter->particles[emitter->particleIndices[i]];

		if (emitter->GravitationalFields.empty())
		{
			particle->position += particle->velocity.Float3Part().Add(emitter->forceVector * particle->relativeLifetime) * particle->velocity.w * App->GetDT();
		}
		else
		{
			for (int h = 0; h < emitter->GravitationalFields.size(); h++)
			{
				float range = ((PinFloat*)emitter->GravitationalFields[h]->GetOutputPinByName("Range"))->pinFloat;

				float intensity = ((PinFloat*)emitter->GravitationalFields[h]->GetOutputPinByName("Intensity"))->pinFloat;

				GameObject* gravityFieldObject = emitter->GravitationalFields[h]->gameObject;

				if (particle->position.Distance(gravityFieldObject->transform->GetPosition()) < range)
				{
					float3 newVector;

					if (intensity > 0.0f)
					{
						newVector = particle->position - gravityFieldObject->transform->GetPosition();
					}
					else if (intensity < 0.0f)
					{
						newVector = gravityFieldObject->transform->GetPosition() - particle->position;
						intensity *= -1.0f;
					}


					particle->lerpCounter += App->GetDT() / 10.0f;

					newVector = float3::Lerp(particle->velocity.Float3Part(), newVector, particle->lerpCounter);

					particle->position += particle->velocity.Float3Part().Add(newVector * intensity) * particle->velocity.w * App->GetDT();
				}
				else
				{
					particle->position += particle->velocity.Float3Part().Add(emitter->forceVector * particle->relativeLifetime) * particle->velocity.w * App->GetDT();
				}
			}
		}
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
