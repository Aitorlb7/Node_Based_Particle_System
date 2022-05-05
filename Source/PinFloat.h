#ifndef _PIN_FLOAT_
#define _PIN_FLOAT_
#include "Pin.h"

class PinFloat : public Pin
{
public:

    PinFloat(int id, const char* name) : Pin(id, name, PinType::Float),
        pinFloat(0)
    {
    }
    ~PinFloat() {}

public:

    float pinFloat;

};


#endif //_PIN_FLOAT_