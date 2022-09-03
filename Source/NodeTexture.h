#ifndef _TEXTURE_NODE_
#define _TEXTURE_NODE_

#include "Node.h"

class ResourceTexture;

class NodeTexture : public Node
{
public:

    NodeTexture(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeTexture() override;

    ResourceTexture* GetDefaultParticleTex();

protected:

    void Draw(NodeBuilder& builder, WindowNodeEditor* nodeEditorWindow) override;

};


#endif //_TEXTURE_NODE_