#include "NodeGravity.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"
#include "EmitterInstance.h"
#include "PinFloat.h"
#include "PinFlow.h"
NodeGravity::NodeGravity(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::Gravity)
{
	Inputs.emplace_back(new PinFlow(App->editor->nodeEditorWindow->GetNextId(), ""));
	
	Outputs.emplace_back(new PinFlow(App->editor->nodeEditorWindow->GetNextId(), ""));

	PinFloat* pinFloat = new PinFloat(App->editor->nodeEditorWindow->GetNextId(), "Float");

	pinFloat->pinFloat = -9.8f;

	Outputs.emplace_back(pinFloat);
}

NodeGravity::~NodeGravity()
{
    PinFlow* inputPinFlow = (PinFlow*)Inputs[0];

    inputPinFlow->emitterInstance->forceVector = float3(0.0, 0.0f, 0.0);
}

void NodeGravity::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
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

    PinFlow* inputPinFlow = (PinFlow*)Inputs[0];

    PinFlow* outputPinFlow = (PinFlow*)Outputs[0];

    if (inputPinFlow->emitterInstance)
    {
        inputPinFlow->emitterInstance->forceVector = float3(0.0, tempPinFloat->pinFloat, 0.0);

        outputPinFlow->emitterInstance = inputPinFlow->emitterInstance;
    }

    ImGui::PopItemWidth();

    ImGui::PopStyleVar();


    if (updateLinks)
    {
        updateLinks = false;

        Pin* linkedPin = nodeEditorWindow->GetPinLinkedTo(tempPinFloat->ID);

        if (linkedPin && tempPinFloat)
            nodeEditorWindow->UpdateNodeLinks(tempPinFloat, linkedPin);
    }


}
