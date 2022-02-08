#include "WindowInspector.h"
#include "ModuleEditor.h"
#include "ModuleResource.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ColorTextEditor/TextEditor.h"

#include "Component.h"
#include "ComponentCamera.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

#include "ResourceShader.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"

WindowInspector::WindowInspector(bool isActive) : Window("Inspector Window", isActive)
{
}

WindowInspector::~WindowInspector()
{
}

void WindowInspector::Draw()
{
	ImGui::Begin("Inspector", &isActive);

	if (App->scene->selected_object != nullptr)
	{
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.33f);
		char tempName[64];
		strcpy_s(tempName, App->scene->selected_object->name.c_str());
		if (ImGui::InputText("Name", (char*)App->scene->selected_object->name.c_str(), 64, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			App->scene->selected_object->SetName(tempName);
		}
		ImGui::Separator();

		if (ImGui::Checkbox("Object Enabled", &enableObject))
		{
			enableObject ? App->scene->selected_object->Enable() : App->scene->selected_object->Disable();
		}

		ImGui::Text("Parent: ");
		ImGui::SameLine();
		if (App->scene->selected_object->parent != nullptr)
		{
			ImGui::TextColored(GREEN, "%s", App->scene->selected_object->parent->name.c_str());
		}
		else ImGui::Text("No Parent");


		if (ImGui::Button("Delete Object"))
		{
			ImGui::OpenPopup("Delete Object", ImGuiPopupFlags_None);
		}
		if (ImGui::BeginPopupModal("Delete Object", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("This action cannot be undone!\n Are you sure? \n\n");
			ImGui::Separator();

			if (ImGui::Button("Delete", ImVec2(120, 0)))
			{
				App->scene->DeleteGameObject(App->scene->selected_object);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		ImGui::Separator();
		if (App->scene->selected_object != nullptr) {
			for (uint m = 0; m < App->scene->selected_object->components.size(); m++)
			{
				DrawComponent(App->scene->selected_object->components[m]);
				//App->scene->selected_object->components[m]->DrawInspector();
			}
		}

		ImGui::Separator();

		const char* items[] = { "Transform", "Mesh", "Texture", "Camera" };
		static const char* current_item = NULL;
		if (ImGui::BeginCombo("-", current_item))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(items[n], is_selected))
				{
					current_item = items[n];
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)	
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		if (ImGui::Button("Add Component"))
		{
			if (strcmp(current_item, "Transform") == 0)
			{
				if (App->scene->selected_object->GetComponent(ComponentType::Transform) == nullptr)
				{
					ComponentTransform* newComponent = new ComponentTransform(App->scene->selected_object);
					App->scene->selected_object->AddComponent(newComponent);
				}
				else
				{
					LOG("ERROR: Game Objects can not have repeated components");
				}
			}
			else if (strcmp(current_item, "Mesh") == 0)
			{
				if (App->scene->selected_object->GetComponent(ComponentType::Mesh) == nullptr)
				{
					ComponentMesh* newComponent = new ComponentMesh(App->scene->selected_object);
					App->scene->selected_object->AddComponent(newComponent);
				}
				else
				{
					LOG("ERROR: Game Objects can not have repeated components");
				}
			}
			else if (strcmp(current_item, "Texture") == 0)
			{
				if (App->scene->selected_object->GetComponent(ComponentType::Material) == nullptr)
				{
					ComponentMaterial* newComponent = new ComponentMaterial(App->scene->selected_object);
					App->scene->selected_object->AddComponent(newComponent);
				}
				else
				{
					LOG("ERROR: Game Objects can not have repeated components");
				}
			}
			else if (strcmp(current_item, "Camera") == 0)
			{
				if (App->scene->selected_object->GetComponent(ComponentType::Camera) == nullptr)
				{
					ComponentCamera* newComponent = new ComponentCamera(App->scene->selected_object);
					App->scene->selected_object->AddComponent(newComponent);
				}
				else
				{
					LOG("ERROR: Game Objects can not have repeated components");
				}
			}
		}
	}
	ImGui::End();

}

void WindowInspector::DrawComponent(Component* component)
{
	switch (component->type)
	{
	case ComponentType::Camera: DrawCamera((ComponentCamera*)component); break;
	case ComponentType::Material: DrawMaterial((ComponentMaterial*)component); break;
	case ComponentType::Mesh: DrawMesh((ComponentMesh*)component); break;
	case ComponentType::Transform: DrawTransform((ComponentTransform*)component); break;
	}
}

void WindowInspector::DrawTransform(ComponentTransform* component)
{
	if (ImGui::CollapsingHeader("Component Transform"), ImGuiTreeNodeFlags_DefaultOpen)
	{
		ImGui::Text("Local");
		if (ImGui::DragFloat3("Local Position", (float*)&component->local_position, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None)) { component->UpdateLocalTransform(); }
		if (ImGui::DragFloat3("Local Scale", (float*)&component->local_scale, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None)) { component->UpdateLocalTransform(); }
		if (ImGui::DragFloat3("Local Rotation", (float*)&component->local_eulerRotation, 0.08f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None)) { component->SetLocalEulerRotation(component->local_eulerRotation); }

		ImGui::Separator();

		ImGui::Text("Global");
		if (ImGui::DragFloat3("Global Position", (float*)&component->global_position, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None)) { component->UpdateLocalTransform(); }
		if (ImGui::DragFloat3("Global Scale", (float*)&component->global_scale, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None)) { component->UpdateLocalTransform(); }
		if (ImGui::DragFloat3("Global Rotation", (float*)&component->global_eulerRotation, 0.08f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None)) { component->SetGlobalEulerRotation(component->global_eulerRotation); }
	}
}

void WindowInspector::DrawMesh(ComponentMesh* component)
{
	if (ImGui::CollapsingHeader("Component Mesh"), ImGuiTreeNodeFlags_DefaultOpen)
	{

		if (component->GetMesh() != nullptr)
		{

			ImGui::Text("Path: ");
			ImGui::SameLine();
			ImGui::TextColored(GREEN, "%s", component->GetPath().c_str());

			ImGui::Text("Index: ");
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%d", component->GetMesh()->size[0]);

			ImGui::Text("Vertices: ");
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%d", component->GetMesh()->size[1]);

			ImGui::Text("Normals: ");
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%d", component->GetMesh()->size[2]);

			ImGui::Text("Texture Coords: ");
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%d", component->GetMesh()->size[3]);

		}
		else
		{
			ImGui::Text("Mesh is nullptr");
		}
	}
}

void WindowInspector::DrawMaterial(ComponentMaterial* component)
{
	if (ImGui::CollapsingHeader("Component Material"), ImGuiTreeNodeFlags_DefaultOpen)
	{
		std::map<uint32, ResourceTexture*> texturesInMemory = App->resources->GetTexturesInMemory();

		std::map<uint32, ResourceTexture*>::iterator nameIt = texturesInMemory.begin();

		if (ImGui::BeginCombo("Texture", component->GetMaterial()->GetTexture()->name.c_str(), ImGuiComboFlags_PopupAlignLeft))
		{
			for (std::map<uint32, ResourceTexture*>::iterator item = texturesInMemory.begin(); item != texturesInMemory.end(); item++)
			{
				const bool is_selected = (nameIt == item);
				if (ImGui::Selectable(item->second->name.c_str(), is_selected))
				{
					//To delete
					if (item->second->id == 0)
					{
						ResourceTexture* newTexture = (ResourceTexture*)App->resources->LoadResource(item->second->UID);
						component->GetMaterial()->SetTexture(newTexture);
					}
					else
					{
						component->GetMaterial()->SetTexture(item->second);
					}

					nameIt = item;
				}

			}
			ImGui::EndCombo();
		}


		if (component->GetMaterial()->GetTexture() != nullptr)
		{
			ImGui::Text("Path: ");
			ImGui::SameLine();
			ImGui::TextColored(GREEN, "%s", component->GetMaterial()->GetTexture()->assetsFile.c_str());
			ImGui::Text("Id: ");
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%d", component->GetMaterial()->GetId());
		}
		else
		{
			ImGui::Text("Texture is nullptr");
		}
		ImGui::Separator();
		ImGui::Text("Material:");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%s", component->GetMaterial()->name.c_str());
		if (component->GetMaterial())
		{
			ImGui::Text("Color: ");
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%d, ", component->GetMaterial()->GetColor().r);
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%d, ", component->GetMaterial()->GetColor().g);
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%d, ", component->GetMaterial()->GetColor().b);
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%d", component->GetMaterial()->GetColor().a);
		}
		ImGui::Separator();
		ImGui::Text("Shader:");
		ImGui::SameLine();
		ImGui::TextColored(GREEN, "%s", component->GetMaterial()->GetShader()->name.c_str());


		std::map<uint32, ResourceShader*> shadersInMemory = App->resources->GetShadersInMemory();

		std::map<uint32, ResourceShader*>::iterator shaderName = shadersInMemory.begin();

		if (ImGui::BeginCombo("Shader", component->GetMaterial()->GetShader()->name.c_str(), ImGuiComboFlags_PopupAlignLeft))
		{
			for (std::map<uint32, ResourceShader*>::iterator iterator = shadersInMemory.begin(); iterator != shadersInMemory.end(); iterator++)
			{
				const bool selectedShader = (shaderName == iterator);
				if (ImGui::Selectable(iterator->second->name.c_str(), selectedShader))
				{
					if (iterator->second->shaderProgramID == 0)
					{
						ResourceShader* newShader = (ResourceShader*)App->resources->LoadResource(iterator->second->UID);
						component->GetMaterial()->SetShader(newShader);
					}
					else
					{
						component->GetMaterial()->SetShader(iterator->second);
					}

					shaderName = iterator;
				}
			}
			ImGui::EndCombo();
		}


		if (ImGui::Button("Edit Shader"))
		{
			App->editor->CallTextEditor(component->GetMaterial());
		}

		ResourceShader* shader = component->GetMaterial()->GetShader();

		for (uint i = 0; i < shader->uniforms.size(); i++)
		{
			switch (shader->uniforms[i].uniformType)
			{
			case  UniformType::INT:	ImGui::DragInt(shader->uniforms[i].name.c_str(), &shader->uniforms[i].integer, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::FLOAT: ImGui::DragFloat(shader->uniforms[i].name.c_str(), &shader->uniforms[i].floatNumber, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::INT_VEC2: ImGui::DragInt2(shader->uniforms[i].name.c_str(), (int*)&shader->uniforms[i].vec2, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::INT_VEC3: ImGui::DragInt3(shader->uniforms[i].name.c_str(), (int*)&shader->uniforms[i].vec3, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::INT_VEC4: ImGui::DragInt4(shader->uniforms[i].name.c_str(), (int*)&shader->uniforms[i].vec4, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::FLOAT_VEC2: ImGui::DragFloat2(shader->uniforms[i].name.c_str(), (float*)&shader->uniforms[i].vec2, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::FLOAT_VEC3: ImGui::DragFloat3(shader->uniforms[i].name.c_str(), (float*)&shader->uniforms[i].vec3, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::FLOAT_VEC4: ImGui::DragFloat4(shader->uniforms[i].name.c_str(), (float*)&shader->uniforms[i].vec4, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case UniformType::MATRIX4: ImGui::DragFloat4(shader->uniforms[i].name.c_str(), shader->uniforms[i].matrix4.ToEulerXYZ().ptr(), 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			}
		}

		if (!component->GetMaterial()->GetShader()->uniforms.empty())
		{
			if (ImGui::Button("Save Uniforms"))
			{
				App->resources->SaveResource(component->GetMaterial()->GetShader());
			}
		}

	}
}

void WindowInspector::DrawCamera(ComponentCamera* component)
{
	if (ImGui::CollapsingHeader("Component Camera"), ImGuiTreeNodeFlags_DefaultOpen)
	{
		//if (ImGui::Checkbox("Viewport Camera", &active_camera)) {}

		if (ImGui::Checkbox("Frustum Culling", &component->frustum_culling)) {}

		//Set FOV
		float Inspector_FOV = component->GetFOV();
		if (ImGui::SliderFloat("FOV", &Inspector_FOV, 30, 120, "%0.2f", ImGuiSliderFlags_None)) { component->SetFOV(Inspector_FOV); }

		//Set NearPlane
		float NearPlane = component->GetNearPlane();
		if (ImGui::DragFloat("Near plane", &NearPlane)) { component->SetNearPlane(NearPlane); }

		//Set FarPlane
		float FarPlane = component->GetFarPlane();
		if (ImGui::DragFloat("Far plane", &FarPlane)) { component->SetFarPlane(FarPlane); }

		//Add Toggle for frustum draw?

	}
}

void WindowInspector::CleanUp()
{
}
