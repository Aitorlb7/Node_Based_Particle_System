#pragma once
#include "Node.h"
class NodeBool : public Node
{

public:

    NodeBool(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeBool() override;

protected:

    void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;
};

