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

void NodeTexture::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
{

    //Header------------------------------------

    builder.Header(Color);
    builder.SetCustomNode();
    ImGui::Spring(0);
    ImGui::TextUnformatted(Name.c_str());
    ImGui::Spring(1);
    ImGui::Dummy(ImVec2(0, 28));
    ImGui::Spring(0);
    builder.EndHeader();

    //Output------------------------------------

    PinTexture* tempPinTex = (PinTexture*)Outputs[0];

    auto alpha = ImGui::GetStyle().Alpha;
    if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, tempPinTex) && tempPinTex != nodeEditorWindow->newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);


    if (!tempPinTex->pinTexture)
    {
        tempPinTex->pinTexture = GetDefaultParticleTex();
    }

    //TODO: Choose texture

    ImGui::Image((ImTextureID)tempPinTex->pinTexture->id, ImVec2(80, 80), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset"))
        {
            uint32 UID = *(const uint32*)payload->Data;
            Resource* resource = App->resources->GetResourceInMemory(UID);
            if (resource->type == ResourceType::Texture)
            {
                tempPinTex->pinTexture = (ResourceTexture*)App->resources->LoadResource(UID);
                updateLinks = true;
            }
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();

    ed::BeginPin(tempPinTex->ID, ed::PinKind::Output);

    nodeEditorWindow->DrawPinIcon(tempPinTex, nodeEditorWindow->IsPinLinked(tempPinTex->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    if (updateLinks)
    {
        updateLinks = false;

        Pin* linkedPin = nodeEditorWindow->GetPinLinkedTo(tempPinTex->ID);

        if (linkedPin && tempPinTex) nodeEditorWindow->UpdateNodeLinks(tempPinTex, linkedPin);
    }

}
