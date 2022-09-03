#pragma once
#include "Node.h"
#include "Dependencies/MathGeoLib/include/Math/float3.h"

class ResourceMesh;
class GameObject;

class NodeSpawnArea : public Node
{
public:

    NodeSpawnArea(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeSpawnArea() override;

    float3 ComputePosition();

protected:

    void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;

    ResourceMesh* areaResourceMesh;

    bool isAreaVisible;

};

