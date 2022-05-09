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
