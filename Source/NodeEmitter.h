#pragma once
#include "Node.h"


class NodeEmitter : public Node
{
public:

	NodeEmitter(int id, const char* name, ImColor color = ImColor(255, 255, 255));

	~NodeEmitter();

protected:

	void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;
};

