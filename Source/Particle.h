#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Globals.h"
#include "Color.h"

#include "Dependencies/MathGeoLib/include/Math/float3.h"
#include "Dependencies/MathGeoLib/include/Math/Quat.h"

class Particle
{
public:
	Particle();
	Particle(const Particle& particle);
	~Particle();

	void Update(float dt);

public:

	float3 position;
	Quat worldRotation;

	float relativeLifetime;
	float maxLifetime;
	float currentLifetime;
	float normalizedLifetime;
	float rotation;

	float4 velocity;
	float3 acceleration;
	float size;

	float4 color;

	float distanceToCamera;
	bool usesBillboarding;
};

#endif // !__PARTICLE_H__