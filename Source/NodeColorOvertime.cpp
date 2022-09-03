#include "NodeColorOvertime.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"

#include "Random.h"
#include "PinFloat4.h"
#include "PinFloat4Array.h"
#include "PinFloat.h"

#include "Dependencies/ImGui/imgui_internal.h"
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>

NodeColorOvertime::NodeColorOvertime(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::ColorOverTime)
{

	PinFloat4* newPin = new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Color1");

	newPin->pinFloat4 = float4::one;

	Inputs.emplace_back(newPin);

	newPin = new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Color2");

	newPin->pinFloat4 = float4::one;

	Inputs.emplace_back(newPin);



	PinFloat4Array* outputPin = new PinFloat4Array(App->editor->nodeEditorWindow->GetNextId(), "ColorOvertime");

	outputPin->float4Array[0] = float4::one;
	outputPin->float4Array[1] = float4::one;

	Outputs.emplace_back(outputPin);

}

NodeColorOvertime::~NodeColorOvertime()
{
}

void NodeColorOvertime::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
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



    PinFloat4* pinFloat4 = nullptr;

    PinFloat4Array* pinFloat4Array = nullptr;


    pinFloat4Array = (PinFloat4Array*)Outputs[0];

    auto alpha = ImGui::GetStyle().Alpha;

    ImGui::BeginColumns("Columns", 2, ImGuiColumnsFlags_NoBorder);
    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 450);


    for (Pin* input : Inputs)
    {
        pinFloat4 = ((PinFloat4*)input);

        if (!pinFloat4)
            continue;


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

        if (pinFloat4->Name == "Color1")
        {
            if (ImGui::ColorEdit4(pinFloat4->Name.c_str(), (float*)&pinFloat4->pinFloat4, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoOptions))
            {
                pinFloat4Array->float4Array[0] = pinFloat4->pinFloat4;
            }
        }
        else if (pinFloat4->Name == "Color2")
        {
            if (ImGui::ColorEdit4(pinFloat4->Name.c_str(), (float*)&pinFloat4->pinFloat4, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoOptions))
            {
                pinFloat4Array->float4Array[1] = pinFloat4->pinFloat4;
            }
        }


        ImGui::PopStyleVar();

        ed::EndPin();
    }

    ImGui::NextColumn();

    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 40);

    ImGui::Dummy(ImVec2(0, 10));


    if (!pinFloat4Array)
        return;


    if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, pinFloat4Array) && pinFloat4Array != nodeEditorWindow->newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

    ed::BeginPin(pinFloat4Array->ID, ed::PinKind::Output);

    nodeEditorWindow->DrawPinIcon(pinFloat4Array, nodeEditorWindow->IsPinLinked(pinFloat4Array->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    ImGui::EndColumns();


    Pin* linkedPin = nodeEditorWindow->GetPinLinkedTo(pinFloat4Array->ID);

    if (linkedPin && pinFloat4Array)
        nodeEditorWindow->UpdateNodeLinks(pinFloat4Array, linkedPin);

}

