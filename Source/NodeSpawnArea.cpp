#include "NodeSpawnArea.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ComponentTransform.h"
#include "ResourceModel.h"
#include "ModuleResource.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"
#include "WindowAssetExplorer.h"

#include "Random.h"
#include "PinFloat3.h"
#include "PinBool.h"
#include "PinGameObject.h"
#include "PinTexture.h"

#include "Dependencies/ImGui/imgui_internal.h"
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>
NodeSpawnArea::NodeSpawnArea(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::SpawnArea),
areaResourceMesh(nullptr),
isAreaVisible(true)
{

	Outputs.emplace_back(new PinBool(App->editor->nodeEditorWindow->GetNextId(), "Visible"));
	
    PinGameObject* pinGameObject = new PinGameObject(App->editor->nodeEditorWindow->GetNextId(), "GameObject");

    Outputs.emplace_back(pinGameObject);


	App->resources->GetModel("Sphere2");

    pinGameObject->gameObject = App->scene->game_objects.back();

    App->scene->SelectObject(pinGameObject->gameObject);


}

NodeSpawnArea::~NodeSpawnArea()
{

    PinGameObject* pinGameObject = (PinGameObject*)Outputs[1];

    App->scene->DeleteGameObject(pinGameObject->gameObject->parent);

}

float3 NodeSpawnArea::ComputePosition()
{
	//if (!areaResourceMesh) return float3::zero;

 //   float3 randomPos = pinGameObject->gameObject->transform->GetPosition();

 //   int size = areaResourceMesh->size[ResourceMesh::index];

 //	int randomIndex = Random::GenerateRandomIntRange(0, size - 3);


 //   randomPos.x += (areaResourceMesh->vertices[(areaResourceMesh->indices[randomIndex] * 3) + 1] * areaGameObject->transform->GetScale().x);
 //   randomPos.y += (areaResourceMesh->vertices[(areaResourceMesh->indices[randomIndex] * 3) + 2] * areaGameObject->transform->GetScale().y);
 //   randomPos.z += (areaResourceMesh->vertices[(areaResourceMesh->indices[randomIndex] * 3)] * areaGameObject->transform->GetScale().z);



 //   //for (int i = 0; i < size; ++i)
 //   //{
 //   //    if (i >= randomIndex - 3)
 //   //    {
 //   //        randomPos.x += (areaResourceMesh->vertices[(areaResourceMesh->indices[i] * 3) + 1] * areaGameObject->transform->GetScale().x);
 //   //        randomPos.y += (areaResourceMesh->vertices[(areaResourceMesh->indices[i] * 3) + 2] * areaGameObject->transform->GetScale().y);
 //   //        randomPos.z += (areaResourceMesh->vertices[(areaResourceMesh->indices[i] * 3) ] * areaGameObject->transform->GetScale().z);
 //   //        break;
 //   //    }
 //   //}
	//

	//return randomPos;

    return float3::zero;
}

void NodeSpawnArea::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
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

    PinBool* tempPinBool = (PinBool*)Outputs[0];

    PinGameObject* pinGameObject = (PinGameObject*)Outputs[1];

    if (!tempPinBool || !pinGameObject) return;

    auto alpha = ImGui::GetStyle().Alpha;
    if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, tempPinBool) && tempPinBool != nodeEditorWindow->newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

    updateLinks = true;

    ImGui::PushItemWidth(80);

    if (ImGui::Checkbox(tempPinBool->Name.c_str(), &isAreaVisible))
    {
        pinGameObject->gameObject->active = isAreaVisible;
    }

    ImGui::PopItemWidth();


    ImGui::Dummy(ImVec2(0, 10));

    ImGui::Image((ImTextureID)App->editor->explorerWindow->modelIcon->id, ImVec2(80, 80), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset"))
        {
            uint32 UID = *(const uint32*)payload->Data;
            Resource* resource = App->resources->GetResourceInMemory(UID);

            if (resource->type == ResourceType::Model)
            {
                App->scene->DeleteGameObject(pinGameObject->gameObject->parent);

                ResourceModel* model = (ResourceModel*)App->resources->LoadResource(UID); 
                
                pinGameObject->gameObject = App->scene->game_objects.back();

                isAreaVisible = pinGameObject->gameObject->active;
            }
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();

    ed::BeginPin(pinGameObject->ID, ed::PinKind::Output);

    nodeEditorWindow->DrawPinIcon(pinGameObject, nodeEditorWindow->IsPinLinked(pinGameObject->ID), (int)(alpha * 255));

    ed::EndPin();

    ImGui::BeginColumns("Columns", 2, ImGuiColumnsFlags_NoBorder);
    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 20);

    ImGui::NextColumn();

    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 100);

    ImGui::Text(pinGameObject->gameObject->name.c_str());


    ImGui::EndColumns();
  
    ImGui::PopStyleVar();


    if (updateLinks)
    {
        updateLinks = false;

        Pin* linkedPin = nodeEditorWindow->GetPinLinkedTo(pinGameObject->ID);

        if (linkedPin && pinGameObject)
            nodeEditorWindow->UpdateNodeLinks(pinGameObject, linkedPin);

    }
}
