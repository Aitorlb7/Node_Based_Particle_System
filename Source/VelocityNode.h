#ifndef _VELOCITY_NODE_
#define _VELOCITY_NODE_

#include "Node.h"
#include "Dependencies/MathGeoLib/include/Math/float3.h"
#include "Dependencies/MathGeoLib/include/Math/float4.h"

class VelocityNode : public Node
{
public:

    VelocityNode(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~VelocityNode();

    float4 ComputeVelocity();



public:

    float4 initialVelocity1;
    float4 initialVelocity2;

    /*float speed1;
    float speed2;*/

    //float4 finalVelocity;

};


#endif //_VELOCITY_NODE_