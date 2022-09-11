#ifndef __EMITTER_INSTANCE_H__
#define __EMITTER_INSTANCE_H__

#include "Particle.h"

class Emitter;
class ComponentParticleSystem;
class Node;
class NodeGravitationalField;

class EmitterInstance
{
public:
	EmitterInstance();

	void Init(Emitter* emitterReference, ComponentParticleSystem* component);

	void Update();
	void DrawParticles();
	void Reset();

	void SpawnParticle();

	void UpdateWithNode();

	void AddGravitationalField(NodeGravitationalField* gravitationalField);
	void DeleteGravitationalField(NodeGravitationalField* gravitationalField);

private:
	void KillDeadParticles();
	void KillAllParticles();
	void UpdateModules();

public:
	bool isActive;

	float emitterTime = 0;
	unsigned int activeParticles = 0;
	unsigned int* particleIndices;	
	std::vector<Particle> particles;

	Emitter* emitterReference;
	ComponentParticleSystem* component;	

	Node* emitterNode;

	float3 forceVector;

	std::vector<NodeGravitationalField*> GravitationalFields;
};

#endif 

