#pragma once
#include "Node.h"

class GameObject;
class EmitterInstance;

class NodeGravitationalField : public Node
{
public:

    NodeGravitationalField(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeGravitationalField() override;

protected:

    void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;

public:

    GameObject* gameObject;

    bool addedField;

};

