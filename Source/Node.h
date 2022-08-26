#ifndef _NODE_
#define _NODE_
#include "Dependencies/NodeEditor/imgui_node_editor.h"
#include "Globals.h"
#include "Pin.h"

namespace ed = ax::NodeEditor;

class NodeBuilder;
class WindowNodeEditor;
class Node;

enum class NodeType
{
    Comment,
    Emitter,
    Bool,
    Float,
    Float3,
    Color,
    ColorOverTime,
    Velocity,
    Texture,
    Alignment,
    Gravity,
    GravitationalPull,
    Default
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
    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255), NodeType type = NodeType::Default);
    ~Node();


    const Pin* GetInputPinByName(std::string pinName) const;
    const Pin* GetOutputPinByName(std::string pinName) const;

    virtual void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) = 0;

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

    bool updateLinks;
    bool updateEmitter;

    //TODO: Is really necesary????
    bool isActive;
};

#endif //_NODE_