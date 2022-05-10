#ifndef _FLOAT3_NODE_
#define _FLOAT3_NODE_

#include "Node.h"
#include "Dependencies/MathGeoLib/include/Math/float3.h"

class NodeFloat3 : public Node
{
public:

    NodeFloat3(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeFloat3();

    //float4 ComputeVelocity();

};


#endif //_FLOAT3_NODE_