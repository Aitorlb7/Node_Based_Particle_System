#ifndef __EMITTER_H__
#define __EMITTER_H__

#include <vector>
#include "Particle.h"
#include "ParticleModule.h"

class ResourceMaterial;

class Emitter
{
public:
	Emitter();

	//TODO: Loop trhough modules
	void Update(float dt);

	//void Serialize(Config& config);
	ParticleModule* AddModuleFromType(ParticleModule::Type type);

public:
	std::string name = "Particle Emitter";
	std::vector<ParticleModule*> modules;
	ResourceMaterial* material; 
	int maxParticleCount = 100;
};

#endif // !__EMITTER_H__
