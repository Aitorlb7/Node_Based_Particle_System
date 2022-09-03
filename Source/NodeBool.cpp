#include "NodeBool.h"
#include "PinBool.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"


NodeBool::NodeBool(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::Bool)
{

	Outputs.emplace_back(new PinBool(App->editor->nodeEditorWindow->GetNextId(), "OutBool"));
}

NodeBool::~NodeBool()
{
}

void NodeBool::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
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

    if (!tempPinBool) return;

    auto alpha = ImGui::GetStyle().Alpha;
    if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, tempPinBool) && tempPinBool != nodeEditorWindow->newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);


    ImGui::PushItemWidth(80);

    if(ImGui::Checkbox(Name.c_str(), &tempPinBool->pinBool)) updateLinks = true;

    ImGui::PopItemWidth();

    ImGui::SameLine();

    ed::BeginPin(tempPinBool->ID, ed::PinKind::Output);

    nodeEditorWindow->DrawPinIcon(tempPinBool, nodeEditorWindow->IsPinLinked(tempPinBool->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    if (updateLinks)
    {
        updateLinks = false;

        Pin* linkedPin = nodeEditorWindow->GetPinLinkedTo(tempPinBool->ID);

        if (linkedPin && tempPinBool)
            nodeEditorWindow->UpdateNodeLinks(tempPinBool, linkedPin);
    }
}
