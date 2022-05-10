#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleResource.h"
#include "WindowNodeEditor.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Dependencies/ImGui/imgui_internal.h"
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>

#include "NodeVelocity.h"
#include "NodeFloat3.h"
#include "NodeTexture.h"
#include "NodeAlignment.h"
#include "NodeColor.h"

#include "PinFloat3.h"
#include "PinFloat4.h"
#include "PinString.h"
#include "PinInt.h"
#include "PinFloat.h"
#include "PinBool.h"
#include "PinTexture.h"

#include "Dependencies/MathGeoLib/include/Math/float3.h"

WindowNodeEditor::WindowNodeEditor(bool isActive): Window("Particle System Node Editor", isActive),
    newNode(nullptr)
{
}

WindowNodeEditor::~WindowNodeEditor()
{
    newNode = nullptr;
    delete newNode;
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
            if (node->Type == NodeType::Blueprint || node->Type == NodeType::Simple)
            {
                isSimple = node->Type == NodeType::Simple;

                hasOutputDelegates = false;

                for (auto output : node->Outputs)
                    if (output->Type == PinType::Delegate)
                        hasOutputDelegates = true;

            }        
            builder.Begin(node->ID);
           

            switch (node->Type)
            {
            case NodeType::Simple: DrawSimpleNode(builder, node); break;
            case NodeType::Blueprint: DrawBlueprintNode(builder, node); break;
            case NodeType::Float3: DrawFloat3Node(builder, node); break;
            case NodeType::Velocity: DrawVelocityNode(builder, node); break;
            case NodeType::Texture: DrawTextureNode(builder, node); break;
            case NodeType::Alignment: DrawAlignmentNode(builder, node); break;
            case NodeType::Color: DrawColorNode(builder, node); break;

            }

           
            builder.End();

            switch (node->Type)
            {
            case NodeType::Tree: DrawTreeNode(newLinkPin, node); break;
            case NodeType::Houdini: DrawHoudiniNode(newLinkPin, node); break;
            case NodeType::Comment: DrawCommentNode(node); break;

            }
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
            ImGui::Text("Type: %s", node->Type == NodeType::Blueprint ? "Blueprint" : (node->Type == NodeType::Tree ? "Tree" : "Comment"));
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
    case PinType::Texture:   return ImColor(220, 80, 80);
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
    case PinType::Texture:   iconType = IconType::RoundSquare; break;
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
                nodes.erase(id);
        }
    }
}

void WindowNodeEditor::DrawSimpleNode(NodeBuilder& builder, Node* node)
{
    //builder.Header(node->Color);
    //ImGui::Spring(0);
    //ImGui::TextUnformatted(node->Name.c_str());
    //ImGui::Spring(1);
    //ImGui::Dummy(ImVec2(0, 28));
    //builder.EndHeader();


    SimpleAndBlueprintPins(builder, node);
}


void WindowNodeEditor::DrawVelocityNode(NodeBuilder& builder, Node* node)
{
    //TODO: Specify which node or pin to perma update
    // I shouldn't be updating them every frame
    node->updateLinks = true;


    //Header------------------------------------

    builder.Header(node->Color);
    builder.SetCustomNode();
    ImGui::Spring(0);
    ImGui::TextUnformatted(node->Name.c_str());
    ImGui::Spring(1);
    ImGui::Dummy(ImVec2(0, 28));
    ImGui::Spring(0);
    builder.EndHeader();

    //Inputs------------------------------------

    PinFloat4* tempPinFloat4 = nullptr;
    NodeVelocity* tempVelNode = (NodeVelocity*)node;

    if (!tempVelNode)
        return;

    ImGui::BeginColumns("Columns", 2, ImGuiColumnsFlags_NoBorder);
    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 520);
   
    
    for (Pin* input : node->Inputs)
    {
        tempPinFloat4 = ((PinFloat4*)input);

        if (!tempPinFloat4)
            continue;

        auto alpha = ImGui::GetStyle().Alpha;
        if (newLinkPin && !CanCreateLink(newLinkPin, input) && input != newLinkPin)
            alpha = alpha * (48.0f / 255.0f);

        ed::BeginPin(input->ID, ed::PinKind::Input);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

        if (!input->Name.empty())
        {
            ImGui::TextUnformatted(input->Name.c_str());
            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();

        }
            
        ImGui::PushItemWidth(80);

        if (tempPinFloat4->Name == "Velocity1")
        {
            if (ImGui::DragFloat("X1", &tempPinFloat4->pinFloat4.x, 0.1f))
            {
                tempVelNode->initialVelocity1.x = tempPinFloat4->pinFloat4.x;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Y1", &tempPinFloat4->pinFloat4.y, 0.1f))
            {
                tempVelNode->initialVelocity1.y = tempPinFloat4->pinFloat4.y;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Z1", &tempPinFloat4->pinFloat4.z, 0.1f))
            {
                tempVelNode->initialVelocity1.z = tempPinFloat4->pinFloat4.z;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Speed1", &tempPinFloat4->pinFloat4.w, 0.1f))
            {
                tempVelNode->initialVelocity1.w = tempPinFloat4->pinFloat4.w;
            }
        }
        else if (tempPinFloat4->Name == "Velocity2")
        {
            if (ImGui::DragFloat("X2", &tempPinFloat4->pinFloat4.x, 0.1f))
            {
                tempVelNode->initialVelocity2.x = tempPinFloat4->pinFloat4.x;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Y2", &tempPinFloat4->pinFloat4.y, 0.1f))
            {
                tempVelNode->initialVelocity2.y = tempPinFloat4->pinFloat4.y;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Z2", &tempPinFloat4->pinFloat4.z, 0.1f))
            {
                tempVelNode->initialVelocity2.z = tempPinFloat4->pinFloat4.z;
            }
            ImGui::SameLine();
            if (ImGui::DragFloat("Speed2", &tempPinFloat4->pinFloat4.w, 0.1f))
            {
                tempVelNode->initialVelocity2.w = tempPinFloat4->pinFloat4.w;
            }
        }

        ImGui::PopItemWidth();

        ImGui::SameLine();

        ImGui::PopStyleVar();
        
        ed::EndPin();
    }
    

    //Output--------------------------------

    ImGui::NextColumn();

    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 40);

    ImGui::Dummy(ImVec2(0,10));

    tempPinFloat4 = (PinFloat4*)tempVelNode->Outputs[0];

    if (!tempPinFloat4)
        return;

    auto alpha = ImGui::GetStyle().Alpha;
    if (newLinkPin && !CanCreateLink(newLinkPin, tempPinFloat4) && tempPinFloat4 != newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

    

    static bool wasActive = false;


    if (node->updateLinks)
    {
        if (tempPinFloat4)
        {
            tempPinFloat4->pinFloat4 = tempVelNode->ComputeVelocity();
        }

        node->updateLinks = false;

        Pin* linkedPin = GetPinLinkedTo(tempPinFloat4->ID);

        if (linkedPin && tempPinFloat4)
            UpdateNodeLinks(tempPinFloat4, linkedPin);
    }
    ed::BeginPin(tempPinFloat4->ID, ed::PinKind::Input);

    DrawPinIcon(tempPinFloat4, IsPinLinked(tempPinFloat4->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    ImGui::EndColumns();
}

void WindowNodeEditor::DrawFloat3Node(NodeBuilder& builder, Node* node)
{

    //Header------------------------------------

    builder.Header(node->Color);
    builder.SetCustomNode();
    ImGui::Spring(0);
    ImGui::TextUnformatted(node->Name.c_str());
    ImGui::Spring(1);
    ImGui::Dummy(ImVec2(0, 28));
    ImGui::Spring(0);
    builder.EndHeader();

    //Output------------------------------------

    PinFloat3* tempPinFloat3 = (PinFloat3*)node->Outputs[0];

    if (!tempPinFloat3) return;

    auto alpha = ImGui::GetStyle().Alpha;
    if (newLinkPin && !CanCreateLink(newLinkPin, tempPinFloat3) && tempPinFloat3 != newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

    


    ImGui::PushItemWidth(80);

    if (ImGui::DragFloat("X", &tempPinFloat3->pinFloat3.x, 0.1f)) node->updateLinks = true;

    ImGui::SameLine();

    if (ImGui::DragFloat("Y", &tempPinFloat3->pinFloat3.y, 0.1f)) node->updateLinks = true;

    ImGui::SameLine();

    if (ImGui::DragFloat("Z", &tempPinFloat3->pinFloat3.z, 0.1f)) node->updateLinks = true;


    ImGui::PopItemWidth();

    ImGui::SameLine();

    ed::BeginPin(tempPinFloat3->ID, ed::PinKind::Output);

    DrawPinIcon(tempPinFloat3, IsPinLinked(tempPinFloat3->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    if (node->updateLinks)
    {
        node->updateLinks = false;

        Pin* linkedPin = GetPinLinkedTo(tempPinFloat3->ID);

        if (linkedPin && tempPinFloat3)
            UpdateNodeLinks(tempPinFloat3, linkedPin);
    }


}

void WindowNodeEditor::DrawTextureNode(NodeBuilder& builder, Node* node)
{

    //Header------------------------------------

    builder.Header(node->Color);
    builder.SetCustomNode();
    ImGui::Spring(0);
    ImGui::TextUnformatted(node->Name.c_str());
    ImGui::Spring(1);
    ImGui::Dummy(ImVec2(0, 28));
    ImGui::Spring(0);
    builder.EndHeader();

    //Output------------------------------------

    NodeTexture* tempTexNode = (NodeTexture*)node;
    PinTexture* tempPinTex = (PinTexture*)node->Outputs[0];

    if (!tempPinTex) return;

    auto alpha = ImGui::GetStyle().Alpha;
    if (newLinkPin && !CanCreateLink(newLinkPin, tempPinTex) && tempPinTex != newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);


    if (!tempPinTex->pinTexture)
    {
        tempPinTex->pinTexture = tempTexNode->GetDefaultParticleTex();
    }
    
    //TODO: Choose texture

    ImGui::Image((ImTextureID)tempPinTex->pinTexture->id, ImVec2(80, 80), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset"))
        {
            uint32 UID = *(const uint32*)payload->Data;
            Resource* resource = App->resources->GetResourceInMemory(UID);
            if (resource->type == ResourceType::Texture)
            {
                tempPinTex->pinTexture = (ResourceTexture*)App->resources->LoadResource(UID);
                node->updateLinks = true;
            }
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();

    ed::BeginPin(tempPinTex->ID, ed::PinKind::Output);

    DrawPinIcon(tempPinTex, IsPinLinked(tempPinTex->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    if (node->updateLinks)
    {
        node->updateLinks = false;

        Pin* linkedPin = GetPinLinkedTo(tempPinTex->ID);

        if (linkedPin && tempPinTex) UpdateNodeLinks(tempPinTex, linkedPin);
    }
}

void WindowNodeEditor::DrawAlignmentNode(NodeBuilder& builder, Node* node)
{
    //Header------------------------------------

    builder.Header(node->Color);
    builder.SetCustomNode();
    ImGui::Spring(0);
    ImGui::TextUnformatted(node->Name.c_str());
    ImGui::Spring(1);
    ImGui::Dummy(ImVec2(0, 28));
    ImGui::Spring(0);
    builder.EndHeader();

    //Output------------------------------------

    NodeAlignment* alignmentNode = (NodeAlignment*)node;
    
    PinInt* pinInt = (PinInt*)node->Outputs[0];

    if (!pinInt) return;

    auto alpha = ImGui::GetStyle().Alpha;
    if (newLinkPin && !CanCreateLink(newLinkPin, pinInt) && pinInt != newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

    //TODO Set default selcetd

    ImGui::BeginColumns("Columns", 2, ImGuiColumnsFlags_NoBorder);
    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 70);
 
    static int selected = -1;
    for (int i = 1; i < 9; i++)
    {

        if (ImGui::Selectable(alignmentNode->aligmentArray[i], selected == i))
        {
            pinInt->pinInt = i;
            node->updateLinks = true;
            selected = i;
        }
    }

    ImGui::NextColumn();

    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 40);

    ImGui::Dummy(ImVec2(0, 50));
    
    ed::BeginPin(pinInt->ID, ed::PinKind::Output);

    DrawPinIcon(pinInt, IsPinLinked(pinInt->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    ImGui::EndColumns();

    if (node->updateLinks)
    {
        node->updateLinks = false;

        Pin* linkedPin = GetPinLinkedTo(pinInt->ID);

        if (linkedPin && pinInt) UpdateNodeLinks(pinInt, linkedPin);
    }


}

void WindowNodeEditor::DrawColorNode(NodeBuilder& builder, Node* node)
{
    //Header------------------------------------

    builder.Header(node->Color);
    builder.SetCustomNode();
    ImGui::Spring(0);
    ImGui::TextUnformatted(node->Name.c_str());
    ImGui::Spring(1);
    ImGui::Dummy(ImVec2(0, 28));
    ImGui::Spring(0);
    builder.EndHeader();

    //Output------------------------------------

    NodeColor* colorNode = (NodeColor*)node;

    PinFloat4* pinFloat4 = nullptr;

    auto alpha = ImGui::GetStyle().Alpha;

    ImGui::BeginColumns("Columns", 2, ImGuiColumnsFlags_NoBorder);
    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 450);


    for (Pin* input : colorNode->Inputs)
    {
        pinFloat4 = ((PinFloat4*)input);

        if (!pinFloat4)
            continue;

       
        if (newLinkPin && !CanCreateLink(newLinkPin, input) && input != newLinkPin)
            alpha = alpha * (48.0f / 255.0f);

        ed::BeginPin(input->ID, ed::PinKind::Input);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

        if (!input->Name.empty())
        {
            ImGui::TextUnformatted(input->Name.c_str());
            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();

        }

        if (pinFloat4->Name == "Color1")
        {
            if (ImGui::ColorEdit4(pinFloat4->Name.c_str(), (float*)&pinFloat4->pinFloat4, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoOptions))
            {
                colorNode->color1 = pinFloat4->pinFloat4;
            }
        }
        else if (pinFloat4->Name == "Color2")
        {
            if (ImGui::ColorEdit4(pinFloat4->Name.c_str(), (float*)&pinFloat4->pinFloat4, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoOptions))
            {
                colorNode->color2 = pinFloat4->pinFloat4;
            }
        }


        ImGui::PopStyleVar();

        ed::EndPin();
    }

    ImGui::NextColumn();

    ImGui::SetColumnWidth(ImGui::GetColumnIndex(), 40);

    ImGui::Dummy(ImVec2(0, 10));

    pinFloat4 = (PinFloat4*)colorNode->Outputs[0];

    if (!pinFloat4)
        return;


    if (newLinkPin && !CanCreateLink(newLinkPin, pinFloat4) && pinFloat4 != newLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);   

    ed::BeginPin(pinFloat4->ID, ed::PinKind::Output);

    DrawPinIcon(pinFloat4, IsPinLinked(pinFloat4->ID), (int)(alpha * 255));

    ImGui::PopStyleVar();

    ed::EndPin();

    ImGui::EndColumns();


    //Update Node Links

    if (pinFloat4)
    {
        pinFloat4->pinFloat4 = colorNode->ComputeColor();
    }

    Pin* linkedPin = GetPinLinkedTo(pinFloat4->ID);

    if (linkedPin && pinFloat4)
        UpdateNodeLinks(pinFloat4, linkedPin);

}


void WindowNodeEditor::DrawBlueprintNode(NodeBuilder& builder, Node* node)
{
    builder.Header(node->Color);
    ImGui::Spring(0);
    ImGui::TextUnformatted(node->Name.c_str());
    ImGui::Spring(1);
    ImGui::Dummy(ImVec2(0, 28));
    if (hasOutputDelegates)
    {
        ImGui::BeginVertical("delegates", ImVec2(0, 28));
        ImGui::Spring(1, 0);
        for (auto output : node->Outputs)
        {
            if (output->Type != PinType::Delegate)
                continue;

            auto alpha = ImGui::GetStyle().Alpha;
            if (newLinkPin && !CanCreateLink(newLinkPin, output) && output != newLinkPin)
                alpha = alpha * (48.0f / 255.0f);

            ed::BeginPin(output->ID, ed::PinKind::Output);
            ed::PinPivotAlignment(ImVec2(1.0f, 0.5f));
            ed::PinPivotSize(ImVec2(0, 0));
            ImGui::BeginHorizontal(output->ID.AsPointer());
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            if (!output->Name.empty())
            {
                ImGui::TextUnformatted(output->Name.c_str());
                ImGui::Spring(0);
            }
            DrawPinIcon(output, IsPinLinked(output->ID), (int)(alpha * 255));
            ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
            ImGui::EndHorizontal();
            ImGui::PopStyleVar();
            ed::EndPin();

            //DrawItemRect(ImColor(255, 0, 0));
        }
        ImGui::Spring(1, 0);
        ImGui::EndVertical();
        ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
    }
    else
        ImGui::Spring(0);
    builder.EndHeader();


    SimpleAndBlueprintPins(builder, node);
}

void WindowNodeEditor::SimpleAndBlueprintPins(NodeBuilder& builder, Node* node)
{
    for (Pin* input : node->Inputs)
    {
        auto alpha = ImGui::GetStyle().Alpha;
        if (newLinkPin && !CanCreateLink(newLinkPin, input) && input != newLinkPin)
            alpha = alpha * (48.0f / 255.0f);

        builder.Input(input->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        DrawPinIcon(input, IsPinLinked(input->ID), (int)(alpha * 255));
        ImGui::Spring(0);
        if (!input->Name.empty())
        {
            ImGui::TextUnformatted(input->Name.c_str());
            ImGui::Spring(0);
        }

        if (input->Type == PinType::Bool)
        {
            //ImGui::Button("Hello");
            ImGui::Spring(0);
        }
        ImGui::PopStyleVar();
        builder.EndInput();
    }

    if (isSimple)
    {
        builder.Middle();

        ImGui::Spring(1, 0);
        ImGui::TextUnformatted(node->Name.c_str());
        ImGui::Spring(1, 0);
    }

    for (Pin* output : node->Outputs)
    {
        if (!isSimple && output->Type == PinType::Delegate)
            continue;

        auto alpha = ImGui::GetStyle().Alpha;
        if (newLinkPin && !CanCreateLink(newLinkPin, output) && output != newLinkPin)
            alpha = alpha * (48.0f / 255.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        builder.Output(output->ID);


        static bool wasActive = false;
        static char* boolText = "False";

        //PinFloat3* float3Temp = dynamic_cast<PinFloat3*>(output);

        switch (output->Type)
        {
        case PinType::String:

            ImGui::PushItemWidth(100.0f);
            if (ImGui::InputText("##edit", (char*)((PinString*)output)->pinString.c_str(), 127))
            {
                node->updateLinks = true;
            }

            ImGui::PopItemWidth();
            if (ImGui::IsItemActive() && !wasActive)
            {
                ed::EnableShortcuts(false);
                wasActive = true;
            }
            else if (!ImGui::IsItemActive() && wasActive)
            {
                ed::EnableShortcuts(true);
                wasActive = false;
            }
            ImGui::Spring(0);

            break;

        case PinType::Bool:

            ImGui::PushItemWidth(100.0f);

            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 0.6f, 0.6f));

            if (ImGui::Button(boolText))
            {
                ((PinBool*)output)->pinBool = !((PinBool*)output)->pinBool;

                if (((PinBool*)output)->pinBool)
                    boolText = "True";
                else
                    boolText = "False";

                node->updateLinks = true;
            }
            ImGui::PopStyleColor(1);
            ImGui::PopItemWidth();
            if (ImGui::IsItemActive() && !wasActive)
            {
                ed::EnableShortcuts(false);
                wasActive = true;
            }
            else if (!ImGui::IsItemActive() && wasActive)
            {
                ed::EnableShortcuts(true);
                wasActive = false;
            }
            ImGui::Spring(0);

            break;

        case PinType::Float:

            ImGui::PushItemWidth(100.0f);
            if (ImGui::InputFloat("", &((PinFloat*)output)->pinFloat, 0.0f, 0.0f, "%.2f"))
            {
                node->updateLinks = true;
            }

            ImGui::PopItemWidth();
            if (ImGui::IsItemActive() && !wasActive)
            {
                ed::EnableShortcuts(false);
                wasActive = true;
            }
            else if (!ImGui::IsItemActive() && wasActive)
            {
                ed::EnableShortcuts(true);
                wasActive = false;
            }
            ImGui::Spring(0);

            break;
        case PinType::Float3:

            ImGui::PushItemWidth(200.0f);

            if (ImGui::DragFloat3("", (float*)&((PinFloat3*)output)->pinFloat3, 1.0, 0, 0, "%.2f"))
            {
                node->updateLinks = true;
            }

            ImGui::PopItemWidth();
            if (ImGui::IsItemActive() && !wasActive)
            {
                ed::EnableShortcuts(false);
                wasActive = true;
            }
            else if (!ImGui::IsItemActive() && wasActive)
            {
                ed::EnableShortcuts(true);
                wasActive = false;
            }
            ImGui::Spring(0);

            break;
        case PinType::Float4:

            //ImGui::PushItemWidth(200.0f);
            ////ImGui::InputFloat3("", (float*)&f3, 2);

            //ImGui::DragFloat4("", (float*)&((PinFloat3*)output), ImGuiColorEditFlags_Float);

            //ImGui::ColorButton("", *(ImVec4*)&col , ImGuiColorEditFlags_NoBorder, ImVec2(20, 20));

            //ImGui::PopItemWidth();
            //if (ImGui::IsItemActive() && !wasActive)
            //{
            //    ed::EnableShortcuts(false);
            //    wasActive = true;
            //}
            //else if (!ImGui::IsItemActive() && wasActive)
            //{
            //    ed::EnableShortcuts(true);
            //    wasActive = false;
            //}
            //ImGui::Spring(0);

            break;
        default:
            break;
        }

        if (node->updateLinks)
        {
            node->updateLinks = false;

            Pin* linkedPin = GetPinLinkedTo(output->ID);

            if (linkedPin && output)
                UpdateNodeLinks(output, linkedPin);
        }

        if (!output->Name.empty())
        {
            ImGui::Spring(0);
            ImGui::TextUnformatted(output->Name.c_str());
        }
        ImGui::Spring(0);
        DrawPinIcon(output, IsPinLinked(output->ID), (int)(alpha * 255));
        ImGui::PopStyleVar();
        builder.EndOutput();
    }
}

void WindowNodeEditor::DrawCommentNode(Node* node)
{

    const float commentAlpha = 0.75f;

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
    ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
    ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
    ed::BeginNode(node->ID);
    ImGui::PushID(node->ID.AsPointer());
    ImGui::BeginVertical("content");
    ImGui::BeginHorizontal("horizontal");
    ImGui::Spring(1);
    ImGui::TextUnformatted(node->Name.c_str());
    ImGui::Spring(1);
    ImGui::EndHorizontal();
    ed::Group(node->Size);
    ImGui::EndVertical();
    ImGui::PopID();
    ed::EndNode();
    ed::PopStyleColor(2);
    ImGui::PopStyleVar();

    if (ed::BeginGroupHint(node->ID))
    {
        //auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
        auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

        //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

        auto min = ed::GetGroupMin();
        //auto max = ed::GetGroupMax();

        ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
        ImGui::BeginGroup();
        ImGui::TextUnformatted(node->Name.c_str());
        ImGui::EndGroup();

        auto drawList = ed::GetHintBackgroundDrawList();

        auto hintBounds = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

        drawList->AddRectFilled(
            hintFrameBounds.GetTL(),
            hintFrameBounds.GetBR(),
            IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

        drawList->AddRect(
            hintFrameBounds.GetTL(),
            hintFrameBounds.GetBR(),
            IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

        //ImGui::PopStyleVar();
    }
    ed::EndGroupHint();
}

void WindowNodeEditor::DrawHoudiniNode(Pin* newLinkPin, Node* node)
{

    const float rounding = 10.0f;
    const float padding = 12.0f;


    ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(229, 229, 229, 200));
    ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(125, 125, 125, 200));
    ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(229, 229, 229, 60));
    ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(125, 125, 125, 60));

    const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

    ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
    ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
    ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
    ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
    ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
    ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
    ed::PushStyleVar(ed::StyleVar_PinRadius, 6.0f);
    ed::BeginNode(node->ID);

    ImGui::BeginVertical(node->ID.AsPointer());
    if (!node->Inputs.empty())
    {
        ImGui::BeginHorizontal("inputs");
        ImGui::Spring(1, 0);

        ImRect inputsRect;
        int inputAlpha = 200;
        for (auto pin : node->Inputs)
        {
            ImGui::Dummy(ImVec2(padding, padding));
            inputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
            ImGui::Spring(1, 0);
            inputsRect.Min.y -= padding;
            inputsRect.Max.y -= padding;

            //ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
            //ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
            ed::PushStyleVar(ed::StyleVar_PinCorners, 15);
            ed::BeginPin(pin->ID, ed::PinKind::Input);
            ed::PinPivotRect(inputsRect.GetCenter(), inputsRect.GetCenter());
            ed::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
            ed::EndPin();
            //ed::PopStyleVar(3);
            ed::PopStyleVar(1);

            auto drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(inputsRect.GetTL(), inputsRect.GetBR(),
                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 15);
            drawList->AddRect(inputsRect.GetTL(), inputsRect.GetBR(),
                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 15);

            if (newLinkPin && !CanCreateLink(newLinkPin, pin) && pin != newLinkPin)
                inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        }

        //ImGui::Spring(1, 0);
        ImGui::EndHorizontal();
    }

    ImGui::BeginHorizontal("content_frame");
    ImGui::Spring(1, padding);

    ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
    ImGui::Dummy(ImVec2(160, 0));
    ImGui::Spring(1);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::TextUnformatted(node->Name.c_str());
    ImGui::PopStyleColor();
    ImGui::Spring(1);
    ImGui::EndVertical();
    auto contentRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

    ImGui::Spring(1, padding);
    ImGui::EndHorizontal();

    if (!node->Outputs.empty())
    {
        ImGui::BeginHorizontal("outputs");
        ImGui::Spring(1, 0);

        ImRect outputsRect;
        int outputAlpha = 200;
        for (auto pin : node->Outputs)
        {
            ImGui::Dummy(ImVec2(padding, padding));
            outputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
            ImGui::Spring(1, 0);
            outputsRect.Min.y += padding;
            outputsRect.Max.y += padding;

            ed::PushStyleVar(ed::StyleVar_PinCorners, 3);
            ed::BeginPin(pin->ID, ed::PinKind::Output);
            ed::PinPivotRect(outputsRect.GetCenter(), outputsRect.GetCenter());
            ed::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
            ed::EndPin();
            ed::PopStyleVar();

            auto drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR(),
                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 15);
            drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR(),
                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 15);


            if (newLinkPin && !CanCreateLink(newLinkPin, pin) && pin != newLinkPin)
                outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        }

        ImGui::EndHorizontal();
    }

    ImGui::EndVertical();

    ed::EndNode();
    ed::PopStyleVar(7);
    ed::PopStyleColor(4);

    auto drawList = ed::GetNodeBackgroundDrawList(node->ID);
}

void WindowNodeEditor::DrawTreeNode(Pin* newLinkPin, Node* node)
{
    const float rounding = 5.0f;
    const float padding = 12.0f;

    const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

    ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
    ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
    ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
    ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

    ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
    ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
    ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
    ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
    ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
    ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
    ed::PushStyleVar(ed::StyleVar_PinRadius, 5.0f);
    ed::BeginNode(node->ID);

    ImGui::BeginVertical(node->ID.AsPointer());
    ImGui::BeginHorizontal("inputs");
    ImGui::Spring(0, padding * 2);

    ImRect inputsRect;
    int inputAlpha = 200;
    if (!node->Inputs.empty())
    {
        auto pin = node->Inputs[0];
        ImGui::Dummy(ImVec2(0, padding));
        ImGui::Spring(1, 0);
        inputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

        ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
        ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
        ed::PushStyleVar(ed::StyleVar_PinCorners, 12);
        ed::BeginPin(pin->ID, ed::PinKind::Input);
        ed::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
        ed::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
        ed::EndPin();
        ed::PopStyleVar(3);

        if (newLinkPin && !CanCreateLink(newLinkPin, pin) && pin != newLinkPin)
            inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
    }
    else
        ImGui::Dummy(ImVec2(0, padding));

    ImGui::Spring(0, padding * 2);
    ImGui::EndHorizontal();

    ImGui::BeginHorizontal("content_frame");
    ImGui::Spring(1, padding);

    ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
    ImGui::Dummy(ImVec2(160, 0));
    ImGui::Spring(1);
    ImGui::TextUnformatted(node->Name.c_str());
    ImGui::Spring(1);
    ImGui::EndVertical();
    auto contentRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

    ImGui::Spring(1, padding);
    ImGui::EndHorizontal();

    ImGui::BeginHorizontal("outputs");
    ImGui::Spring(0, padding * 2);

    ImRect outputsRect;
    int outputAlpha = 200;
    if (!node->Outputs.empty())
    {
        auto pin = node->Outputs[0];
        ImGui::Dummy(ImVec2(0, padding));
        ImGui::Spring(1, 0);
        outputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

        ed::PushStyleVar(ed::StyleVar_PinCorners, 3);
        ed::BeginPin(pin->ID, ed::PinKind::Output);
        ed::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
        ed::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
        ed::EndPin();
        ed::PopStyleVar();

        if (newLinkPin && !CanCreateLink(newLinkPin, pin) && pin != newLinkPin)
            outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
    }
    else
        ImGui::Dummy(ImVec2(0, padding));

    ImGui::Spring(0, padding * 2);
    ImGui::EndHorizontal();

    ImGui::EndVertical();

    ed::EndNode();
    ed::PopStyleVar(7);
    ed::PopStyleColor(4);

    auto drawList = ed::GetNodeBackgroundDrawList(node->ID);

    drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);

    drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);

    drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);

    drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);

    drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);

    drawList->AddRect(
        contentRect.GetTL(),
        contentRect.GetBR(),
        IM_COL32(48, 128, 255, 100), 0.0f);
    //ImGui::PopStyleVar();
}


Node* WindowNodeEditor::CreateEmiterNode()
{
    newNode = new Node(GetNextId(), "Emitter", ImColor(128, 195, 248));
   
    newNode->Inputs.emplace_back(new PinFloat(GetNextId(), "Spawn Rate"));
    newNode->Inputs.emplace_back(new PinFloat3(GetNextId(), "Initial Position"));
    newNode->Inputs.emplace_back(new PinFloat4(GetNextId(), "Velocity"));
    newNode->Inputs.emplace_back(new PinFloat(GetNextId(), "Lifetime"));
    newNode->Inputs.emplace_back(new PinFloat3(GetNextId(), "Size"));
    newNode->Inputs.emplace_back(new PinFloat4(GetNextId(), "Color"));
    newNode->Inputs.emplace_back(new PinInt(GetNextId(), "Alignment"));
    newNode->Inputs.emplace_back(new PinTexture(GetNextId(), "Texture"));
    newNode->Inputs.emplace_back(new PinBool(GetNextId(), "Active"));

    nodes.emplace_back(newNode);
    BuildNode(newNode);

    return newNode;
}

Node* WindowNodeEditor::CreateParticleSystem()
{

    Node* rateNode = CreateFloatNode(); ed::SetNodePosition(rateNode->ID, ImVec2(0, 0));
    Node* velocityNode = CreateVelocityNode(); ed::SetNodePosition(velocityNode->ID, ImVec2(-150, 50));
    Node* lifetimeFloat = CreateFloatNode(); ed::SetNodePosition(lifetimeFloat->ID, ImVec2(0, 150));
    Node* sizeFloat3 = CreateFloat3Node("Size"); ed::SetNodePosition(sizeFloat3->ID, ImVec2(0, 200));
    Node* colorNode = CreateColorNode(); ed::SetNodePosition(colorNode->ID, ImVec2(0, 300));
    Node* alignmentNode = CreateAlignmentNode(); ed::SetNodePosition(alignmentNode->ID, ImVec2(0, 400));
    Node* textureNode = CreateTextureNode(); ed::SetNodePosition(textureNode->ID, ImVec2(0, 600));
    Node* activeNode = CreateBoolNode(); ed::SetNodePosition(activeNode->ID, ImVec2(0, 700));
    Node* emitterNode = CreateEmiterNode(); ed::SetNodePosition(emitterNode->ID, ImVec2(500, 300));

    ed::NavigateToContent();

    links.push_back(Link(GetNextLinkId(), rateNode->Outputs[0]->ID, emitterNode->Inputs[0]->ID));
    links.push_back(Link(GetNextLinkId(), velocityNode->Outputs[0]->ID, emitterNode->Inputs[2]->ID));
    links.push_back(Link(GetNextLinkId(), lifetimeFloat->Outputs[0]->ID, emitterNode->Inputs[3]->ID));
    links.push_back(Link(GetNextLinkId(), sizeFloat3->Outputs[0]->ID, emitterNode->Inputs[4]->ID));
    links.push_back(Link(GetNextLinkId(), colorNode->Outputs[0]->ID, emitterNode->Inputs[5]->ID));
    links.push_back(Link(GetNextLinkId(), alignmentNode->Outputs[0]->ID, emitterNode->Inputs[6]->ID));
    links.push_back(Link(GetNextLinkId(), textureNode->Outputs[0]->ID, emitterNode->Inputs[7]->ID));
    links.push_back(Link(GetNextLinkId(), activeNode->Outputs[0]->ID, emitterNode->Inputs[8]->ID));




    return emitterNode;
}

//TODO: Unify all functions below in a single Function

Node* WindowNodeEditor::CreateVelocityNode()
{
    //TODO:: Allow dynamically allocate more velocities???

    newNode = new NodeVelocity(GetNextId(), "Velocity", ImColor(128, 195, 248));
  
    nodes.emplace_back(newNode);
    BuildNode(newNode);

    return newNode;
}

Node* WindowNodeEditor::CreateFloat3Node(std::string name)
{
    newNode = new NodeFloat3(GetNextId(), name.c_str(), ImColor(128, 195, 248));

    nodes.emplace_back(newNode);
    BuildNode(newNode);

    return newNode;
}

Node* WindowNodeEditor::CreateTextureNode()
{
    newNode = new NodeTexture(GetNextId(), "Texture", ImColor(128, 195, 248));

    nodes.emplace_back(newNode);
    BuildNode(newNode);

    return newNode;
}

Node* WindowNodeEditor::CreateAlignmentNode()
{
    newNode = new NodeAlignment(GetNextId(), "Alignment", ImColor(128, 195, 248));

    nodes.emplace_back(newNode);
    BuildNode(newNode);

    return newNode;
}

//Node* WindowNodeEditor::CreateFloat3Node()
//{
//    newNode = new Node(GetNextId(), "", ImColor(128, 195, 248));
//
//    newNode->Type = NodeType::Simple;
//    newNode->Outputs.emplace_back(new PinFloat3(GetNextId(), "Float3"));
//
//    nodes.emplace_back(newNode);
//    BuildNode(newNode);
//
//    
//    return newNode;
//}

Node* WindowNodeEditor::CreateFloatNode()
{
    newNode = new Node(GetNextId(), "", ImColor(128, 195, 248));

    newNode->Type = NodeType::Simple;
    newNode->Outputs.emplace_back(new PinFloat(GetNextId(), "Float"));

    nodes.emplace_back(newNode);
    BuildNode(newNode);


    return newNode;
}

Node* WindowNodeEditor::CreateBoolNode()
{
    newNode = new Node(GetNextId(), "", ImColor(128, 195, 248));

    newNode->Type = NodeType::Simple;
    newNode->Outputs.emplace_back(new PinBool(GetNextId(), "Bool"));

    nodes.emplace_back(newNode);
    BuildNode(newNode);

    return newNode;
}

Node* WindowNodeEditor::CreateColorNode()
{

    newNode = new NodeColor(GetNextId(), "Color", ImColor(128, 195, 248));

    nodes.emplace_back(newNode);
    BuildNode(newNode);

    return newNode;

}

Node* WindowNodeEditor::CreateContainerNode()
{
    newNode = new Node(GetNextId(), "Container", ImColor(50, 50, 50));

    newNode->Type = NodeType::Comment;
    newNode->Size = ImVec2(200, 200);

    nodes.emplace_back(newNode);
    BuildNode(newNode);


    return nullptr;
}


void WindowNodeEditor::RightClickMenu(Node* node)
{
   
    if (ImGui::BeginMenu("Particle Nodes"))
    {
        
        if (ImGui::MenuItem("Particle Emitter")) node = CreateEmiterNode();

        if (ImGui::MenuItem("Module Velocity"))  node = CreateVelocityNode();

        if (ImGui::MenuItem("Module Texture"))  node = CreateTextureNode();

        if (ImGui::MenuItem("Module Alignment"))  node = CreateAlignmentNode();

        if (ImGui::MenuItem("Module Color"))  node = CreateColorNode();

        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Base Nodes"))
    {
        if (ImGui::MenuItem("Container"))  node = CreateContainerNode();

        else if (ImGui::MenuItem("Boolean")) node = CreateBoolNode();
        
        else if (ImGui::MenuItem("Float")) node = CreateFloatNode();

        else if (ImGui::MenuItem("Float3")) node = CreateFloat3Node();

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
    default:
        break;
    }
}


