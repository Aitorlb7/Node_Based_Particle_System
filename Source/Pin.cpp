#include "Pin.h"
#include "Node.h"

Pin::Pin(int id, const char* name, PinType type) : 
	ID(id), 
	Node(nullptr), 
	Name(name), 
	Type(type), 
	Kind(PinKind::Input)
{
}

Pin::~Pin()
{
}
