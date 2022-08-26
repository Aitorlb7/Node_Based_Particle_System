#include "NodeAlignment.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "WindowNodeEditor.h"
#include "PinInt.h"
#include "Dependencies/ImGui/imgui_internal.h"
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>

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

void NodeAlignment::Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow)
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

    PinInt* pinInt = (PinInt*)Outputs[0];

    if (!pinInt) return;

    auto alpha = ImGui::GetStyle().Alpha;
    if (nodeEditorWindow->newLinkPin && !nodeEditorWindow->CanCreateLink(nodeEditorWindow->newLinkPin, pinInt) && pinInt != nodeEditorWindow->newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

    //TODO Set default selcetd

    ImGui::BeginColumns("Columns", 2, ImGuiColumnsFlags_NoBorder);
    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 70);

    static int selected = -1;
    for (int i = 1; i < 9; i++)
    {

        if (ImGui::Selectable(aligmentArray[i], selected == i))
        {
            pinInt->pinInt = i;
            updateLinks = true;
            selected = i;
        }
    }

    ImGui::NextColumn();

    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 40);

    ImGui::Dummy(ImVec2(0, 50));

    ed::BeginPin(pinInt->ID, ed::PinKind::Output);

    nodeEditorWindow->DrawPinIcon(pinInt, nodeEditorWindow->IsPinLinked(pinInt->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    ImGui::EndColumns();

    if (updateLinks)
    {
        updateLinks = false;

        Pin* linkedPin = nodeEditorWindow->GetPinLinkedTo(pinInt->ID);

        if (linkedPin && pinInt) nodeEditorWindow->UpdateNodeLinks(pinInt, linkedPin);
    }

}
