#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleResource.h"
#include "WindowNodeEditor.h"
#include "EmitterInstance.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Dependencies/ImGui/imgui_internal.h"
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>

#include "NodeBool.h"
#include "NodeComment.h"
#include "NodeFloat.h"
#include "NodeVelocity.h"
#include "NodeFloat3.h"
#include "NodeTexture.h"
#include "NodeAlignment.h"
#include "NodeColor.h"
#include "NodeColorOvertime.h"
#include "NodeGravity.h"
#include "NodeGravitationalField.h"
#include "NodeEmitter.h"
#include "NodeSpawnArea.h"

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

#include "Dependencies/MathGeoLib/include/Math/float3.h"

WindowNodeEditor::WindowNodeEditor(bool isActive): Window("Particle System Node Editor", isActive)
{
}

WindowNodeEditor::~WindowNodeEditor()
{

}

//TODO: STATIC really????
 void TouchNode(ed::NodeId id)
{
    App->editor->nodeEditorWindow->nodeTouchTime[id] = App->editor->nodeEditorWindow->touchTime;
}

void WindowNodeEditor::Start()
{
	ed::Config config;

	config.SettingsFile = "ParticleNodeEditor.json";

	/*config.LoadNodeSettings = [](ed::NodeId nodeId, char* data, void* userPointer) -> size_t
	{

        Node* FindNode(ed::NodeId id);

		auto node = FindNode(nodeId);
		if (!node)
			return 0;

		if (data != nullptr)
			memcpy(data, node->State.data(), node->State.size());
		return node->State.size();
	};

	config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
	{
        Node* FindNode(ed::NodeId id);

		auto node = FindNode(nodeId);
		if (!node)
			return false;

		node->State.assign(data, size);

		TouchNode(nodeId);

		return true;
	};*/

	contextEditor = ed::CreateEditor(&config);
    ed::SetCurrentEditor(contextEditor);

	ed::NavigateToContent();

	BuildNodes();
    
    headerBackground = (ImTextureID)App->resources->GetTexture("pizza");
   
}

void WindowNodeEditor::Draw()
{
    if (!isActive) return;

    UpdateTouch();

    auto& io = ImGui::GetIO();

    ed::SetCurrentEditor(contextEditor);

    auto& style = ImGui::GetStyle();

# if 0
    {
        for (auto x = -io.DisplaySize.y; x < io.DisplaySize.x; x += 10.0f)
        {
            ImGui::GetWindowDrawList()->AddLine(ImVec2(x, 0), ImVec2(x + io.DisplaySize.y, io.DisplaySize.y),
                IM_COL32(255, 255, 0, 255));
        }
    }
# endif


    ImGui::Begin("Node Editor", &isActive);


    ed::NodeId contextNodeId = 0;
    ed::LinkId contextLinkId = 0;
    ed::PinId  contextPinId = 0;
    createNewNode = false;
    newNodeLinkPin = nullptr;
    newLinkPin = nullptr;

     leftPaneWidth = 400.0f;
     rightPaneWidth = 800.0f;


    Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f,-1);

    ShowLeftPane(leftPaneWidth - 4.0f);

    ImGui::SameLine(0.0f, 12.0f);

    ed::Begin("Node editor");
    {
        auto cursorTopLeft = ImGui::GetCursorScreenPos();

        NodeBuilder builder(headerBackground, ImGui_GetTextureWidth(headerBackground), ImGui_GetTextureHeight(headerBackground));

        for (Node* node : nodes)
        {
           
            builder.Begin(node->ID);
           
            node->Draw(builder, this);
                      
            builder.End();

            //Comment Node draw goes Here
        }

        //Links
        for (auto& link : links)
            ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);
        //
        if (!createNewNode)
        {
            if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
            {
                auto showLabel = [](const char* label, ImColor color)
                {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                    auto size = ImGui::CalcTextSize(label);

                    auto padding = ImGui::GetStyle().FramePadding;
                    auto spacing = ImGui::GetStyle().ItemSpacing;

                    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                    auto rectMin = ImGui::GetCursorScreenPos() - padding;
                    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                    auto drawList = ImGui::GetWindowDrawList();
                    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                    ImGui::TextUnformatted(label);
                };
                ed::PinId startPinId = 0, endPinId = 0;
                if (ed::QueryNewLink(&startPinId, &endPinId))
                {
                    Pin* startPin = FindPin(startPinId);
                    Pin* endPin = FindPin(endPinId);

                    newLinkPin = startPin ? startPin : endPin;

                    if (startPin->Kind == PinKind::Input)
                    {
                        std::swap(startPin, endPin);
                        std::swap(startPinId, endPinId);
                    }

                    if (startPin && endPin)
                    {
                        if (endPin == startPin)
                        {
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->Kind == startPin->Kind)
                        {
                            showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->Type != startPin->Type)
                        {
                            showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                        }
                        else
                        {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                            {

                                //TODO: HERE GOES INFORMATION LOGIC!!!!!!!!!!!
                                //Get Pin Type 
                                //Get the pin pointer (needs casting with the type)
                                //Pass the proper information

                                UpdateNodeLinks(startPin, endPin);

                                links.emplace_back(Link(GetNextId(), startPinId, endPinId));
                                links.back().Color = GetIconColor(startPin->Type);
                            }
                        }
                    }
                }

                ed::PinId pinId = 0;
                if (ed::QueryNewNode(&pinId))
                {
                    newLinkPin = FindPin(pinId);
                    if (newLinkPin)
                        showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                    if (ed::AcceptNewItem())
                    {
                        createNewNode = true;
                        newNodeLinkPin = FindPin(pinId);
                        newLinkPin = nullptr;
                        ed::Suspend();
                        ImGui::OpenPopup("Create New Node");
                        ed::Resume();
                    }
                }


                //CreateNewLink(newLinkPin, newNodeLinkPin, createNewNode);
            }
            else
            {
                newLinkPin = nullptr;
            }

            ed::EndCreate();

            if (ed::BeginDelete())
            {
                DeleteNodeOrLink();
            }
            ed::EndDelete();
        }

        ImGui::SetCursorScreenPos(cursorTopLeft);
    }

# if 1
    auto openPopupPosition = ImGui::GetMousePos();
    ed::Suspend();
    if (ed::ShowNodeContextMenu(&contextNodeId))
        ImGui::OpenPopup("Node Context Menu");
    else if (ed::ShowPinContextMenu(&contextPinId))
        ImGui::OpenPopup("Pin Context Menu");
    else if (ed::ShowLinkContextMenu(&contextLinkId))
        ImGui::OpenPopup("Link Context Menu");
    else if (ed::ShowBackgroundContextMenu())
    {
        ImGui::OpenPopup("Create New Node");
        newNodeLinkPin = nullptr;
    }
    ed::Resume();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Node Context Menu"))
    {
        auto node = FindNode(contextNodeId);

        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();
        if (node)
        {
            ImGui::Text("ID: %p", node->ID.AsPointer());
            ImGui::Text("Inputs: %d", (int)node->Inputs.size());
            ImGui::Text("Outputs: %d", (int)node->Outputs.size());
        }
        else
            ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteNode(contextNodeId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Pin Context Menu"))
    {
        auto pin = FindPin(contextPinId);

        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();
        if (pin)
        {
            ImGui::Text("ID: %p", pin->ID.AsPointer());
            if (pin->Node)
                ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
            else
                ImGui::Text("Node: %s", "<none>");
        }
        else
            ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Link Context Menu"))
    {
        auto link = FindLink(contextLinkId);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();
        if (link)
        {
            ImGui::Text("ID: %p", link->ID.AsPointer());
            ImGui::Text("From: %p", link->StartPinID.AsPointer());
            ImGui::Text("To: %p", link->EndPinID.AsPointer());
        }
        else
            ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteLink(contextLinkId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Create New Node"))
    {
        auto newNodePostion = ImGui::GetMousePos();;
        //ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

        //auto drawList = ImGui::GetWindowDrawList();
        //drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

        Node* node = nullptr;

        RightClickMenu(node);

        if (node)
        {
            BuildNodes();

            createNewNode = false;

            ed::SetNodePosition(node->ID, newNodePostion);

            if (auto startPin = newNodeLinkPin)
            {
                auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

                for (auto& pin : pins)
                {
                    if (CanCreateLink(startPin, pin))
                    {
                        auto endPin = pin;
                        if (startPin->Kind == PinKind::Input)
                            std::swap(startPin, endPin);

                        links.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
                        links.back().Color = GetIconColor(startPin->Type);

                        break;
                    }
                }
            }

            //ed::NavigateToContent();
        }


        ImGui::EndPopup();
    }
    else
        createNewNode = false;

    ImGui::PopStyleVar();
    ed::Resume();
# endif

    /*
        cubic_bezier_t c;
        c.p0 = pointf(100, 600);
        c.p1 = pointf(300, 1200);
        c.p2 = pointf(500, 100);
        c.p3 = pointf(900, 600);

        auto drawList = ImGui::GetWindowDrawList();
        auto offset_radius = 15.0f;
        auto acceptPoint = [drawList, offset_radius](const bezier_subdivide_result_t& r)
        {
            drawList->AddCircle(to_imvec(r.point), 4.0f, IM_COL32(255, 0, 255, 255));

            auto nt = r.tangent.normalized();
            nt = pointf(-nt.y, nt.x);

            drawList->AddLine(to_imvec(r.point), to_imvec(r.point + nt * offset_radius), IM_COL32(255, 0, 0, 255), 1.0f);
        };

        drawList->AddBezierCurve(to_imvec(c.p0), to_imvec(c.p1), to_imvec(c.p2), to_imvec(c.p3), IM_COL32(255, 255, 255, 255), 1.0f);
        cubic_bezier_subdivide(acceptPoint, c);
    */

    ed::End();

    ImGui::End();
    //ImGui::ShowTestWindow();
    //ImGui::ShowMetricsWindow();
}

void WindowNodeEditor::CleanUp()
{
    /*if (&headerBackground)
    {
        ImGui_DestroyTexture(&headerBackground);
        headerBackground = nullptr;
    }*/

	if (contextEditor)
	{
		ed::DestroyEditor(contextEditor);
		contextEditor = nullptr;
	}
}

void WindowNodeEditor::UpdateTouch()
{
	const float deltaTime = ImGui::GetIO().DeltaTime;
	for (auto& entry : nodeTouchTime)
	{
		if (entry.second > 0.0f) entry.second -= deltaTime;
	}
}

float WindowNodeEditor::GetTouchProgress(ed::NodeId id)
{
    auto it = nodeTouchTime.find(id);
    if (it != nodeTouchTime.end() && it->second > 0.0f)
        return (touchTime - it->second) / touchTime;
    else
        return 0.0f;
}

Node* WindowNodeEditor::FindNode(ed::NodeId id)
{
    for (Node* node : App->editor->nodeEditorWindow->nodes)
    {
        if (node->ID == id)
            return node;
    }

    return nullptr;
}

Link* WindowNodeEditor::FindLink(ed::LinkId id)
{
	for (Link& link : links)
	{
		if (link.ID == id)
			return &link;
	}
		
	return nullptr;
}

Pin* WindowNodeEditor::FindPin(ed::PinId id)
{
	if (!id)
		return nullptr;

	for (Node* node : nodes)
	{
		for (auto pin : node->Inputs)
		{
			if (pin->ID == id)
				return pin;
		}
			
		for (auto pin : node->Outputs)
		{
			if (pin->ID == id)
				return pin;
		}
			
	}

	return nullptr;
}

bool WindowNodeEditor::IsPinLinked(ed::PinId id)
{
	if (!id)
		return false;

	for (Link& link : links)
	{
		if (link.StartPinID == id || link.EndPinID == id)
			return true;
	}
		
	return false;
}

Pin* WindowNodeEditor::GetPinLinkedTo(ed::PinId id)
{
    if (!id)
        return false;

    for (Link& link : links)
    {
        if (link.StartPinID == id )
            return FindPin(link.EndPinID);

        else if (link.EndPinID == id)
            return FindPin(link.StartPinID);
    }

    return nullptr;
}

bool WindowNodeEditor::CanCreateLink(Pin* a, Pin* b)
{
	if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
		return false;
	else
		return true;
}

void WindowNodeEditor::BuildNode(Node* node)
{
	for (Pin* input : node->Inputs)
	{
		input->Node = node;
		input->Kind = PinKind::Input;
	}

	for (Pin* output : node->Outputs)
	{
		output->Node = node;
		output->Kind = PinKind::Output;
	}
}

void WindowNodeEditor::BuildNodes()
{
	for (Node* node : nodes)
		BuildNode(node);
}


ImColor WindowNodeEditor::GetIconColor(PinType type)
{
	/*switch (type)
	{
	default:
	case PinType::Flow:     return ImColor(255, 255, 255);
	case PinType::Bool:     return ImColor(220, 48, 48);
	case PinType::Int:      return ImColor(68, 201, 156);
	case PinType::Float:    return ImColor(147, 226, 74);
	case PinType::String:   return ImColor(124, 21, 153);
	case PinType::Object:   return ImColor(51, 150, 215);
	case PinType::Function: return ImColor(218, 0, 183);
    case PinType::Vector3: return ImColor(218, 0, 183);
	case PinType::Delegate: return ImColor(255, 48, 48);
	}*/

    //Full Red Test

    switch (type)
    {
    default:
    case PinType::Flow:     return ImColor(255, 255, 255);
    case PinType::Bool:     return ImColor(220, 80, 80);
    case PinType::Int:      return ImColor(220, 80, 80);
    case PinType::Float:    return ImColor(220, 80, 80);
    case PinType::String:   return ImColor(220, 80, 80);
    case PinType::Float3: return ImColor(220, 80, 80);
    case PinType::Float4:   return ImColor(220, 80, 80);
    case PinType::Float4Array:   return ImColor(220, 80, 80);
    case PinType::Texture:   return ImColor(220, 80, 80);
    case PinType::GameObject: return ImColor(220, 80, 80);
    case PinType::Delegate: return ImColor(255, 48, 48);
    }
}

void WindowNodeEditor::DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32 innerColor)
{
    auto rect = ImRect(a, b);
    auto rect_x = rect.Min.x;
    auto rect_y = rect.Min.y;
    auto rect_w = rect.Max.x - rect.Min.x;
    auto rect_h = rect.Max.y - rect.Min.y;
    auto rect_center_x = (rect.Min.x + rect.Max.x) * 0.5f;
    auto rect_center_y = (rect.Min.y + rect.Max.y) * 0.5f;
    auto rect_center = ImVec2(rect_center_x, rect_center_y);
    const auto outline_scale = rect_w / 24.0f;
    const auto extra_segments = static_cast<int>(2 * outline_scale); // for full circle

    if (type == IconType::Flow)
    {
        const auto origin_scale = rect_w / 24.0f;

        const auto offset_x = 1.0f * origin_scale;
        const auto offset_y = 0.0f * origin_scale;
        const auto margin = (filled ? 2.0f : 2.0f) * origin_scale;
        const auto rounding = 0.1f * origin_scale;
        const auto tip_round = 0.7f; // percentage of triangle edge (for tip)
        //const auto edge_round = 0.7f; // percentage of triangle edge (for corner)
        const auto canvas = ImRect(
            rect.Min.x + margin + offset_x,
            rect.Min.y + margin + offset_y,
            rect.Max.x - margin + offset_x,
            rect.Max.y - margin + offset_y);
        const auto canvas_x = canvas.Min.x;
        const auto canvas_y = canvas.Min.y;
        const auto canvas_w = canvas.Max.x - canvas.Min.x;
        const auto canvas_h = canvas.Max.y - canvas.Min.y;

        const auto left = canvas_x + canvas_w * 0.5f * 0.3f;
        const auto right = canvas_x + canvas_w - canvas_w * 0.5f * 0.3f;
        const auto top = canvas_y + canvas_h * 0.5f * 0.2f;
        const auto bottom = canvas_y + canvas_h - canvas_h * 0.5f * 0.2f;
        const auto center_y = (top + bottom) * 0.5f;
        //const auto angle = AX_PI * 0.5f * 0.5f * 0.5f;

        const auto tip_top = ImVec2(canvas_x + canvas_w * 0.5f, top);
        const auto tip_right = ImVec2(right, center_y);
        const auto tip_bottom = ImVec2(canvas_x + canvas_w * 0.5f, bottom);

        drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
        drawList->PathBezierCurveTo(
            ImVec2(left, top),
            ImVec2(left, top),
            ImVec2(left, top) + ImVec2(rounding, 0));
        drawList->PathLineTo(tip_top);
        drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
        drawList->PathBezierCurveTo(
            tip_right,
            tip_right,
            tip_bottom + (tip_right - tip_bottom) * tip_round);
        drawList->PathLineTo(tip_bottom);
        drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
        drawList->PathBezierCurveTo(
            ImVec2(left, bottom),
            ImVec2(left, bottom),
            ImVec2(left, bottom) - ImVec2(0, rounding));

        if (!filled)
        {
            if (innerColor & 0xFF000000)
                drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

            drawList->PathStroke(color, true, 2.0f * outline_scale);
        }
        else
            drawList->PathFillConvex(color);
    }
    else
    {
        auto triangleStart = rect_center_x + 0.32f * rect_w;

        auto rect_offset = -static_cast<int>(rect_w * 0.25f * 0.25f);

        rect.Min.x += rect_offset;
        rect.Max.x += rect_offset;
        rect_x += rect_offset;
        rect_center_x += rect_offset * 0.5f;
        rect_center.x += rect_offset * 0.5f;

        if (type == IconType::Circle)
        {
            const auto c = rect_center;

            if (!filled)
            {
                const auto r = 0.5f * rect_w / 2.0f - 0.5f;

                if (innerColor & 0xFF000000)
                    drawList->AddCircleFilled(c, r, innerColor, 12 + extra_segments);
                drawList->AddCircle(c, r, color, 12 + extra_segments, 2.0f * outline_scale);
            }
            else
                drawList->AddCircleFilled(c, 0.5f * rect_w / 2.0f, color, 12 + extra_segments);
        }

        if (type == IconType::Square)
        {
            if (filled)
            {
                const auto r = 0.5f * rect_w / 2.0f;
                const auto p0 = rect_center - ImVec2(r, r);
                const auto p1 = rect_center + ImVec2(r, r);

                drawList->AddRectFilled(p0, p1, color, 0, 15 + extra_segments);
            }
            else
            {
                const auto r = 0.5f * rect_w / 2.0f - 0.5f;
                const auto p0 = rect_center - ImVec2(r, r);
                const auto p1 = rect_center + ImVec2(r, r);

                if (innerColor & 0xFF000000)
                    drawList->AddRectFilled(p0, p1, innerColor, 0, 15 + extra_segments);

                drawList->AddRect(p0, p1, color, 0, 15 + extra_segments, 2.0f * outline_scale);
            }
        }

        if (type == IconType::Grid)
        {
            const auto r = 0.5f * rect_w / 2.0f;
            const auto w = ceilf(r / 3.0f);

            const auto baseTl = ImVec2(floorf(rect_center_x - w * 2.5f), floorf(rect_center_y - w * 2.5f));
            const auto baseBr = ImVec2(floorf(baseTl.x + w), floorf(baseTl.y + w));

            auto tl = baseTl;
            auto br = baseBr;
            for (int i = 0; i < 3; ++i)
            {
                tl.x = baseTl.x;
                br.x = baseBr.x;
                drawList->AddRectFilled(tl, br, color);
                tl.x += w * 2;
                br.x += w * 2;
                if (i != 1 || filled)
                    drawList->AddRectFilled(tl, br, color);
                tl.x += w * 2;
                br.x += w * 2;
                drawList->AddRectFilled(tl, br, color);

                tl.y += w * 2;
                br.y += w * 2;
            }

            triangleStart = br.x + w + 1.0f / 24.0f * rect_w;
        }

        if (type == IconType::RoundSquare)
        {
            if (filled)
            {
                const auto r = 0.5f * rect_w / 2.0f;
                const auto cr = r * 0.5f;
                const auto p0 = rect_center - ImVec2(r, r);
                const auto p1 = rect_center + ImVec2(r, r);

                drawList->AddRectFilled(p0, p1, color, cr, 15);
            }
            else
            {
                const auto r = 0.5f * rect_w / 2.0f - 0.5f;
                const auto cr = r * 0.5f;
                const auto p0 = rect_center - ImVec2(r, r);
                const auto p1 = rect_center + ImVec2(r, r);

                if (innerColor & 0xFF000000)
                    drawList->AddRectFilled(p0, p1, innerColor, cr, 15);

                drawList->AddRect(p0, p1, color, cr, 15, 2.0f * outline_scale);
            }
        }
        else if (type == IconType::Diamond)
        {
            if (filled)
            {
                const auto r = 0.607f * rect_w / 2.0f;
                const auto c = rect_center;

                drawList->PathLineTo(c + ImVec2(0, -r));
                drawList->PathLineTo(c + ImVec2(r, 0));
                drawList->PathLineTo(c + ImVec2(0, r));
                drawList->PathLineTo(c + ImVec2(-r, 0));
                drawList->PathFillConvex(color);
            }
            else
            {
                const auto r = 0.607f * rect_w / 2.0f - 0.5f;
                const auto c = rect_center;

                drawList->PathLineTo(c + ImVec2(0, -r));
                drawList->PathLineTo(c + ImVec2(r, 0));
                drawList->PathLineTo(c + ImVec2(0, r));
                drawList->PathLineTo(c + ImVec2(-r, 0));

                if (innerColor & 0xFF000000)
                    drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

                drawList->PathStroke(color, true, 2.0f * outline_scale);
            }
        }
        else
        {
            const auto triangleTip = triangleStart + rect_w * (0.45f - 0.32f);

            drawList->AddTriangleFilled(
                ImVec2(ceilf(triangleTip), rect_y + rect_h * 0.5f),
                ImVec2(triangleStart, rect_center_y + 0.15f * rect_h),
                ImVec2(triangleStart, rect_center_y - 0.15f * rect_h),
                color);
        }
    }

}

void WindowNodeEditor::DrawPinIcon(const Pin* pin, bool connected, int alpha)
{
	IconType iconType;
    ImVec2 size = ImVec2(pinIconSize, pinIconSize);
	ImColor  color = GetIconColor(pin->Type);
	color.Value.w = alpha / 255.0f;
	switch (pin->Type)
	{
	case PinType::Flow:     iconType = IconType::Flow;   break;
	case PinType::Bool:     iconType = IconType::Circle; break;
	case PinType::Int:      iconType = IconType::Circle; break;
	case PinType::Float:    iconType = IconType::Circle; break;
	case PinType::String:   iconType = IconType::Circle; break;
    case PinType::Float3: iconType = IconType::Circle; break;
    case PinType::Float4: iconType = IconType::Circle; break;
    case PinType::Float4Array: iconType = IconType::Circle; break;
    case PinType::Texture:   iconType = IconType::RoundSquare; break;
    case PinType::GameObject:   iconType = IconType::Diamond; break;
	case PinType::Delegate: iconType = IconType::Square; break;
	default:
		return;
	}

    if (ImGui::IsRectVisible(size))
    {
        auto cursorPos = ImGui::GetCursorScreenPos();
        auto drawList = ImGui::GetWindowDrawList();
        DrawIcon(drawList, cursorPos, cursorPos + size, iconType, connected, ImColor(color), ImColor(32, 32, 32, alpha));
    }

    ImGui::Dummy(size);
}

bool WindowNodeEditor::Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

void WindowNodeEditor::ShowLeftPane(float paneWidth)
{
   auto& io = ImGui::GetIO();

    ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

    paneWidth = ImGui::GetContentRegionAvailWidth();

    static bool showStyleEditor = false;
    ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
    ImGui::Spring(0.0f, 0.0f);
    if (ImGui::Button("Zoom to Content"))
        ed::NavigateToContent();
    ImGui::Spring(0.0f);
    if (ImGui::Button("Show Flow"))
    {
        for (auto& link : links)
            ed::Flow(link.ID);
    }
    ImGui::Spring();
    if (ImGui::Button("Edit Style"))
        showStyleEditor = true;
    ImGui::EndHorizontal();

    if (showStyleEditor)
        ShowStyleEditor(&showStyleEditor);

    std::vector<ed::NodeId> selectedNodes;
    std::vector<ed::LinkId> selectedLinks;
    selectedNodes.resize(ed::GetSelectedObjectCount());
    selectedLinks.resize(ed::GetSelectedObjectCount());

    int nodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
    int linkCount = ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

    selectedNodes.resize(nodeCount);
    selectedLinks.resize(linkCount);


    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing(); ImGui::SameLine();
    ImGui::TextUnformatted("Nodes");
    ImGui::Indent();
    for (Node* node : nodes)
    {
        ImGui::PushID(node->ID.AsPointer());
        auto start = ImGui::GetCursorScreenPos();

        if (const auto progress = GetTouchProgress(node->ID))
        {
            ImGui::GetWindowDrawList()->AddLine(
                start + ImVec2(-8, 0),
                start + ImVec2(-8, ImGui::GetTextLineHeight()),
                IM_COL32(255, 0, 0, 255 - (int)(255 * progress)), 4.0f);
        }

        bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(), node->ID) != selectedNodes.end();
        if (ImGui::Selectable((node->Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(node->ID.AsPointer()))).c_str(), &isSelected))
        {
            if (io.KeyCtrl)
            {
                if (isSelected)
                    ed::SelectNode(node->ID, true);
                else
                    ed::DeselectNode(node->ID);
            }
            else
                ed::SelectNode(node->ID, false);

            ed::NavigateToSelection();
        }
        if (ImGui::IsItemHovered() && !node->State.empty())
            ImGui::SetTooltip("State: %s", node->State.c_str());

        auto id = std::string("(") + std::to_string(reinterpret_cast<uintptr_t>(node->ID.AsPointer())) + ")";
        auto textSize = ImGui::CalcTextSize(id.c_str(), nullptr);
        /*auto iconPanelPos = start + ImVec2(
            paneWidth - ImGui::GetStyle().FramePadding.x - ImGui::GetStyle().IndentSpacing - saveIconWidth - restoreIconWidth - ImGui::GetStyle().ItemInnerSpacing.x * 1,
            (ImGui::GetTextLineHeight() - saveIconHeight) / 2);*/
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(textSize.x - ImGui::GetStyle().ItemInnerSpacing.x, start.y),
            IM_COL32(255, 255, 255, 255), id.c_str(), nullptr);

        auto drawList = ImGui::GetWindowDrawList();
       
        ImGui::PopID();
    }
    ImGui::Unindent();

    static int changeCount = 0;

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing(); ImGui::SameLine();
    ImGui::TextUnformatted("Selection");

    ImGui::BeginHorizontal("Selection Stats", ImVec2(paneWidth, 0));
    ImGui::Text("Changed %d time%s", changeCount, changeCount > 1 ? "s" : "");
    ImGui::Spring();
    if (ImGui::Button("Deselect All"))
        ed::ClearSelection();
    ImGui::EndHorizontal();
    ImGui::Indent();
    for (int i = 0; i < nodeCount; ++i) ImGui::Text("Node (%p)", selectedNodes[i].AsPointer());
    for (int i = 0; i < linkCount; ++i) ImGui::Text("Link (%p)", selectedLinks[i].AsPointer());
    ImGui::Unindent();

    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
        for (auto& link : links)
            ed::Flow(link.ID);

    if (ed::HasSelectionChanged())
        ++changeCount;

    ImGui::EndChild();
}


void WindowNodeEditor::ShowStyleEditor(bool* show = nullptr)
{
    if (!ImGui::Begin("Node Editor Style", show))
    {
        ImGui::End();
        return;
    }

    auto paneWidth = ImGui::GetContentRegionAvailWidth();

    auto& editorStyle = ed::GetStyle();

    if (ImGui::Button("Reset values to defaults"))
        editorStyle = ed::Style();

    ImGui::Separator();
    ImGui::DragFloat4("Node Padding", &editorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Node Rounding", &editorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Node Border Width", &editorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Hovered Node Border Width", &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Selected Node Border Width", &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Pin Rounding", &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Pin Border Width", &editorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Link Strength", &editorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
    ImGui::DragFloat("Scroll Duration", &editorStyle.ScrollDuration, 0.001f, 0.0f, 2.0f);
    ImGui::DragFloat("Flow Marker Distance", &editorStyle.FlowMarkerDistance, 1.0f, 1.0f, 200.0f);
    ImGui::DragFloat("Flow Speed", &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
    ImGui::DragFloat("Flow Duration", &editorStyle.FlowDuration, 0.001f, 0.0f, 5.0f);
    ImGui::DragFloat("Group Rounding", &editorStyle.GroupRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Group Border Width", &editorStyle.GroupBorderWidth, 0.1f, 0.0f, 15.0f);

    ImGui::Separator();

    static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_RGB;
    ImGui::TextUnformatted("Filter Colors");
    ImGui::Spacing();
    ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_RGB);
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_HSV);
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_HEX);

    static ImGuiTextFilter filter;
    filter.Draw("", paneWidth);

    ImGui::Spacing();

    ImGui::PushItemWidth(-160);
    for (int i = 0; i < ed::StyleColor_Count; ++i)
    {
        auto name = ed::GetStyleColorName((ed::StyleColor)i);
        if (!filter.PassFilter(name))
            continue;

        ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

ImRect WindowNodeEditor::ImRect_Expanded(const ImRect& rect, float x, float y)
{
    auto result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}

void WindowNodeEditor::CreateNewLink(Pin* newLinkPin,Pin* newNodeLinkPin, bool& createNewNode)
{
   
   
    //ed::PinId startPinId = 0, endPinId = 0;
    //if (ed::QueryNewLink(&startPinId, &endPinId))
    //{
    //    Pin* startPin = FindPin(startPinId);
    //    Pin* endPin = FindPin(endPinId);

    //    newLinkPin = startPin ? startPin : endPin;

    //    if (startPin->Kind == PinKind::Input)
    //    {
    //        std::swap(startPin, endPin);
    //        std::swap(startPinId, endPinId);
    //    }

    //    if (startPin && endPin)
    //    {
    //        if (endPin == startPin)
    //        {
    //            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
    //        }
    //        else if (endPin->Kind == startPin->Kind)
    //        {
    //            showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
    //            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
    //        }
    //        else if (endPin->Type != startPin->Type)
    //        {
    //            showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
    //            ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
    //        }
    //        else
    //        {
    //            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
    //            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
    //            {

    //                //TODO: HERE GOES INFORMATION LOGIC!!!!!!!!!!!
    //                //Get Pin Type 
    //                //Get the pin pointer (needs casting with the type)
    //                //Pass the proper information

    //                UpdateNodeLinks(startPin, endPin);

    //                links.emplace_back(Link(GetNextId(), startPinId, endPinId));
    //                links.back().Color = GetIconColor(startPin->Type);
    //            }
    //        }
    //    }
    //}

    //ed::PinId pinId = 0;
    //if (ed::QueryNewNode(&pinId))
    //{
    //    newLinkPin = FindPin(pinId);
    //    if (newLinkPin)
    //        showLabel("+ Create Node", ImColor(32, 45, 32, 180));

    //    if (ed::AcceptNewItem())
    //    {
    //        createNewNode = true;
    //        newNodeLinkPin = FindPin(pinId);
    //        newLinkPin = nullptr;
    //        ed::Suspend();
    //        ImGui::OpenPopup("Create New Node");
    //        ed::Resume();
    //    }
    //}



}

void WindowNodeEditor::DeleteNodeOrLink()
{
    ed::LinkId linkId = 0;
    while (ed::QueryDeletedLink(&linkId))
    {
        if (ed::AcceptDeletedItem())
        {
            auto id = std::find_if(links.begin(), links.end(), [linkId](auto& link) { return link.ID == linkId; });
            if (id != links.end())
                links.erase(id);
        }
    }

    ed::NodeId nodeId = 0;
    while (ed::QueryDeletedNode(&nodeId))
    {
        if (ed::AcceptDeletedItem())
        {
            auto id = std::find_if(nodes.begin(), nodes.end(), [nodeId](Node* node) { return node->ID == nodeId; });
            if (id != nodes.end())
            {
                //(*id)->~Node();
                delete* id;
                nodes.erase(id);
            }

        }
    }
}

Node* WindowNodeEditor::CreateParticleSystem()
{
    Node* emitterNode = CreateNode(NodeType::Emitter); ed::SetNodePosition(emitterNode->ID, ImVec2(700, 300));

    Node* node = CreateNode(NodeType::Float,"SpawnRate"); ed::SetNodePosition(node->ID, ImVec2(0, 0));
    ((PinFloat*)node->GetOutputPinByName("Float"))->pinFloat = 0.3f;
    node->updateLinks = true;
    links.push_back(Link(GetNextLinkId(), node->Outputs[0]->ID, emitterNode->Inputs[0]->ID));
     
    node = CreateNode(NodeType::Float, "Lifetime"); ed::SetNodePosition(node->ID, ImVec2(0, 75));
    ((PinFloat*)node->GetOutputPinByName("Float"))->pinFloat = 7.0f;
    node->updateLinks = true;
    links.push_back(Link(GetNextLinkId(), node->Outputs[0]->ID, emitterNode->Inputs[1]->ID));

    node = CreateNode(NodeType::Float,"Size"); ed::SetNodePosition(node->ID, ImVec2(0, 150));
    ((PinFloat*)node->GetOutputPinByName("Float"))->pinFloat = 1.5f;
    node->updateLinks = true;
    links.push_back(Link(GetNextLinkId(), node->Outputs[0]->ID, emitterNode->Inputs[2]->ID));

    node = CreateNode(NodeType::Velocity); ed::SetNodePosition(node->ID, ImVec2(-150, 225));
    links.push_back(Link(GetNextLinkId(), node->Outputs[0]->ID, emitterNode->Inputs[3]->ID));

    node = CreateNode(NodeType::ColorOverTime); ed::SetNodePosition(node->ID, ImVec2(-150, 325));
    links.push_back(Link(GetNextLinkId(), node->Outputs[0]->ID, emitterNode->Inputs[4]->ID));

    node = CreateNode(NodeType::Alignment); ed::SetNodePosition(node->ID, ImVec2(0, 425));
    links.push_back(Link(GetNextLinkId(), node->Outputs[0]->ID, emitterNode->Inputs[5]->ID));

    node = CreateNode(NodeType::Texture); ed::SetNodePosition(node->ID, ImVec2(0, 625));
    links.push_back(Link(GetNextLinkId(), node->Outputs[0]->ID, emitterNode->Inputs[6]->ID));

    node = CreateNode(NodeType::SpawnArea); ed::SetNodePosition(node->ID, ImVec2(150, 625));
    links.push_back(Link(GetNextLinkId(), node->Outputs[1]->ID, emitterNode->Inputs[7]->ID));

    node = CreateNode(NodeType::Bool, "Active"); ed::SetNodePosition(node->ID, ImVec2(400, 675));
    links.push_back(Link(GetNextLinkId(), node->Outputs[0]->ID, emitterNode->Inputs[8]->ID));


    ed::NavigateToContent();


    return emitterNode;
}

Node* WindowNodeEditor::CreateNode(NodeType type, std::string name)
{
    Node* newNode = nullptr;


    switch (type)
    {
    case NodeType::Comment:newNode = new NodeComment(GetNextId(), "Container", ImColor(50, 50, 50)); break;
    case NodeType::Emitter: newNode = new NodeEmitter(GetNextId(), "Emitter", ImColor(128, 195, 248)); break;
    case NodeType::Bool:  newNode = new NodeBool(GetNextId(), name.c_str(), ImColor(128, 195, 248)); break;
    case NodeType::Float: newNode = new NodeFloat(GetNextId(), name.c_str(), ImColor(128, 195, 248)); break;
    case NodeType::Float3: newNode = new NodeFloat3(GetNextId(), name.c_str(), ImColor(128, 195, 248)); break;
    case NodeType::Color: newNode = new NodeColor(GetNextId(), "Color", ImColor(128, 195, 248)); break;
    case NodeType::ColorOverTime: newNode = new NodeColorOvertime(GetNextId(), "Color OverTime", ImColor(128, 195, 248)); break;
    case NodeType::Velocity: newNode = new NodeVelocity(GetNextId(), "Velocity", ImColor(128, 195, 248)); break;
    case NodeType::Texture: newNode = new NodeTexture(GetNextId(), "Texture", ImColor(128, 195, 248)); break;
    case NodeType::Alignment: newNode = new NodeAlignment(GetNextId(), "Alignment", ImColor(128, 195, 248)); break;
    case NodeType::Gravity: newNode = new NodeGravity(GetNextId(), "Gravity", ImColor(128, 195, 248)); break;
    case NodeType::GravitationalField: newNode = new NodeGravitationalField(GetNextId(), "Gravitational Field", ImColor(128, 195, 248)); break;
    case NodeType::SpawnArea: newNode = new NodeSpawnArea(GetNextId(), "Spawn From Model", ImColor(128, 195, 248)); break;
    default: break;
    }

    if (newNode)
    {
        nodes.emplace_back(newNode);
        BuildNode(newNode);
    }

    return newNode;
}


void WindowNodeEditor::RightClickMenu(Node* node)
{
   
    if (ImGui::BeginMenu("Particle Nodes"))
    {
        
        if (ImGui::MenuItem("Particle Emitter")) node = CreateNode(NodeType::Emitter);

        if (ImGui::MenuItem("Velocity"))  node = CreateNode(NodeType::Velocity);

        if (ImGui::MenuItem("Texture"))  node = CreateNode(NodeType::Texture);

        if (ImGui::MenuItem("Alignment"))  node = CreateNode(NodeType::Alignment);

        if (ImGui::MenuItem("Color"))  node = CreateNode(NodeType::Color);

        if (ImGui::MenuItem("ColorOverTime"))  node = CreateNode(NodeType::ColorOverTime);

        if (ImGui::MenuItem("Gravity"))  node = CreateNode(NodeType::Gravity);

        if (ImGui::MenuItem("Gravitational Field"))  node = CreateNode(NodeType::GravitationalField);

        if (ImGui::MenuItem("Spawn From Model"))  node = CreateNode(NodeType::SpawnArea);

        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Base Nodes"))
    {
        //if (ImGui::MenuItem("Container"))  node = CreateNode(NodeType::Comment);

        if (ImGui::MenuItem("Boolean")) node = CreateNode(NodeType::Bool, "Boolean");
        
        else if (ImGui::MenuItem("Float")) node = CreateNode(NodeType::Float, "Float");

        else if (ImGui::MenuItem("Float3")) node = CreateNode(NodeType::Float3, "Vector3");

        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Focus"))
    {
        ed::NavigateToContent();
       
    }
}

void WindowNodeEditor::UpdateNodeLinks(Pin* startPin, Pin* endPin)
{
    startPin->Node->updateEmitter = true;
    endPin->Node->updateEmitter = true;

    switch (startPin->Type)
    {
    case PinType::Flow:
        endPin->Node->isActive = startPin->Node->isActive;
        ((PinFlow*)endPin)->emitterInstance = ((PinFlow*)startPin)->emitterInstance;
        break;
    case PinType::Bool:
        ((PinBool*)endPin)->pinBool = ((PinBool*)startPin)->pinBool;
        break;
    case PinType::Int:
        ((PinInt*)endPin)->pinInt = ((PinInt*)startPin)->pinInt;
        break;
    case PinType::String:
        break;
    case PinType::Float:
        ((PinFloat*)endPin)->pinFloat = ((PinFloat*)startPin)->pinFloat;
        break;
    case PinType::Float3:
        ((PinFloat3*)endPin)->pinFloat3 = ((PinFloat3*)startPin)->pinFloat3;
        break;
    case PinType::Float4:
        ((PinFloat4*)endPin)->pinFloat4 = ((PinFloat4*)startPin)->pinFloat4;
        break;
    case PinType::Texture:
        ((PinTexture*)endPin)->pinTexture = ((PinTexture*)startPin)->pinTexture;
        break;
    case PinType::Float4Array:
        ((PinFloat4Array*)endPin)->float4Array[0] = ((PinFloat4Array*)startPin)->float4Array[0];
        ((PinFloat4Array*)endPin)->float4Array[1] = ((PinFloat4Array*)startPin)->float4Array[1];
        break;
    case PinType::GameObject:
        ((PinGameObject*)endPin)->gameObject = ((PinGameObject*)startPin)->gameObject;

    default:
        break;
    }
}


