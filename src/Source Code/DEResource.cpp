#include "DEResource.h"
#include"Globals.h"

Resource::Resource(std::string& _uid, Resource::Type _type) : uid(_uid), assetsFile(_uid),  type(_type),
referenceCount(0)
{

}

Resource::~Resource()
{
	uid.clear();
	assetsFile.clear();
	type = Type::UNKNOWN;

	if (referenceCount != 0)
		LOG(  "DELETING RESOURCE WITH MULTIPLE REFERENCE COUNTS");

	referenceCount = 0;
}

void Resource::SetAssetsPath(const char* _aPath)
{
	assetsFile = _aPath;
}