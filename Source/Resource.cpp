#include "Resource.h"
#include "Random.h"

Resource::Resource()
{
	UID = Random::GenerateRandomInt();
}

Resource::Resource(ResourceType type) : type(type)
{
	UID = Random::GenerateRandomInt();
}

Resource::Resource(ResourceType type, const char* assetsFile, const char* name, uint32 ID) : 
	type(type),
	name(name),
	assetsFile(assetsFile),
	UID(ID)
{
	UID == 0 ? Random::GenerateRandomInt() : ID;
}

Resource::~Resource()
{

}


