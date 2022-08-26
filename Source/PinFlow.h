#ifndef _PIN_FLOW_
#define _PIN_FLOW_

#include "Pin.h"
#include "Node.h"

class EmitterInstance;

class PinFlow : public Pin
{
public:

    PinFlow(int id, const char* name) : Pin(id, name, PinType::Flow),
    emitterInstance(nullptr)
    {
    }
    ~PinFlow() {}

public:

    EmitterInstance* emitterInstance;
};


#endif //_PIN_FLOW_
