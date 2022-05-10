#include "NodeColor.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"

#include "Random.h"
#include "PinFloat4.h"

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



	Outputs.emplace_back(new PinFloat4(App->editor->nodeEditorWindow->GetNextId(), "Color"));

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
