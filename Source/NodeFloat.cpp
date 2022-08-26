#include "NodeFloat.h"
#include "PinFloat.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"

NodeFloat::NodeFloat(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::Float)
{
	Inputs.emplace_back(new PinFloat(App->editor->nodeEditorWindow->GetNextId(), "Float"));

	Outputs.emplace_back(new PinFloat(App->editor->nodeEditorWindow->GetNextId(), "OutFloat"));
}


NodeFloat::~NodeFloat()
{
}

void NodeFloat::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
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

    PinFloat* tempPinFloat = (PinFloat*)Outputs[0];

    if (!tempPinFloat) return;

    auto alpha = ImGui::GetStyle().Alpha;
    if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, tempPinFloat) && tempPinFloat != nodeEditorWindow->newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);


    ImGui::PushItemWidth(80);

    if (ImGui::DragFloat("Float", &tempPinFloat->pinFloat, 0.1f)) updateLinks = true;


    ImGui::PopItemWidth();

    ImGui::SameLine();

    ed::BeginPin(tempPinFloat->ID, ed::PinKind::Output);

    nodeEditorWindow->DrawPinIcon(tempPinFloat, nodeEditorWindow->IsPinLinked(tempPinFloat->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    if (updateLinks)
    {
        updateLinks = false;

        Pin* linkedPin = nodeEditorWindow->GetPinLinkedTo(tempPinFloat->ID);

        if (linkedPin && tempPinFloat)
            nodeEditorWindow->UpdateNodeLinks(tempPinFloat, linkedPin);
    }
}
