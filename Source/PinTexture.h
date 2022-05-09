#ifndef _PIN_TEXTURE_
#define _PIN_TEXTURE_

#include "Pin.h"
#include "ResourceTexture.h"

class PinTexture : public Pin
{
public:

    PinTexture(int id, const char* name) : Pin(id, name, PinType::Texture),
        pinTexture(nullptr)
    {
    }
    ~PinTexture() {}

public:

    ResourceTexture* pinTexture;

};


#endif //_PIN_TEXTURE_