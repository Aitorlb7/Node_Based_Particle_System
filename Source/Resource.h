#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"


class JsonConfig;

enum class ResourceType
{
	Mesh,
	Material,
	Texture,
	Scene,
	Model,
	Folder,
	Shader,
	ParticleSystem,
	None
};

class Resource
{
public:
	ResourceType type = ResourceType::None;
	uint32 UID = 0;


	std::string assetsFile = "";
	std::string libraryFile = "";
	uint referenceCount = 0;

	std::string name = "";
	std::vector<uint32> resourcesInModels;

public:

	Resource();
	Resource(ResourceType type);
	Resource(ResourceType type, const char* assetsFile, const char* name = "", uint32 UID = 0);
	~Resource();

	inline const uint32 GetUID() const { return UID; }
	inline ResourceType GetType() const { return type; }

	inline std::string GetAssetsFile() const { return assetsFile; }
	inline std::string GetLibraryFile() const { return libraryFile; }

	inline uint GetReferenceCount() const { return referenceCount; }

	bool IsLoadedToMemory() const;


};
#endif //__RESOURCE_H__
