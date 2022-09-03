#pragma once
#include "Node.h"

class NodeGravity : public Node
{
public:

    NodeGravity(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeGravity() override;

protected:

    void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;
};

