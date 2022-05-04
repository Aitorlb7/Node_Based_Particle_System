#include "Node.h"

Node::Node(int id, const char* name, ImColor color, NodeType type):
    ID(id), 
    Name(name), 
    Color(color), 
    Type(type),
    Size(0, 0),
    updateLinks(false)
{
}

Node::~Node()
{
}

const Pin* Node::GetInputPinByName(std::string pinName) const
{
    for (Pin* pin : Inputs)
    {
        if (pinName == pin->Name)
            return pin;
    }

    return nullptr;
}

const Pin* Node::GetOutputPinByName(std::string pinName) const
{
    for (Pin* pin : Outputs)
    {
        if (pinName == pin->Name)
            return pin;
    }

    return nullptr;
}
