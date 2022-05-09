#include "NodeVelocity.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"

#include "Random.h"

#include "PinFloat4.h"

NodeVelocity::NodeVelocity(int id, const char* name, ImColor color) : Node(id, name, color, NodeType::Velocity),
initialVelocity1(float4(1.0f,1.0f,1.0f,3.0f)),
initialVelocity2(float4(-1.0f, 1.0f, -1.0f, 5.0f))
{
	PinFloat4* newPin = new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Velocity1");

	newPin->pinFloat4 = initialVelocity1;

	Inputs.emplace_back(newPin);

	newPin = new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Velocity2");

	newPin->pinFloat4 = initialVelocity2;

	Inputs.emplace_back(newPin);



    Outputs.emplace_back(new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Velocity"));

}

NodeVelocity::~NodeVelocity()
{
}

float4 NodeVelocity::ComputeVelocity()
{
    return float4(Random::GenerateRandomFloat4(initialVelocity1,initialVelocity2));
}

