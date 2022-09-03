#pragma once
#include "Pin.h"
#include "GameObject.h"

class PinGameObject : public Pin
{

public:

    PinGameObject(int id, const char* name) : Pin(id, name, PinType::GameObject),
        gameObject(nullptr)
    {
    }
    ~PinGameObject() {}

public:

    GameObject* gameObject;

};

