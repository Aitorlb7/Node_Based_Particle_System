#include "NodeVelocity.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"
#include "Dependencies/ImGui/imgui_internal.h"
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>

#include "Random.h"
#include "PinFloat4.h"
#include "PinFloat4Array.h"

NodeVelocity::NodeVelocity(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::Velocity),
initialVelocity1(float4(1.0f,1.0f,1.0f,3.0f)),
initialVelocity2(float4(-1.0f, 1.0f, -1.0f, 5.0f))
{
	PinFloat4* newPin = new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Velocity1");

	newPin->pinFloat4 = initialVelocity1;

	Inputs.emplace_back(newPin);

	newPin = new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Velocity2");

	newPin->pinFloat4 = initialVelocity2;

	Inputs.emplace_back(newPin);

    PinFloat4Array* outputPin = new PinFloat4Array(App->editor->nodeEditorWindow->GetNextId(), "Velocity");

    outputPin->float4Array[0] = initialVelocity1;
    outputPin->float4Array[1] = initialVelocity2;

    Outputs.emplace_back(outputPin);

}

NodeVelocity::~NodeVelocity()
{
}

float4 NodeVelocity::ComputeVelocity()
{
    return float4(Random::GenerateRandomFloat4(initialVelocity1,initialVelocity2));
}

void NodeVelocity::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
{

    //TODO: Specify which node or pin to perma update
   // I shouldn't be updating them every frame
    updateLinks = true;


    //Header------------------------------------

    builder.Header(Color);
    builder.SetCustomNode();
    ImGui::Spring(0);
    ImGui::TextUnformatted(Name.c_str());
    ImGui::Spring(1);
    ImGui::Dummy(ImVec2(0, 28));
    ImGui::Spring(0);
    builder.EndHeader();

    //Inputs------------------------------------

    PinFloat4* tempPinFloat4 = nullptr;

    ImGui::BeginColumns("Columns", 2, ImGuiColumnsFlags_NoBorder);
    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 520);

    PinFloat4Array* pinFloat4Array = (PinFloat4Array*)Outputs[0];

    if (!pinFloat4Array)
        return;

    for (Pin* input : Inputs)
    {
        tempPinFloat4 = ((PinFloat4*)input);

        if (!tempPinFloat4)
            continue;

        auto alpha = ImGui::GetStyle().Alpha;
        if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, input) && input != nodeEditorWindow->newLinkPin)
            alpha = alpha * (48.0f / 255.0f);

        ed::BeginPin(input->ID, ed::PinKind::Input);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

        if (!input->Name.empty())
        {
            ImGui::TextUnformatted(input->Name.c_str());
            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();

        }

        ImGui::PushItemWidth(80);

        if (tempPinFloat4->Name == "Velocity1")
        {
            if (ImGui::DragFloat("X1", &tempPinFloat4->pinFloat4.x, 0.1f))
            {
                pinFloat4Array->float4Array[0].x = tempPinFloat4->pinFloat4.x;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Y1", &tempPinFloat4->pinFloat4.y, 0.1f))
            {
                pinFloat4Array->float4Array[0].y = tempPinFloat4->pinFloat4.y;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Z1", &tempPinFloat4->pinFloat4.z, 0.1f))
            {
                pinFloat4Array->float4Array[0].z = tempPinFloat4->pinFloat4.z;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Speed1", &tempPinFloat4->pinFloat4.w, 0.1f))
            {
                pinFloat4Array->float4Array[0].w = tempPinFloat4->pinFloat4.w;
            }
        }
        else if (tempPinFloat4->Name == "Velocity2")
        {
            if (ImGui::DragFloat("X2", &tempPinFloat4->pinFloat4.x, 0.1f))
            {
                pinFloat4Array->float4Array[1].x = tempPinFloat4->pinFloat4.x;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Y2", &tempPinFloat4->pinFloat4.y, 0.1f))
            {
                pinFloat4Array->float4Array[1].y = tempPinFloat4->pinFloat4.y;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Z2", &tempPinFloat4->pinFloat4.z, 0.1f))
            {
                pinFloat4Array->float4Array[1].z = tempPinFloat4->pinFloat4.z;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Speed2", &tempPinFloat4->pinFloat4.w, 0.1f))
            {
                pinFloat4Array->float4Array[1].w = tempPinFloat4->pinFloat4.w;
            }
        }

        ImGui::PopItemWidth();

        ImGui::SameLine();

        ImGui::PopStyleVar();

        ed::EndPin();
    }


    //Output--------------------------------

    ImGui::NextColumn();

    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 40);

    ImGui::Dummy(ImVec2(0, 10));

    

    auto alpha = ImGui::GetStyle().Alpha;
    if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, pinFloat4Array) && pinFloat4Array != nodeEditorWindow->newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

    ed::BeginPin(pinFloat4Array->ID, ed::PinKind::Input);

    nodeEditorWindow->DrawPinIcon(pinFloat4Array, nodeEditorWindow->IsPinLinked(pinFloat4Array->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    ImGui::EndColumns();


    Pin* linkedPin = nodeEditorWindow->GetPinLinkedTo(pinFloat4Array->ID);

    if (linkedPin && pinFloat4Array)
        nodeEditorWindow->UpdateNodeLinks(pinFloat4Array, linkedPin);
}

