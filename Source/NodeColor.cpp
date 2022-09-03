#include "NodeColor.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"

#include "Dependencies/ImGui/imgui_internal.h"
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>

#include "Random.h"
#include "PinFloat4.h"
#include "PinFloat4Array.h"

NodeColor::NodeColor(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::Color),
color1(float4::one),
color2(float4::one)
{
	PinFloat4* newPin = new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Color1");

	newPin->pinFloat4 = float4::one;

	Inputs.emplace_back(newPin);

	newPin = new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Color2");

	newPin->pinFloat4 = float4::one;

	Inputs.emplace_back(newPin);



	Outputs.emplace_back(new PinFloat4Array(App->editor->nodeEditorWindow->GetNextId(), "Color"));

}

NodeColor::~NodeColor()
{
}

float4 NodeColor::ComputeColor()
{
	return float4(Random::GenerateRandomFloatRange(color1.x, color2.x),
		Random::GenerateRandomFloatRange(color1.y, color2.y),
		Random::GenerateRandomFloatRange(color1.z, color2.z),
		Random::GenerateRandomFloatRange(color1.w, color2.w));
}

void NodeColor::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
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
                color1 = pinFloat4->pinFloat4;
            }
        }
        else if (pinFloat4->Name == "Color2")
        {
            if (ImGui::ColorEdit4(pinFloat4->Name.c_str(), (float*)&pinFloat4->pinFloat4, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoOptions))
            {
                color2 = pinFloat4->pinFloat4;
            }
        }


        ImGui::PopStyleVar();

        ed::EndPin();
    }

    ImGui::NextColumn();

    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 40);

    ImGui::Dummy(ImVec2(0, 10));

    PinFloat4Array* pinFloatArray = (PinFloat4Array*)Outputs[0];

    if (!pinFloatArray)
        return;


    if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, pinFloatArray) && pinFloatArray != nodeEditorWindow->newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

    ed::BeginPin(pinFloatArray->ID, ed::PinKind::Output);

    nodeEditorWindow->DrawPinIcon(pinFloatArray, nodeEditorWindow->IsPinLinked(pinFloatArray->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    ImGui::EndColumns();


    //Update Node Links

    if (pinFloatArray)
    {
        pinFloatArray->float4Array[0] = ComputeColor();
    }

    Pin* linkedPin = nodeEditorWindow->GetPinLinkedTo(pinFloatArray->ID);

    if (linkedPin && pinFloatArray)
        nodeEditorWindow->UpdateNodeLinks(pinFloatArray, linkedPin);

}
