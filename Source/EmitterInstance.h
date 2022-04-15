#ifndef __EMITTER_INSTANCE_H__
#define __EMITTER_INSTANCE_H__

#include "Particle.h"

class Emitter;
class ComponentParticleSystem;

class EmitterInstance
{
public:
	EmitterInstance();

	void Init(Emitter* emitterReference, ComponentParticleSystem* component);

	void Update();
	void DrawParticles();
	void Reset();

	void SpawnParticle();

private:
	void KillDeadParticles();
	void KillAllParticles();
	void UpdateModules();

public:
	float emitterTime = 0;					//Keeping emitter time count, useful later for looping

	unsigned int activeParticles = 0;
	unsigned int* particleIndices;		//Ha ha, nice trick learnt from unreal, this is pure fun.
	std::vector<Particle> particles;

	Emitter* emitterReference;			//A reference to the emitter resource
	ComponentParticleSystem* component;		//A reference to the particle system component
};

#endif 

