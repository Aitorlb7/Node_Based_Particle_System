#pragma once
#include "Node.h"
class NodeComment : public Node
{
public:

    NodeComment(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeComment();

protected:

    void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;

};

