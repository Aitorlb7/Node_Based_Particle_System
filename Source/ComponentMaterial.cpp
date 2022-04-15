#pragma once
#include "Application.h"
#include "ModuleResource.h"
#include "ComponentMaterial.h"
#include "ResourceMaterial.h"
#include "ResourceShader.h"
#include "ImporterShader.h"
#include "GameObject.h"
#include <string>

#include "ComponentTransform.h"

ComponentMaterial::ComponentMaterial(GameObject* owner) : Component(owner)
{
	rMaterial = new ResourceMaterial();
	type = ComponentType::Material;
};

ComponentMaterial::ComponentMaterial(GameObject* owner, const char* texturePath, uint id) : Component(owner)
{
	rMaterial = new ResourceMaterial();
	rMaterial->SetTextureId(id);
	rMaterial->SetPath(texturePath);
	type = ComponentType::Material;
}
ComponentMaterial::ComponentMaterial(GameObject* owner, ResourceMaterial* _rMaterial) : Component(owner)
{
	rMaterial = _rMaterial;
	type = ComponentType::Material;
}


void ComponentMaterial::Enable() {

}

void ComponentMaterial::Disable() {

}

void ComponentMaterial::Update() {

}

void ComponentMaterial::CleanUp()
{
	rMaterial->SetTextureId(0);
	rMaterial = nullptr;
	delete rMaterial;
}

