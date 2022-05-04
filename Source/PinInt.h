#ifndef _PIN_INT_
#define _PIN_INT_
#include "Pin.h"

class PinInt : public Pin
{
public:

    PinInt(int id, const char* name) : Pin(id,name,PinType::Int),
		pinInt(0)
    {
    }
    ~PinInt(){}

public:

    int pinInt;

};


#endif //_PIN_INT_