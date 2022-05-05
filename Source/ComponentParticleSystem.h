#ifndef __C_PARTICLE_SYSTEM_H__
#define __C_PARTICLE_SYSTEM_H__

#include "Globals.h"
#include "Component.h"
#include "ResourceParticleSystem.h"
#include "EmitterInstance.h"
#include <vector>

class GameObject;


class ComponentParticleSystem : public Component
{
public:
	ComponentParticleSystem(GameObject* gameObject);
	ComponentParticleSystem(GameObject* gameObject, const char* meshPath, ResourceParticleSystem* _mesh = nullptr);
	~ComponentParticleSystem();

	void Enable() override;
	void Update() override;
	void Reset();
	void Disable() override;
	void CleanUp() override;

	void SetResourceProperties(ResourceParticleSystem* particleSystem);

	uint32 GetResourceUID() const { return rParticleSystem->UID; }
	ResourceParticleSystem* GetParticleSystem() const { return rParticleSystem; };
	std::string  GetPath() const { return path; };

	void SetParticleSystem(ResourceParticleSystem* particleSystem);
	void SetPath(std::string path);
	void SetResourceID(uint32 ID);

public:

	std::vector<EmitterInstance> emitters;

private:
	ResourceParticleSystem* rParticleSystem = nullptr;
	std::string path = "";
};

#endif
