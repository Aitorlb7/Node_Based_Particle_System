#pragma once
#include "Module.h"
#include "glmath.h"
#include "Light.h"
#include "OpenGL.h"
#include <map>
#include <vector>
#include "Dependencies/SDL/include/SDL.h"
#include "Dependencies/MathGeoLib/include/Math/float4x4.h"
#include "Dependencies/MathGeoLib/include/Math/float3x3.h"
#include "SkyBox.h"
#include "Timer.h"

#define MAX_LIGHTS 8

class WindowConfiguration;
class ComponentCamera;
class ComponentMesh;
class ComponentMaterial;
class GameObject;
class ResourceTexture;
class ResourceShader;
class ResourceMaterial;
class Particle;

struct ParticleRenderInfo 
{
	ParticleRenderInfo(ResourceMaterial* _material, Particle* _particle) : material(_material), particle(_particle) {}

	ResourceMaterial* material;
	Particle* particle;
};


class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

	void OnResize(int width, int height);

	void UseCheckerTexture();
	void DrawAllMeshes();
	void DrawMesh(ComponentMesh* mesh, float4x4 transform, ComponentMaterial* rMaterial = nullptr, GameObject* meshOwner = nullptr);

	void DrawNormals(ComponentMesh* mesh, float4x4 transform);

	void SetDepthtest(bool active);
	void SetCullface(bool active);
	void SetLighting(bool active);
	void SetColormaterial(bool active);
	void SetTexture2D(bool active);
	void SetCubemap(bool active);
	void SetPolygonssmooth(bool active);

	void UpdateProjectionMatrix();

	//Particles
	void SetUpParticlesBuffer();

	void AddParticle(Particle* particle, ResourceMaterial* material);

	void DrawAllParticles();

	void DrawParticle(ParticleRenderInfo& particleInfo);

private:
	bool DoesIntersect(const AABB& aabb);
	void DrawCuboid(const float3* corners, Color color);
	uint32 SetDefaultShader(ResourceMaterial* componentMaterial);

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ProjectionMatrix;

	GLbyte checkerImage[64][64][4];
	GLuint checkerID;

	GLuint newTexture = 0;

	Skybox defaultSkyBox;
	ResourceShader* defaultShader = nullptr;

	bool drawboundingboxes = false;

	Timer timer;

	std::map<float, GameObject*> sortedGO;

	//Use Map or Multimap?? We will have several particles at the same camera distance?
	std::map<float, ParticleRenderInfo> particles;

private:

	WindowConfiguration* configWindow;

	std::vector<float> particleVertices;
	std::vector<float> particleUVs;

	uint particleVAO;
	uint particleVertexBuffer;
	uint particleUVBuffer;

	ResourceTexture* defaultParticleTex;

};