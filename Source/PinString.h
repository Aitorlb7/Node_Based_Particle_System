#ifndef _PIN_STRING_
#define _PIN_STRING_
#include "Pin.h"

class PinString : public Pin
{
public:

    PinString(int id, const char* name) : Pin(id, name, PinType::String),
        pinString("")
    {
    }
    ~PinString() {}

public:

    std::string pinString;

};


#endif //_PIN_STRING_