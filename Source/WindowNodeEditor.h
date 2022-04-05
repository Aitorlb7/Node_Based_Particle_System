#include "Window.h"
#include "NodeBuilder.h"
#include <map>

struct ImRect;

class WindowNodeEditor: public Window
{
public:
	WindowNodeEditor(bool isActive);
	~WindowNodeEditor();

	void Start() override;

	void Draw() override;

	void CleanUp() override;

	void UpdateTouch();
	float GetTouchProgress(ed::NodeId id);
	//Node* FindNode(ed::NodeId id);
	Link* FindLink(ed::LinkId id);
	Pin* FindPin(ed::PinId id);
	bool IsPinLinked(ed::PinId id);
	bool CanCreateLink(Pin* a, Pin* b);

	void BuildNode(Node* node);

	void BuildNodes();

	Node* CreateEmiterNode();

	ImColor GetIconColor(PinType type);
	void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32 innerColor);
	void DrawPinIcon(const Pin& pin, bool connected, int alpha);
	bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size);
	void ShowLeftPane(float paneWidth);
	void ShowStyleEditor(bool* show);

	inline int GetNextId() { return nextId++;}
	inline ed::LinkId GetNextLinkId() { return ed::LinkId(GetNextId()); }
	//inline void TouchNode(ed::NodeId id) { nodeTouchTime[id] = touchTime;}

	ImRect ImRect_Expanded(const ImRect& rect, float x, float y);

public:
	std::vector<Node> nodes;

	const float          touchTime = 1.0f;
	std::map<ed::NodeId, float, NodeIdLess> nodeTouchTime;

private:
	int nextId = 1;
	const int pinIconSize = 24;


	std::vector<Link> links;
	ed::EditorContext* contextEditor = nullptr;
	ImTextureID      headerBackground = nullptr;




};