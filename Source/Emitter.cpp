#include "Emitter.h"
#include "ResourceMaterial.h"

Emitter::Emitter():
	name("Particle Emitter"),
	maxParticleOnScreen(MAX_PARTICLES)
{
	material = new ResourceMaterial();
}

void Emitter::Update(float dt)
{
}

ParticleModule* Emitter::AddModuleFromType(ParticleModule::Type type)
{
	switch (type)
	{
	case(ParticleModule::Type::EmitterBase):
	{
		modules.push_back(new EmitterBase());
		return modules.back();
	}
	case(ParticleModule::Type::EmitterSpawn):
	{
		modules.push_back(new EmitterSpawn());
		return modules.back();
	}
	case(ParticleModule::Type::EmitterArea):
	{
		modules.push_back(new EmitterArea());
		return modules.back();
	}
	case(ParticleModule::Type::ParticlePosition):
	{
		modules.push_back(new ParticlePosition());
		return modules.back();
	}
	case(ParticleModule::Type::ParticleRotation):
	{
		modules.push_back(new ParticleRotation());
		return modules.back();
	}
	case(ParticleModule::Type::ParticleSize):
	{
		modules.push_back(new ParticleSize());
		return modules.back();
	}
	case(ParticleModule::Type::ParticleColor):
	{
		modules.push_back(new ParticleColor());
		return modules.back();
	}
	case(ParticleModule::Type::ParticleLifetime):
	{
		modules.push_back(new ParticleLifetime());
		return modules.back();
	}
	case(ParticleModule::Type::ParticleVelocity):
	{
		modules.push_back(new ParticleVelocity());
		return modules.back();
	}
	}
	return nullptr;
}