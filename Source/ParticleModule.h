#ifndef __PARTICLE_MODULE_H__
#define __PARTICLE_MODULE_H__

#include "Dependencies/MathGeoLib/include/Math/float3.h"
#include "Dependencies/MathGeoLib/include/Math/float4.h"
#include "Dependencies/MathGeoLib/include/Algorithm/Random/LCG.h"

#include "NodeAlignment.h"

class Particle;
class Emitter;
class EmitterInstance;
class Node;

struct ParticleModule
{
	enum Type
	{
		EmitterBase,
		EmitterSpawn,
		EmitterArea,
		ParticlePosition,
		ParticleRotation,
		ParticleSize,
		ParticleColor,
		ParticleLifetime,
		ParticleVelocity,
		NONE
	} type;

	ParticleModule(Type type) : type(type) {};

	virtual void Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode) = 0;
	virtual void Update(EmitterInstance* emitter, Node* emitterNode) = 0;
	virtual void UpdateWithNode(EmitterInstance* emitter, Node* emitterNode) = 0;
};


struct EmitterBase : ParticleModule
{

	EmitterBase() : ParticleModule(Type::EmitterBase) {};

	void Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode);
	void Update(EmitterInstance* emitter, Node* emitterNode);
	void UpdateWithNode(EmitterInstance* emitter, Node* emitterNode);

	Quat GetParticleAlignment(const float3& position, const float4x4& cameraTransform);

	float3 emitterOrigin = float3::zero;
	Alignment alignment = Alignment::Camera;


};

struct EmitterSpawn : ParticleModule
{
	EmitterSpawn() : ParticleModule(Type::EmitterSpawn) {};

	void Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode);
	void Update(EmitterInstance* emitter, Node* emitterNode);
	void UpdateWithNode(EmitterInstance* emitter, Node* emitterNode);


	float spawnRatio = 0.1f;
	float currentTimer = 0.0f;
};

struct EmitterArea : ParticleModule
{
	EmitterArea() : ParticleModule(Type::EmitterArea) {};

	void Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode);
	void Update(EmitterInstance* emitter, Node* emitterNode);
	void UpdateWithNode(EmitterInstance* emitter, Node* emitterNode);

};

struct ParticlePosition : ParticleModule
{
	ParticlePosition() : ParticleModule(Type::ParticlePosition) {};

	void Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode);
	void Update(EmitterInstance* emitter, Node* emitterNode);
	void UpdateWithNode(EmitterInstance* emitter, Node* emitterNode);


	float3 initialPosition = float3::zero;
};

struct ParticleRotation : ParticleModule
{
	ParticleRotation() : ParticleModule(Type::ParticleRotation) {};

	void Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode);
	void Update(EmitterInstance* emitter, Node* emitterNode);
	void UpdateWithNode(EmitterInstance* emitter, Node* emitterNode);

	float initialRotation = 0.0f;
};

struct ParticleSize : ParticleModule
{
	ParticleSize() : ParticleModule(Type::ParticleSize) {};

	void Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode);
	void Update(EmitterInstance* emitter, Node* emitterNode);
	void UpdateWithNode(EmitterInstance* emitter, Node* emitterNode);

	float size = 1.0f;
};

struct ParticleColor : ParticleModule
{
	ParticleColor() : ParticleModule(Type::ParticleColor) {};

	void Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode);
	void Update(EmitterInstance* emitter, Node* emitterNode);
	void UpdateWithNode(EmitterInstance* emitter, Node* emitterNode);

	float4 initialColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct ParticleLifetime : ParticleModule
{
	ParticleLifetime() : ParticleModule(Type::ParticleLifetime) {};

	void Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode);
	void Update(EmitterInstance* emitter, Node* emitterNode);
	void UpdateWithNode(EmitterInstance* emitter, Node* emitterNode);

	float lifetime = 3.0f;
};

struct ParticleVelocity : ParticleModule
{
	ParticleVelocity() : ParticleModule(Type::ParticleVelocity) {};

	void Spawn(EmitterInstance* emitter, Particle* particle, Node* emitterNode);
	void Update(EmitterInstance* emitter, Node* emitterNode);
	void UpdateWithNode(EmitterInstance* emitter, Node* emitterNode);

	float4 initialVelocity = float4(1.0f, 1.0f, 1.0f, 2.0f);
	
};

#endif;
