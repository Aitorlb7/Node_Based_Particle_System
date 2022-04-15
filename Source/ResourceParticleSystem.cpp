#include "ResourceParticleSystem.h"

ResourceParticleSystem::ResourceParticleSystem() : Resource(ResourceType::ParticleSystem)
{
    CreateDefaultSystem();
}

ResourceParticleSystem::ResourceParticleSystem(const char* assetsFile, const char* libraryFile, const char* name, uint32 UID) : Resource(ResourceType::ParticleSystem)
{
    this->name = name;
    this->libraryFile = libraryFile;
    this->assetsFile = assetsFile;
    if (UID != 0) this->UID = UID;
}

ResourceParticleSystem::~ResourceParticleSystem()
{
}


void ResourceParticleSystem::CreateDefaultSystem()
{
    emitters.clear();
    emitters.reserve(4);

    emitters.push_back(Emitter());

    emitters.back().modules.push_back(new EmitterBase());
    emitters.back().modules.push_back(new EmitterSpawn());
    emitters.back().modules.push_back(new ParticleLifetime());
    emitters.back().modules.push_back(new ParticleVelocity());
    emitters.back().modules.push_back(new ParticleSize());
    emitters.back().modules.push_back(new ParticleColor());
}
