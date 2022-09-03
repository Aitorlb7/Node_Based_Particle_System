#include "NodeGravitationalField.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"
#include "EmitterInstance.h"

#include "PinFloat.h"
#include "PinFlow.h"

NodeGravitationalField::NodeGravitationalField(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::GravitationalField),
gameObject(nullptr),
addedField(false)
{
	Inputs.emplace_back(new PinFlow(App->editor->nodeEditorWindow->GetNextId(), ""));

	Outputs.emplace_back(new PinFlow(App->editor->nodeEditorWindow->GetNextId(), ""));

	PinFloat* pinFloat = new PinFloat(App->editor->nodeEditorWindow->GetNextId(), "Intensity");

    pinFloat->pinFloat = -9.8f;

    Outputs.emplace_back(pinFloat);

    pinFloat = new PinFloat(App->editor->nodeEditorWindow->GetNextId(), "Range");

    pinFloat->pinFloat = 20.f;

	Outputs.emplace_back(pinFloat);


	gameObject = App->scene->CreateGameObject("GravitationalPull", App->scene->root_object);

    App->scene->SelectObject(gameObject);
}

NodeGravitationalField::~NodeGravitationalField()
{
    PinFlow* inputPinFlow = (PinFlow*)Inputs[0];

    inputPinFlow->emitterInstance->DeleteGravitationalField(this);
}

void NodeGravitationalField::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
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

    PinFlow* inputPinFlow = (PinFlow*)Inputs[0];

    PinFlow* outputPinFlow = (PinFlow*)Outputs[0];

    if (!addedField && inputPinFlow->emitterInstance)
    {
        inputPinFlow->emitterInstance->AddGravitationalField(this);

        outputPinFlow->emitterInstance = inputPinFlow->emitterInstance;

        addedField = true;
    }



    PinFloat* tempPinFloat = (PinFloat*)Outputs[1];

    if (!tempPinFloat) return;


    //if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, tempPinFloat) && tempPinFloat != nodeEditorWindow->newLinkPin)
    //    alpha = alpha * (48.0f / 255.0f);

    //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

    ImGui::PushItemWidth(80);

    if (ImGui::DragFloat(tempPinFloat->Name.c_str(), &tempPinFloat->pinFloat, 0.1f)) updateLinks = true;

    ImGui::PopItemWidth();

    PinFloat* tempPinFloat2 = (PinFloat*)Outputs[2];

    ImGui::PushItemWidth(80);

    if (ImGui::DragFloat(tempPinFloat2->Name.c_str(), &tempPinFloat2->pinFloat, 0.1f)) updateLinks = true;

    ImGui::PopItemWidth();


    //ImGui::PopStyleVar();


    if (updateLinks)
    {
        updateLinks = false;

        Pin* linkedPin = nodeEditorWindow->GetPinLinkedTo(tempPinFloat->ID);

        if (linkedPin && tempPinFloat)
            nodeEditorWindow->UpdateNodeLinks(tempPinFloat, linkedPin);

        linkedPin = nodeEditorWindow->GetPinLinkedTo(tempPinFloat2->ID);

        if (linkedPin && tempPinFloat2)
            nodeEditorWindow->UpdateNodeLinks(tempPinFloat2, linkedPin);
    }

}
