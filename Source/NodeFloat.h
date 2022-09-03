#pragma once
#include "Node.h"
#include "Dependencies/MathGeoLib/include/Math/float3.h"

class NodeFloat : public Node
{
public:

    NodeFloat(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeFloat() override;

protected:

    void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;

};

