#ifndef _NODE_BUILDER_
#define _NODE_BUILDER_

#include "Node.h"

enum class Stage
{
    Invalid,
    Begin,
    Header,
    Content,
    Input,
    Output,
    Middle,
    End
};


class NodeBuilder
{
public:
    NodeBuilder(ImTextureID texture = nullptr, int textureWidth = 0, int textureHeight = 0);



    void Begin(ed::NodeId id);
    void End();

    void Header(const ImVec4& color = ImVec4(1, 1, 1, 1));
    void EndHeader();

    void Input(ed::PinId id);
    void EndInput();

    void Middle();

    void Output(ed::PinId id);
    void EndOutput();

    void SetCustomNode();


private:

    bool SetStage(Stage stage);

    void Pin(ed::PinId id, ax::NodeEditor::PinKind kind);
    void EndPin();

    ImTextureID HeaderTextureId;
    int         HeaderTextureWidth;
    int         HeaderTextureHeight;
    ed::NodeId  CurrentNodeId;
    Stage       CurrentStage;
    ImU32       HeaderColor;
    ImVec2      NodeMin;
    ImVec2      NodeMax;
    ImVec2      HeaderMin;
    ImVec2      HeaderMax;
    ImVec2      ContentMin;
    ImVec2      ContentMax;
    bool        HasHeader;

    bool        CustomNode;
};
#endif //_NODE_BUILDER_