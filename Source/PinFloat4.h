#ifndef _PIN_FLOAT4_
#define _PIN_FLOAT4_
#include "Pin.h"
#include "Dependencies/MathGeoLib/include/Math/float4.h"
class PinFloat4 : public Pin
{
public:

    PinFloat4(int id, const char* name) : Pin(id, name, PinType::Float4),
        pinFloat4(float4::zero)
    {
    }
    ~PinFloat4() {}

public:

    float4 pinFloat4;

};


#endif //_PIN_FLOAT4_