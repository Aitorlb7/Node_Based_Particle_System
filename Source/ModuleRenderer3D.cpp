#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

#include "ModuleResource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ImporterShader.h"
#include "ImporterMaterials.h"

#include "Particle.h"
#include "GameObject.h"
#include "ModuleFileSystem.h"

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "WindowConfiguration.h"

#include "Dependencies/MathGeoLib/include/Geometry/Plane.h"
#include "Dependencies/MathGeoLib/include/Geometry/LineSegment.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#pragma comment (lib, "Dependencies/Glew/libx86/Release/Win32/glew32.lib")

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled), context()
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	glewInit();

	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		glClearColor(0.3f, 0.3f, 0.3f, 1.f);
	
		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		lights[0].Active(true);
		SetDepthtest(false);
		SetColormaterial(false);

		

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		
	}	

	Importer::TextureImporter::InitDevil();

	UseCheckerTexture();
	
	timer.Start();

	return ret;
}

bool ModuleRenderer3D::Start()
{	
	defaultParticleTex = App->resources->GetTexture("DefaultParticle");

	SetUpParticlesBuffer();

	SetUpFrameBuffers();

	//defaultSkyBox.SetUpSkyBoxBuffers();

	//defaultSkyBox.CreateSkybox();

	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.4f, 0.4f, 0.4f, 0.4f);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);

	glLoadMatrixf(App->camera->GetRawViewMatrix());

	lights[0].SetPos(App->camera->currentCamera->frustum.Pos().x, App->camera->currentCamera->frustum.Pos().y, App->camera->currentCamera->frustum.Pos().z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	SetCullface(App->editor->configWindow->cullface);
	SetLighting(App->editor->configWindow->lighting);
	SetTexture2D(App->editor->configWindow->texture2D);

	glColor4f(0.3f, 0.3f, 0.3f, 0.3f);
	glEnd();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawGrid();
	DrawGameCamera();

	if (App->editor->configWindow->drawWireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); glEnable(GL_TEXTURE_CUBE_MAP); }
	else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); glDisable(GL_TEXTURE_CUBE_MAP); }
	
	DrawAllMeshes();
	DrawAllParticles();

	ImGui::GetBackgroundDrawList();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	App->editor->DrawGUI();

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");
	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::SetUpFrameBuffers()
{
	glGenFramebuffers(1, (GLuint*)&sceneFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);

	// --- SCENE RENDER TEXTURE ---
	glGenTextures(1, (GLuint*)&sceneTexture);
	glBindTexture(GL_TEXTURE_2D, sceneTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, App->window->Width(), App->window->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// --- DEPTH BUFFER TEXTURE ---
	glGenTextures(1, (GLuint*)&depthBufferTexture);
	glBindTexture(GL_TEXTURE_2D, depthBufferTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, App->window->Width(), App->window->Height(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBufferTexture, 0);

	// --- DEPTH & STENCIL BUFFERS ---
	glGenRenderbuffers(1, (GLuint*)&depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App->window->Width(), App->window->Height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("[ERROR] Renderer 3D: Could not generate the scene's frame buffer! Error: %s", gluErrorString(glGetError()));
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRenderer3D::UpdateFrameBufferSize()
{
	//Update texture size
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, App->window->Width(), App->window->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Update depth & stencil buffer size
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App->window->Width(), App->window->Height());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


void ModuleRenderer3D::DrawGrid()
{

	glColor4f(0.7f, 0.7f, 0.7f, 0.7f);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	float z = 70.0f;
	for (float x = -70; x <= z; x += 2.0f)
	{
		glVertex3f(x, -1.0f, -z);
		glVertex3f(x, -1.0f, z);
		glVertex3f(-z, -1.0f, x);
		glVertex3f(z, -1.0f, x);
	}
	glEnd();
	glLineWidth(1.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void ModuleRenderer3D::DrawGameCamera()
{
	if (App->camera->gameCamera != nullptr) {
		glColor4f(0.1f, 0.7f, 0.7f, 0.7f);
		glBegin(GL_LINES);
		glLineWidth(2.0f);
		vec* frustum_corners;
		frustum_corners = App->camera->gameCamera->GetFrustumPoints();
		DrawCuboid(frustum_corners, App->editor->frustumColor);
		glEnd();
		glLineWidth(1.0f);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);
	
	if (App->camera->currentCamera != nullptr) {

		App->camera->currentCamera->SetAspectRatio((float)width / (float)height);
	}

	UpdateProjectionMatrix();

	SetUpFrameBuffers();
}


void ModuleRenderer3D::UpdateProjectionMatrix()
{
	//Only do if cameras are up and running
	if (App->camera->currentCamera == nullptr) return;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf((GLfloat*)App->camera->currentCamera->GetProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void ModuleRenderer3D::DrawAllMeshes()
{
	//Draw all the GameObjects in the scene

	for (uint i = 0; i < App->scene->game_objects.size(); i++)
	{
		if (!App->scene->game_objects.at(i)->active && !App->scene->game_objects.at(i)->children.empty()) break;

		if (App->scene->game_objects[i]->GetComponent(ComponentType::Mesh) != nullptr)
		{
			
			std::vector<Component*> meshComponents = App->scene->game_objects[i]->GetComponents(ComponentType::Mesh);
			std::vector<Component*>::iterator item = meshComponents.begin();

			for (; item != meshComponents.end(); ++item) {
				
				if (!App->scene->game_objects.at(i)->active) break;

				ComponentMaterial* componentMat = (ComponentMaterial*)App->scene->game_objects[i]->GetComponent(ComponentType::Material);
				ComponentMesh* tempComponentMesh = (ComponentMesh*)(*item);
				ComponentTransform* tempComponentTransform = (ComponentTransform*)App->scene->game_objects[i]->GetComponent(ComponentType::Transform);
				if (componentMat != nullptr)
				{
					DrawMesh(tempComponentMesh, tempComponentTransform->GetGlobalTransform(), componentMat, App->scene->game_objects[i]);
					if (App->editor->configWindow->drawNormals) DrawNormals(tempComponentMesh, tempComponentTransform->GetGlobalTransform());
				}
				else 
				{
					DrawMesh(tempComponentMesh, tempComponentTransform->GetLocalTransform(), nullptr, App->scene->game_objects[i]);
					if (App->editor->configWindow->drawNormals) DrawNormals(tempComponentMesh, tempComponentTransform->GetLocalTransform());
				}

				if (drawboundingboxes) {
					glLineWidth(2.0f);
					glBegin(GL_LINES);

					vec* corners = new vec[8];
					App->scene->game_objects[i]->aabb.GetCornerPoints(corners);
					DrawCuboid(corners, App->editor->AABBColor);
					App->scene->game_objects[i]->obb.GetCornerPoints(corners);
					DrawCuboid(corners, App->editor->OBBColor);

					delete[] corners;

					glEnd();
				}
			}
		}
	}
}

void ModuleRenderer3D::DrawMesh(ComponentMesh* componentMesh, float4x4 transform, ComponentMaterial* componentMaterial, GameObject* meshOwner)
{
	uint32 shaderProgram = 0;
	if (App->camera->currentCamera->frustum_culling) 
	{
		if(!DoesIntersect(meshOwner->aabb))
		{
			return;
		}
	}
	if (componentMaterial->GetMaterial() != nullptr)
	{
		if(componentMaterial->GetMaterial()->GetShader()) shaderProgram = componentMaterial->GetMaterial()->GetShaderProgramID();

		shaderProgram ? shaderProgram : shaderProgram = SetDefaultShader(componentMaterial->GetMaterial());

		glUseProgram(shaderProgram);
		
		App->editor->configWindow->drawCheckerTex ? glBindTexture(GL_TEXTURE_2D, checkerID) : glBindTexture(GL_TEXTURE_2D, componentMaterial->GetMaterial()->GetTextureId());

		componentMaterial->GetMaterial()->GetTextureId() ? componentMaterial->GetMaterial()->GetShader()->SetUniform1i("hasTexture", (GLint)true) : componentMaterial->GetMaterial()->GetShader()->SetUniform1i("hasTexture", (GLint)false);

		if (shaderProgram != 0) 
		{
			componentMaterial->GetMaterial()->GetShader()->SetUniformVec4f("inColor", (GLfloat*)&componentMaterial->GetMaterial()->GetColor());

			componentMaterial->GetMaterial()->GetShader()->SetUniformMatrix4("modelMatrix", transform.Transposed().ptr());

			componentMaterial->GetMaterial()->GetShader()->SetUniformMatrix4("viewMatrix", App->camera->GetRawViewMatrix());

			componentMaterial->GetMaterial()->GetShader()->SetUniformMatrix4("projectionMatrix", App->camera->GetProjectionMatrix());

			componentMaterial->GetMaterial()->GetShader()->SetUniform1f("time", App->scene->GameTime.ReadSec());

			componentMaterial->GetMaterial()->GetShader()->SetUniform1i("skybox", 11);

			componentMaterial->GetMaterial()->GetShader()->SetUniformVec3f("cameraPosition", (GLfloat*)&App->camera->currentCamera->frustum.Pos());

			Importer::ShaderImporter::SetShaderUniforms(componentMaterial->GetMaterial()->GetShader());
		}

		//Update Each Time?
		componentMaterial->GetMaterial()->GetShader()->SetUniformVec3f("cameraPosition", (GLfloat*)&App->camera->currentCamera->frustum.Pos());
		componentMaterial->GetMaterial()->GetShader()->SetUniform1f("time", App->scene->GameTime.ReadSec());
	}
	
	if (!App->scene->root_object->children.empty() && componentMesh->GetMesh() != nullptr)
	{
		glBindVertexArray(componentMesh->GetMesh()->VAO);

		glDrawElements(GL_TRIANGLES, componentMesh->GetMesh()->size[ResourceMesh::index], GL_UNSIGNED_INT, nullptr);
		
		glBindVertexArray(0);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glUseProgram(0);
	}	
}

uint32 ModuleRenderer3D::SetDefaultShader(ResourceMaterial* resourceMaterial)
{	
	if (!defaultShader) defaultShader = App->resources->GetShader("DefaultShader");
	
	resourceMaterial->SetShader(defaultShader);

	return resourceMaterial->GetShaderProgramID();
}

void ModuleRenderer3D::UseCheckerTexture() {

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkerID);
	glBindTexture(GL_TEXTURE_2D, checkerID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
}

void ModuleRenderer3D::DrawNormals(ComponentMesh* componentMesh, float4x4 transform)
{
	glBegin(GL_LINES);
	glColor4f(App->editor->NormalColor.r, App->editor->NormalColor.g, App->editor->NormalColor.b, App->editor->NormalColor.a);
	
	for (uint i = 0; i < componentMesh->GetMesh()->size[ResourceMesh::vertex] * 3; i +=3)
	{
		LineSegment NormalDirection(vec(componentMesh->GetMesh()->vertices[i], componentMesh->GetMesh()->vertices[i + 1], componentMesh->GetMesh()->vertices[i + 2]),
									vec(componentMesh->GetMesh()->vertices[i] + componentMesh->GetMesh()->normals[i], componentMesh->GetMesh()->vertices[i + 1] + componentMesh->GetMesh()->normals[i + 1],
										componentMesh->GetMesh()->vertices[i + 2] + componentMesh->GetMesh()->normals[i + 2]));

		NormalDirection.Transform(transform);

		glVertex3f(NormalDirection.a.x, NormalDirection.a.y, NormalDirection.a.z);
		glVertex3f(NormalDirection.b.x, NormalDirection.b.y, NormalDirection.b.z);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
}


void ModuleRenderer3D::SetDepthtest(bool state) {
	if (state == false)
		glEnable(GL_DEPTH_TEST);
	else if (state == true)
		glDisable(GL_DEPTH_TEST);
}
void ModuleRenderer3D::SetCullface(bool state) {
	if (state == false)
		glEnable(GL_CULL_FACE);
	else if (state == true)
		glDisable(GL_CULL_FACE);
}
void ModuleRenderer3D::SetLighting(bool state) {
	if (state == false)
		glEnable(GL_LIGHTING);
	else if (state == true)
		glDisable(GL_LIGHTING);
}
void ModuleRenderer3D::SetColormaterial(bool state) {
	if (state == false)
		glEnable(GL_COLOR_MATERIAL);
	else if (state == true)
		glDisable(GL_COLOR_MATERIAL);
}
void ModuleRenderer3D::SetTexture2D(bool state) {
	if (state == false)
		glEnable(GL_TEXTURE_2D);
	else if (state == true)
		glDisable(GL_TEXTURE_2D);
}
void ModuleRenderer3D::SetCubemap(bool state) {
	if (state == false)
		glEnable(GL_TEXTURE_CUBE_MAP);
	else if (state == true)
		glDisable(GL_TEXTURE_CUBE_MAP);
}
void ModuleRenderer3D::SetPolygonssmooth(bool state) {
	if (state == false)
		glEnable(GL_POLYGON_SMOOTH);
	else if (state == true)
		glDisable(GL_POLYGON_SMOOTH);
}

void ModuleRenderer3D::SetUpParticlesBuffer()
{
	// Purely temporal as we port particles to shaders
	particleVertices = { -0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, 0.5f, 0.0f,
						-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f };

	particleUVs = { 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
					0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f };


	// TODO: Remake this into Instancing

	glGenVertexArrays(1, &particleVAO);
	glBindVertexArray(particleVAO);

	glGenBuffers(1, &particleVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, particleVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &particleUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, particleUVs.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void ModuleRenderer3D::AddParticle(Particle* particle, ResourceMaterial* material)
{
	particles.insert(std::pair<float, ParticleRenderInfo>(particle->distanceToCamera, ParticleRenderInfo(material,particle)));
}

void ModuleRenderer3D::DrawAllParticles()
{
	if (particles.empty()) return;

	App->editor->configWindow->particlesInScreen = particles.size();

	std::map<float, ParticleRenderInfo>::reverse_iterator it;

	for (it = particles.rbegin(); it != particles.rend(); ++it)
	{
		DrawParticle((*it).second);
	}
	
	particles.clear();

	glUseProgram(0);
}

void ModuleRenderer3D::DrawParticle(ParticleRenderInfo& particleInfo)
{
	uint32 shaderProgram = 0;
	float4x4 transform = float4x4::FromTRS(particleInfo.particle->position, 
										   particleInfo.particle->worldRotation, 
										   float3(particleInfo.particle->size)).Transposed();

	if (particleInfo.material->GetShader()) 
		shaderProgram = particleInfo.material->GetShaderProgramID();

	shaderProgram ? shaderProgram : shaderProgram = SetDefaultShader(particleInfo.material);

	glUseProgram(shaderProgram);

	if (!particleInfo.material->GetTexture())
	{
		particleInfo.material->SetTexture(defaultParticleTex);
	}


	particleInfo.material->GetShader()->SetUniform1i("hasTexture", (GLint)true);
	
	glBindTexture(GL_TEXTURE_2D, particleInfo.material->GetTextureId());

	if (shaderProgram != 0)
	{
		particleInfo.material->GetShader()->SetUniformVec4f("inColor", (GLfloat*)&particleInfo.particle->color);

		particleInfo.material->GetShader()->SetUniformMatrix4("modelMatrix", transform.ptr());

		particleInfo.material->GetShader()->SetUniformMatrix4("viewMatrix", App->camera->GetRawViewMatrix());

		particleInfo.material->GetShader()->SetUniformMatrix4("projectionMatrix", App->camera->GetProjectionMatrix());

		//Importer::ShaderImporter::SetShaderUniforms(particleInfo.material->GetShader());
	}

	glBindVertexArray(particleVAO);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

bool ModuleRenderer3D::DoesIntersect(const AABB& aabb) {

	static vec aabb_corners[8];
	aabb.GetCornerPoints(aabb_corners);

	static Plane frustum_planes[6];
	App->camera->gameCamera->frustum.GetPlanes(frustum_planes);

	for(int p = 0; p < 6; p++)
	{
		int iInCount = 8;

		for(int i = 0; i < 8; i++)
		{
			if(frustum_planes[p].IsOnPositiveSide(aabb_corners[i]))
			{
				--iInCount;
			}
		}
		if (iInCount == 0) return false;
	}

	return true;
}

void ModuleRenderer3D::DrawCuboid(const float3* corners, Color color) {
	
	glColor4f(color.r, color.g, color.b, color.a);

	//Between-planes right
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[3]);

	//Between-planes left
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[2]);
	glVertex3fv((GLfloat*)&corners[6]);

	//Far plane horizontal
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[6]);
	glVertex3fv((GLfloat*)&corners[7]);

	//Near plane horizontal
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[2]);

	//Near plane vertical
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[2]);

	//Far plane vertical
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[6]);

	glColor4f(1.0, 1.0, 1.0, 1.0);

}

