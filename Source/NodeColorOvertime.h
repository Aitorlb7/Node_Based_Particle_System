#pragma once

#include "Node.h"
#include "Dependencies/MathGeoLib/include/Math/float4.h"


class NodeColorOvertime : public Node
{
public:

	NodeColorOvertime(int id, const char* name, ImColor color = ImColor(255, 255, 255));

	~NodeColorOvertime();

protected:

	void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;
};

