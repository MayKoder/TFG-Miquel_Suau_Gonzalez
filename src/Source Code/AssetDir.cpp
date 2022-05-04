#include"AssetDir.h"
#include"Application.h"
#include"MO_ResourceManager.h"
#include"IM_FileSystem.h"
#include"DEJsonSupport.h"

AssetDir::AssetDir(const char* _dName, const char* _imPath, uint64 _lMod, bool _dir) : isDir(_dir), lastModTime(_lMod),
resourceType(Resource::Type::UNKNOWN), parentDir(nullptr)
{
	dirName = _dName;
	dirName.push_back('\0');
	importPath = _imPath;
	importPath.push_back('\0');
}

AssetDir::~AssetDir()
{
	dirName.clear();
	childDirs.clear();
	importPath.clear();
	metaFileDir.clear();
	libraryPath.clear();
	lastModTime = 0;
}

void AssetDir::ClearData()
{
	dirName.clear();
	childDirs.clear();
	isDir = false;
	importPath.clear();
	metaFileDir.clear();
	lastModTime = 0;
}

