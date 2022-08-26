#include "NodeFloat3.h"
#include "PinFloat3.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"

NodeFloat3::NodeFloat3(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::Float3)
{
	Inputs.emplace_back(new PinFloat3(App->editor->nodeEditorWindow->GetNextId(), "Float3"));

	Outputs.emplace_back(new PinFloat3(App->editor->nodeEditorWindow->GetNextId(), "OutFloat3"));
}

NodeFloat3::~NodeFloat3()
{
}

void NodeFloat3::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
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

    PinFloat3* tempPinFloat3 = (PinFloat3*)Outputs[0];

    if (!tempPinFloat3) return;

    auto alpha = ImGui::GetStyle().Alpha;
    if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, tempPinFloat3) && tempPinFloat3 != nodeEditorWindow->newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);




    ImGui::PushItemWidth(80);

    if (ImGui::DragFloat("X", &tempPinFloat3->pinFloat3.x, 0.1f)) updateLinks = true;

    ImGui::SameLine();

    if (ImGui::DragFloat("Y", &tempPinFloat3->pinFloat3.y, 0.1f)) updateLinks = true;

    ImGui::SameLine();

    if (ImGui::DragFloat("Z", &tempPinFloat3->pinFloat3.z, 0.1f)) updateLinks = true;


    ImGui::PopItemWidth();

    ImGui::SameLine();

    ed::BeginPin(tempPinFloat3->ID, ed::PinKind::Output);

    nodeEditorWindow->DrawPinIcon(tempPinFloat3, nodeEditorWindow->IsPinLinked(tempPinFloat3->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    if (updateLinks)
    {
        updateLinks = false;

        Pin* linkedPin = nodeEditorWindow->GetPinLinkedTo(tempPinFloat3->ID);

        if (linkedPin && tempPinFloat3)
            nodeEditorWindow->UpdateNodeLinks(tempPinFloat3, linkedPin);
    }


}
