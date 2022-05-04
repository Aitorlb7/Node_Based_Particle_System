#ifndef _PIN_BOOL_
#define _PIN_BOOL_
#include "Pin.h"

class PinBool : public Pin
{
public:

    PinBool(int id, const char* name) : Pin(id, name, PinType::Bool),
        pinBool(false)
    {
    }
    ~PinBool() {}

public:

    bool pinBool;

};


#endif //_PIN_BOOL_