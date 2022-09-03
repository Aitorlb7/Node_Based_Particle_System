#ifndef _VELOCITY_NODE_
#define _VELOCITY_NODE_

#include "Node.h"
#include "Dependencies/MathGeoLib/include/Math/float3.h"
#include "Dependencies/MathGeoLib/include/Math/float4.h"

class NodeVelocity : public Node
{
public:

    NodeVelocity(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeVelocity() override;

    float4 ComputeVelocity();

protected:

    void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;

public:

    float4 initialVelocity1;
    float4 initialVelocity2;



};


#endif //_VELOCITY_NODE_