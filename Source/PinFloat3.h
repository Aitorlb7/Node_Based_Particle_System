#ifndef _PIN_FLOAT3_
#define _PIN_FLOAT3_
#include "Pin.h"
#include "Dependencies/MathGeoLib/include/Math/float3.h"
class PinFloat3 : public Pin
{
public:

    PinFloat3(int id, const char* name) : Pin(id, name, PinType::Float3),
        float_3(float3::zero)
    {
    }
    ~PinFloat3() {}

public:

    float3 float_3;

};


#endif //_PIN_FLOAT3_