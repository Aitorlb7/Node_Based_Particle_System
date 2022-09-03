#ifndef _COLOR_NODE_
#define _COLOR_NODE_

#include "Node.h"
#include "Dependencies/MathGeoLib/include/Math/float4.h"

class NodeColor : public Node
{
public:

	NodeColor(int id, const char* name, ImColor color = ImColor(255, 255, 255));

	~NodeColor() override;

	float4 ComputeColor();

protected:

	void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;

public:

	float4 color1;
	float4 color2;

};


#endif //_COLOR_NODE_