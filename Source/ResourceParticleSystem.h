#ifndef __RESOURCE_PARTICLE_SYSTEM_H__
#define __RESOURCE_PARTICLE_SYSTEM_H__

#include "Resource.h"
#include "Emitter.h"

class ResourceParticleSystem : public Resource
{
public:
	ResourceParticleSystem();
	ResourceParticleSystem(const char* assetsFile, const char* libraryFile, const char* name, uint32 UID);
	~ResourceParticleSystem();

	void CreateDefaultSystem();

public:
	std::vector<Emitter> emitters;
};
#endif //__RESOURCE_PARTICLE_SYSTEM_H__
