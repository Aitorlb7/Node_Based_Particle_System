#include "NodeAlignment.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"
#include "PinInt.h"
NodeAlignment::NodeAlignment(int id, const char* name, ImColor color) : Node(id,name,color, NodeType::Alignment),
currentAlignment(aligmentArray[1])
{
	Outputs.emplace_back(new PinInt(App->editor->nodeEditorWindow->GetNextId(), "Alignment"));
}

NodeAlignment::~NodeAlignment()
{
}

std::string NodeAlignment::GetStringBasedOnAlignment(Alignment alignment)
{
	switch (alignment)
	{
	case(Alignment::Screen): return "Screen";
	case(Alignment::Camera): return "Camera";
	case(Alignment::AxisXY): return "AxisXY";
	case(Alignment::AxisXZ): return "AxisXZ";
	case(Alignment::AxisYX): return "AxisYX";
	case(Alignment::AxisYZ): return "AxisYZ";
	case(Alignment::AxisZX): return "AxisZX";
	case(Alignment::AxisZY): return "AxisZY";
	}

	return "";
}
