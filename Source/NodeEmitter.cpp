#include "NodeEmitter.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"
#include "ComponentParticleSystem.h"

#include "PinFloat3.h"
#include "PinFloat4.h"
#include "PinFloat4Array.h"
#include "PinString.h"
#include "PinInt.h"
#include "PinFloat.h"
#include "PinBool.h"
#include "PinTexture.h"
#include "PinFlow.h"
#include "PinGameObject.h"

NodeEmitter::NodeEmitter(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::Emitter)
{




    Outputs.emplace_back(new PinFlow(App->editor->nodeEditorWindow->GetNextId(), ""));

    Inputs.emplace_back(new PinFloat(App->editor->nodeEditorWindow->GetNextId(), "Spawn Rate"));
    Inputs.emplace_back(new PinFloat(App->editor->nodeEditorWindow->GetNextId(), "Lifetime"));
    Inputs.emplace_back(new PinFloat(App->editor->nodeEditorWindow->GetNextId(), "Size"));
    Inputs.emplace_back(new PinFloat4Array(App->editor->nodeEditorWindow->GetNextId(), "Velocity"));
    Inputs.emplace_back(new PinFloat4Array(App->editor->nodeEditorWindow->GetNextId(), "Color"));
    Inputs.emplace_back(new PinInt(App->editor->nodeEditorWindow->GetNextId(), "Alignment"));
    Inputs.emplace_back(new PinTexture(App->editor->nodeEditorWindow->GetNextId(), "Texture"));
    Inputs.emplace_back(new PinGameObject(App->editor->nodeEditorWindow->GetNextId(), "Spawn From Model"));
    Inputs.emplace_back(new PinBool(App->editor->nodeEditorWindow->GetNextId(), "Active"));


    ComponentParticleSystem* newComponent = (ComponentParticleSystem*)App->scene->game_objects.back()->GetComponent(ComponentType::ParticleSystem);

    if (!newComponent)
    {
        GameObject* newGameObject = App->scene->CreateGameObject("", App->scene->root_object);

        newComponent = new ComponentParticleSystem(newGameObject);

        newGameObject->AddComponent(newComponent);

        newComponent->SetResourceProperties(newComponent->GetParticleSystem(), this);

        PinFlow* pin = (PinFlow*)Outputs[0];

        pin->emitterInstance = &newComponent->emitters.back();

        App->scene->SelectObject(newGameObject);
    }
}

NodeEmitter::~NodeEmitter()
{
}

void NodeEmitter::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
{

    auto alpha = ImGui::GetStyle().Alpha;

    //Header------------------------------------

    builder.Header(Color);
    builder.SetCustomNode();

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

    for (Pin* input : Inputs)
    {
        auto alpha = ImGui::GetStyle().Alpha;
        if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, input) && input != nodeEditorWindow->newLinkPin)
            alpha = alpha * (48.0f / 255.0f);

        builder.Input(input->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        nodeEditorWindow->DrawPinIcon(input, nodeEditorWindow->IsPinLinked(input->ID), (int)(alpha * 255));
        ImGui::Spring(0);
        if (!input->Name.empty())
        {
            ImGui::TextUnformatted(input->Name.c_str());
            ImGui::Spring(0);
        }
        ImGui::PopStyleVar();
        builder.EndInput();
    }
}