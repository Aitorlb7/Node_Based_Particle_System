#include "Node.h"

Node::Node(int id, const char* name, ImColor color):
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
