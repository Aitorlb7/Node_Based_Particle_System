#ifndef __RESOURCESCENE_H__
#define __RESOURCESCENE_H__

#include "Resource.h"


class ResourceScene : public Resource
{
public:
	ResourceScene();
	ResourceScene(const char* assetsFile,const char* libraryFile, const char* name , uint32 UID);

	~ResourceScene();

};

#endif //__RESOURCESCENE_H__