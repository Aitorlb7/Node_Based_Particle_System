#ifndef _PIN_
#define _PIN_
#include "Dependencies/NodeEditor/imgui_node_editor.h"
#include "Globals.h"

namespace ed = ax::NodeEditor;

class Node;

enum class PinType
{
    Flow,
    Bool,
    Int,
    Float,
    String,
    Float3,
    Float4,
    Float4Array,
    Texture,
    Delegate,
};

enum class PinKind
{
    Output,
    Input
};


class Pin
{
public:

    Pin(int id, const char* name, PinType type);
    virtual ~Pin();

public:
    ed::PinId   ID;
    Node*       Node;
    std::string Name;
    PinType     Type;
    PinKind     Kind;

};


#endif //_PIN_
