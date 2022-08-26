#pragma once
#include "Node.h"

class GameObject;

class NodeGravitationalPull : public Node
{
public:

    NodeGravitationalPull(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeGravitationalPull();

protected:

    void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;

public:

    GameObject* gameObject;

};

