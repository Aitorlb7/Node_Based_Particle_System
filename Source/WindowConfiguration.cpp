#include "WindowConfiguration.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"

#include "ComponentCamera.h"

#include "Dependencies/ImGUI/imgui.h"
#include "Dependencies/ImGUI/imgui_internal.h"
#include "Dependencies/ImGUI/imgui_impl_sdl.h"
#include "Dependencies/ImGUI/imgui_impl_opengl3.h"
#include "Dependencies/Devil/Include/ilut.h"
#include "Dependencies/ImGuizmo/ImGuizmo.h"

#include "Dependencies/Glew/include/GL/glew.h"
#include "Dependencies/SDL/include/SDL_opengl.h"


WindowConfiguration::WindowConfiguration(bool isActive): Window("Configuration Window", isActive)
{
}

WindowConfiguration::~WindowConfiguration()
{
}

void WindowConfiguration::Draw()
{
	if (!isActive) return;

	ImGui::Begin("Configuration", &isActive);

	if (ImGui::CollapsingHeader("Application"))
	{

		ImGui::SliderInt("Max FPS", &fpsCap, 1, 60);
		App->SetFpsCap(fpsCap);
		ImGui::Text("Limit Framerate:");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%d", fpsCap);

		sprintf_s(title, 25, "Framerate %.1f", App->GetFps()[App->GetFps().size() - 1]);
		ImGui::PlotHistogram("##framerate", &App->GetFps()[0], App->GetFps().size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %0.1f", App->GetMs()[App->GetMs().size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &App->GetMs()[0], App->GetMs().size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}
	if (ImGui::CollapsingHeader("Window"))
	{
		if (ImGui::Checkbox("Fullscreen", &fullscreen)) App->window->SetFullscreen(fullscreen);
		if (ImGui::Checkbox("Borderless", &borderless)) App->window->SetBorderless(borderless);
		if (ImGui::Checkbox("Full Desktop", &full_desktop)) App->window->SetFullscreenDesktop(full_desktop);

		ImGui::Separator();
		if (ImGui::SliderInt("Width", &window_width, 350, 1500, "%d")) SDL_SetWindowSize(App->window->window, window_width, window_height);
		if (ImGui::SliderInt("Height", &window_height, 350, 1200, "%d")) SDL_SetWindowSize(App->window->window, window_width, window_height);


		ImGui::SliderFloat("Brightness", &brightness, 0, 1, "%.3f");
		SDL_SetWindowBrightness(App->window->window, brightness);

	}
	if (ImGui::CollapsingHeader("Hardware"))
	{
		ImGui::Text("SDL Version:");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%d.%d.%d", version.major, version.minor, version.patch);

		ImGui::Separator();

		ImGui::Text("CPUs: ");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%d", SDL_GetCPUCount());

		ImGui::Text("System RAM: ");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, " %d Gb", SDL_GetSystemRAM());
		ImGui::Separator();

		ImGui::Text("Caps: ");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%s", GetSystemCaps());
		ImGui::Separator();

		ImGui::Text("GPU: ");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%s", glGetString(GL_RENDERER));

		ImGui::Text("Brand: ");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%s", glGetString(GL_VENDOR));

		ImGui::Text("Version: ");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%s", glGetString(GL_VERSION));

		ImGui::Text("VRAM Budget:");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%i Mb", GetBudget());

		ImGui::Text("VRAM Usage:");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%i Mb", GetUsage());

		ImGui::Text("VRAM Available:");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%i Mb", GetAvailable());

		ImGui::Text("VRAM Reserved:");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%i Mb", GetReserved());

	}
	if (ImGui::CollapsingHeader("OpenGL Settings")) {

		if (ImGui::Checkbox("Depth Test", &depthtest)) {
			App->renderer3D->SetDepthtest(depthtest);
		}
		if (ImGui::Checkbox("Cull Face", &cullface)) {
			App->renderer3D->SetCullface(cullface);
		}
		if (ImGui::Checkbox("Lightning", &lighting)) {
			App->renderer3D->SetLighting(lighting);
		}
		if (ImGui::Checkbox("Texture2D", &texture2D)) {
			App->renderer3D->SetTexture2D(texture2D);
		}
		if (ImGui::Checkbox("Color Material", &colormaterial)) {
			App->renderer3D->SetColormaterial(colormaterial);
		}
		if (ImGui::Checkbox("Cube Map", &cubemap)) {
			App->renderer3D->SetCubemap(cubemap);
		}
		if (ImGui::Checkbox("Polygons smooth", &polygonssmooth)) {
			App->renderer3D->SetPolygonssmooth(polygonssmooth);
		}

	}
	if (ImGui::CollapsingHeader("Draw Settings"))
	{
		if (ImGui::Checkbox("Draw Normals", &drawNormals)) {}
		if (ImGui::Checkbox("Wireframe Mode", &drawWireframe)) {}
		if (ImGui::Checkbox("Enable Checker Tex", &drawCheckerTex)) { drawTexture = false; }
		if (ImGui::Checkbox("Enable Texture", &drawTexture)) { drawCheckerTex = false; }
		if (ImGui::Checkbox("Draw Bounding Boxes", &App->renderer3D->drawboundingboxes)) { App->renderer3D->drawboundingboxes; }
		if (ImGui::Checkbox("Current Camera Culling", &App->camera->currentCamera->frustum_culling)) {}
	}
	if (ImGui::CollapsingHeader("Camera Settings"))
	{
		float Inspector_FOV = App->camera->currentCamera->GetFOV();
		if (ImGui::SliderFloat("FOV", &Inspector_FOV, 30, 120, "%0.2f", ImGuiSliderFlags_None))
		{
			App->camera->currentCamera->SetFOV(Inspector_FOV);
		}

		float Inspector_NearPlane = App->camera->currentCamera->GetNearPlane();
		if (ImGui::DragFloat("Near Plane", &Inspector_NearPlane))
		{
			App->camera->currentCamera->SetNearPlane(Inspector_NearPlane);
		}

		float Inspector_FarPlane = App->camera->currentCamera->GetFarPlane();
		if (ImGui::DragFloat("Far Plane", &Inspector_FarPlane))
		{
			App->camera->currentCamera->SetFarPlane(Inspector_FarPlane);
		}

		if (ImGui::Button("Change Current Camera"))
		{
			if (App->camera->currentCamera == App->camera->editorCamera) {
				App->camera->currentCamera = App->camera->gameCamera;
			}
			else if (App->camera->currentCamera == App->camera->gameCamera) {
				App->camera->currentCamera = App->camera->editorCamera;
			}
			else LOG("Something went wrong when swapping cameras");
		}
	}
}

const char* WindowConfiguration::GetSystemCaps()
{
	caps.clear();
	// IF the processor has certain register it will be added to the string
	if (SDL_Has3DNow())
	{
		caps.append("3D Now, ");
	}

	if (SDL_HasAVX())
	{
		caps.append("AVX, ");
	}

	if (SDL_HasAVX2())
	{
		caps.append("AVX2, ");
	}

	if (SDL_HasAltiVec())
	{
		caps.append("AltiVec, ");
	}

	if (SDL_HasMMX())
	{
		caps.append("MMX, ");
	}

	if (SDL_HasRDTSC())
	{
		caps.append("RDTSC, ");
	}

	if (SDL_HasSSE())
	{
		caps.append("SSE, ");
	}

	if (SDL_HasSSE2())
	{
		caps.append("SSE2, ");
	}

	if (SDL_HasSSE3())
	{
		caps.append("SSE3, ");
	}

	if (SDL_HasSSE41())
	{
		caps.append("SSE41, ");
	}

	if (SDL_HasSSE41())
	{
		caps.append("SSE42");
	}

	return caps.data();
}

int WindowConfiguration::GetBudget() {
	int budget;
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &budget);
	return budget / 1024.0f;
}

int WindowConfiguration::GetUsage() {
	int usage;
	glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &usage);
	return usage / 1024.0f;
}

int WindowConfiguration::GetAvailable() {
	int available;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &available);
	return available / 1024.0f;
}

int WindowConfiguration::GetReserved() {
	int reserved;
	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &reserved);
	return reserved / 1024.0f;
}


void WindowConfiguration::CleanUp()
{
}
