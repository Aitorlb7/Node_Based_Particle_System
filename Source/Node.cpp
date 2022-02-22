#include "Node.h"

Node::Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)):
    ID(id), 
    Name(name), 
    Color(color), 
    Type(NodeType::Blueprint), 
    Size(0, 0)
{
}

Node::~Node()
{
}
