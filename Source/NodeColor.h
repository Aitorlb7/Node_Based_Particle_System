#ifndef _COLOR_NODE_
#define _COLOR_NODE_

#include "Node.h"
#include "Dependencies/MathGeoLib/include/Math/float4.h"

class NodeColor : public Node
{
public:

	NodeColor(int id, const char* name, ImColor color = ImColor(255, 255, 255));

	~NodeColor();

	float4 ComputeColor();


public:


	float4 color1;
	float4 color2;



};


#endif //_COLOR_NODE_