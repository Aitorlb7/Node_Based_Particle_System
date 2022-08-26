#include "NodeGravitationalPull.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"

#include "PinFloat.h"
#include "PinFlow.h"
NodeGravitationalPull::NodeGravitationalPull(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::GravitationalPull)
{
	Inputs.emplace_back(new PinFlow(App->editor->nodeEditorWindow->GetNextId(), ""));

	Outputs.emplace_back(new PinFlow(App->editor->nodeEditorWindow->GetNextId(), ""));

	PinFloat* pinFloat = new PinFloat(App->editor->nodeEditorWindow->GetNextId(), "Float");

	pinFloat->pinFloat = -9.8f;

	Outputs.emplace_back(pinFloat);


	gameObject = App->scene->CreateGameObject("GravitationalPull", App->scene->root_object);
}

NodeGravitationalPull::~NodeGravitationalPull()
{
}

void NodeGravitationalPull::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
{

    auto alpha = ImGui::GetStyle().Alpha;

    //Header------------------------------------

    builder.Header(Color);
    builder.SetCustomNode();

    ed::BeginPin(Inputs[0]->ID, ed::PinKind::Input);
    nodeEditorWindow->DrawPinIcon(Inputs[0], nodeEditorWindow->IsPinLinked(Inputs[0]->ID), (int)(alpha * 255));
    ed::EndPin();

    ImGui::Spring(0);
    ImGui::TextUnformatted(Name.c_str());
    ImGui::Spring(1);
    ImGui::Dummy(ImVec2(0, 28));

    ed::BeginPin(Outputs[0]->ID, ed::PinKind::Output);
    nodeEditorWindow->DrawPinIcon(Outputs[0], nodeEditorWindow->IsPinLinked(Outputs[0]->ID), (int)(alpha * 255));
    ed::EndPin();

    ImGui::Spring(0);
    builder.EndHeader();

    //Output------------------------------------

    PinFloat* tempPinFloat = (PinFloat*)Outputs[1];

    if (!tempPinFloat) return;


    if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, tempPinFloat) && tempPinFloat != nodeEditorWindow->newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

    ImGui::PushItemWidth(80);

    if (ImGui::DragFloat("Float", &tempPinFloat->pinFloat, 0.1f)) updateLinks = true;

    PinFlow* tempPinFlow = (PinFlow*)Inputs[0];

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
