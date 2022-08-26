#pragma once

#include "Pin.h"
#include "Dependencies/MathGeoLib/include/Math/float4.h"

class PinFloat4Array : public Pin
{
public:

    PinFloat4Array(int id, const char* name) : Pin(id, name, PinType::Float4Array)
    {

    }
    ~PinFloat4Array() {}

public:

    float4 float4Array[2];

};
