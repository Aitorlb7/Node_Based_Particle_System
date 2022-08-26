#ifndef _ALIGNMENT_NODE_
#define _ALIGNMENT_NODE_

#include "Node.h"

enum class Alignment
{
	None,
	Screen,
	Camera,
	AxisXY,
	AxisXZ,
	AxisYZ,
	AxisYX,
	AxisZX,
	AxisZY,
	Unknown
};

class NodeAlignment : public Node
{
public:

    NodeAlignment(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeAlignment();

	std::string GetStringBasedOnAlignment(Alignment alignment);

protected:

	void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;

public:

	const char* aligmentArray[(uint)Alignment::Unknown] = { "None", "Screen", "Camera", "AxisXY", "AxisXZ", "AxisYZ", "AxisYX", "AxisZX", "AxisZY" };

	const char* currentAlignment;

};


#endif //_ALIGNMENT_NODE_