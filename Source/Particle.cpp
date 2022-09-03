#include "Particle.h"

Particle::Particle()
{
	lerpCounter = 0.0f;
}

Particle::Particle(const Particle& particle)
{
	*this = particle;
}

Particle::~Particle()
{
}

void Particle::Update(float dt)
{
}
