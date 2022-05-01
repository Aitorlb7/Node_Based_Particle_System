#ifndef _NODE_
#define _NODE_
#include "Dependencies/NodeEditor/imgui_node_editor.h"
#include "Globals.h"
#include "Pin.h"

namespace ed = ax::NodeEditor;


enum class NodeType
{
    Blueprint,
    Simple,
    Tree,
    Comment,
    Houdini
};

enum class IconType : ImU32 
{   
    Flow, 
    Circle, 
    Square, 
    Grid, 
    RoundSquare, 
    Diamond 
};

class Node;


struct Link
{
    ed::LinkId ID;

    ed::PinId StartPinID;
    ed::PinId EndPinID;

    ImColor Color;

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId) :
        ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
    {
    }
};

struct NodeIdLess
{
    bool operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

class Node
{
public:
    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255));
    ~Node();

public:
    ed::NodeId ID;
    std::string Name;
    std::vector<Pin*> Inputs;
    std::vector<Pin*> Outputs;
    ImColor Color;
    NodeType Type;
    ImVec2 Size;

    std::string State;
    std::string SavedState;
};

#endif //_NODE_