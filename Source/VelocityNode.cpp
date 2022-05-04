#include "VelocityNode.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"

#include "Random.h"

#include "PinFloat.h"
#include "PinFloat3.h"
#include "PinFloat4.h"

VelocityNode::VelocityNode(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::Velocity),
initialVelocity1(float4::zero),
initialVelocity2(float4::zero)
{

    Inputs.emplace_back(new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Velocity1"));
    //Inputs.emplace_back(new PinFloat(App->editor->nodeEditorWindow->GetNextId(), "Speed1"));

    Inputs.emplace_back(new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Velocity2"));
    //Inputs.emplace_back(new PinFloat(App->editor->nodeEditorWindow->GetNextId(), "Speed2"));

    Outputs.emplace_back(new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Velocity"));

}

VelocityNode::~VelocityNode()
{
}

float4 VelocityNode::ComputeVelocity()
{
    return float4(Random::GenerateRandomFloat3(initialVelocity1.xyz(), initialVelocity2.xyz()), Random::GenerateRandomIntRange((int)initialVelocity1.w, (int)initialVelocity2.w));
}

