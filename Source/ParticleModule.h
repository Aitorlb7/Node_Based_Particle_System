#ifndef __PARTICLE_MODULE_H__
#define __PARTICLE_MODULE_H__

#include "Dependencies/MathGeoLib/include/Math/float3.h"
#include "Dependencies/MathGeoLib/include/Math/float4.h"
#include "Dependencies/MathGeoLib/include/Algorithm/Random/LCG.h"

class Particle;
class Emitter;
class EmitterInstance;

struct ParticleModule
{

	//TODO: Split Emiiter Modules and Particles Modules
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

	virtual void Spawn(EmitterInstance* emitter, Particle* particle) = 0;
	virtual void Update(EmitterInstance* emitter) = 0;

};


struct EmitterBase : ParticleModule
{
	enum Alignment
	{
		None,
		Screen,
		Camera,
		AxisXY,
		AxisXZ,
		AxisYZ,
		AxisYX,
		AxisZX,
		AxisZY,
		Unknown
	} alignment = Camera;

	EmitterBase() : ParticleModule(Type::EmitterBase) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(EmitterInstance* emitter);
	Quat GetParticleAlignment(const float3& position, const float4x4& cameraTransform);

	float3 emitterOrigin = float3::zero;
	//float3 emitterRotation;
};

struct EmitterSpawn : ParticleModule
{
	EmitterSpawn() : ParticleModule(Type::EmitterSpawn) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(EmitterInstance* emitter);

	//void Serialize(Config& config);

	float spawnRatio = 0.1f;
	float currentTimer = 0.0f;
};

struct EmitterArea : ParticleModule
{
	EmitterArea() : ParticleModule(Type::EmitterArea) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(EmitterInstance* emitter);

	//void Serialize(Config& config);

	//TODO: Based on a variable shape defined by another node
};

struct ParticlePosition : ParticleModule
{
	ParticlePosition() : ParticleModule(Type::ParticlePosition) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(EmitterInstance* emitter);

	//void Serialize(Config& config);

	float3 initialPosition1 = float3::zero;
	float3 initialPosition2 = float3::zero;
};

struct ParticleRotation : ParticleModule
{
	ParticleRotation() : ParticleModule(Type::ParticleRotation) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(EmitterInstance* emitter);

	//void Serialize(Config& config);

	float initialRotation1 = 0.0f;
	float initialRotation2 = 0.0f;
};

struct ParticleSize : ParticleModule
{
	ParticleSize() : ParticleModule(Type::ParticleSize) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(EmitterInstance* emitter);

	//void Serialize(Config& config);

	float initialSize = 1.0f;
	float finalSize = 1.0f;
};

struct ParticleColor : ParticleModule
{
	ParticleColor() : ParticleModule(Type::ParticleColor) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(EmitterInstance* emitter);

	//void Serialize(Config& config);

	float4 initialColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 finalColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
};

struct ParticleLifetime : ParticleModule
{
	ParticleLifetime() : ParticleModule(Type::ParticleLifetime) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(EmitterInstance* emitter);

	//void Serialize(Config& config);

	float lifetime1 = 3.0f;
	float lifetime2 = 6.0f;
};

struct ParticleVelocity : ParticleModule
{
	ParticleVelocity() : ParticleModule(Type::ParticleVelocity) {};

	void Spawn(EmitterInstance* emitter, Particle* particle);
	void Update(EmitterInstance* emitter);

	//void Serialize(Config& config);

	//Speed is stored in the Z value
	float4 initialVelocity1 = float4(1.0f, 1.0f, 1.0f, 2.0f);
	float4 initialVelocity2 = float4(-1.0f, 1.0f, -1.0f, 2.0f);
	
};

#endif;
