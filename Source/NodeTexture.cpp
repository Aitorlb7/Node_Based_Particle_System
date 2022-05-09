#include "NodeTexture.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleResource.h"

#include "WindowNodeEditor.h"

#include "PinTexture.h"

NodeTexture::NodeTexture(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::Texture)
{
	Outputs.emplace_back(new PinTexture(App->editor->nodeEditorWindow->GetNextId(), "Texture"));
}

NodeTexture::~NodeTexture()
{
}

ResourceTexture* NodeTexture::GetDefaultParticleTex()
{
	return App->resources->GetTexture("DefaultParticle");

}
