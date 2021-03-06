#ifndef _TEXTURE_NODE_
#define _TEXTURE_NODE_

#include "Node.h"

class ResourceTexture;

class NodeTexture : public Node
{
public:

    NodeTexture(int id, const char* name, ImColor color = ImColor(255, 255, 255));

    ~NodeTexture();

    ResourceTexture* GetDefaultParticleTex();

};


#endif //_TEXTURE_NODE_