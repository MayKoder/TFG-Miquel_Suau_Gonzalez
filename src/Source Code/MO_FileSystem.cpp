#include "MO_FileSystem.h"
#include "Globals.h"

#include "IM_FileSystem.h"

#include "Application.h"

#include "MO_Editor.h"
#include "MO_ResourceManager.h"
#include"MO_Scene.h"

#include "PhysFS/include/physfs.h"
#include"DEJsonSupport.h"

#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include"RE_Shader.h"


M_FileSystem::M_FileSystem(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

M_FileSystem::~M_FileSystem()
{
}

bool M_FileSystem::Init()
{
	//Devil init
	LOG(  "DevIL Init");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	FileSystem::FSInit();

//#ifndef STANDALONE
//	MeshLoader::EnableDebugMode();
//#endif // !STANDALONE


	return true;
}

#include"MO_GUI.h"
#include"CreationTool.h"
bool M_FileSystem::Start()
{


	//GetAllFilesRecursive(App->moduleResources->meshesLibraryRoot);
	//App->moduleScene->defaultMaterial = (ResourceMaterial*)App->moduleResources->RequestFromAssets("Assets/Materials/default.mat");
	App->moduleRenderer3D->skybox.shaderRes = dynamic_cast<ResourceShader*>(App->moduleResources->RequestResource("Assets/Shaders/cubeMap.glsl", Resource::Type::SHADER));

	//App->moduleRenderer3D->gridInstance.LoadShader("Library/Shaders/1554189485.shdr");

	//for (int y = -20; y < 21; y+=1)
	//{
	//	for (int x = -20; x < 21; x+=1)
	//	{
	//		if (y == 20 && x == 20) {

	//		}
	//		else {
	//			App->moduleRenderer3D->gridInstance.AddNode(x, y, false, false);
	//		}
	//	}
	//}
	//App->moduleRenderer3D->gridInstance.AddNode(20, 20);

	return true;
}

bool M_FileSystem::CleanUp()
{

	FileSystem::FSDeInit();

//#ifndef STANDALONE
//	MeshLoader::DisableDebugMode();
//#endif // !STANDALONE

	return true;
}

void M_FileSystem::GetAllFiles(AssetDir& file, const char* directory)
{
	char** files = PHYSFS_enumerateFiles(directory);

	for (char** i = files; *i != nullptr; i++)
	{
		std::string str = std::string(directory) + std::string("/") + std::string(*i);
		std::string ext = "";
		FileSystem::SplitFilePath(str.c_str(), nullptr, nullptr, &ext);

		if (ext != "meta") 
		{
			AssetDir child = AssetDir(*i, str.c_str(), GetLastModTime(str.c_str()), (FileSystem::IsDirectory(str.c_str())) ?true :false);
			child.parentDir = &file;
			file.childDirs.push_back(child); //It's a file, TODO: ADD FILE PATH
		}
	}

	PHYSFS_freeList(files);
}

void M_FileSystem::GetAllFilesRecursive(AssetDir& _file)
{
	GetAllFiles(_file, _file.importPath.c_str());

	if (_file.childDirs.size() != 0)
	{
		for (unsigned int i = 0; i < _file.childDirs.size(); i++)
		{
			if (_file.childDirs[i].isDir) 
			{
				//_file.childDirs[i].dirName.push_back('/');
				GetAllFilesRecursive(_file.childDirs[i]);
			}
		}
	}
}

uint64 M_FileSystem::GetLastModTime(const char* filename)
{
	PHYSFS_Stat stat;
	PHYSFS_stat(filename, &stat);
	return stat.modtime;
}

std::string M_FileSystem::OpenFileSelectDialog()
{
	char filename[MAX_PATH];
	filename[0] = '\0';

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = ".des\0*.des*\0"; //Text Files\0*.txt\0 //I DONT KNOW HOW TO FILTER .META FILES AAAAAAAH
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select a File, yo!";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	
	if (GetOpenFileNameA(&ofn))
		return std::string(filename);

	return std::string("\0");
}

std::string M_FileSystem::OpenSaveAsDialog()
{
	return std::string("\0");
}

void M_FileSystem::ToLocalAssetsPath(std::string& inp)
{
	std::string localPath = FileSystem::NormalizePath(inp.c_str());
	inp.clear();

	size_t pos = 0;
	pos = localPath.find(ASSETS_PATH);
	if (pos != std::string::npos)
	{
		localPath = localPath.substr(pos);
		if (localPath.c_str() != "") {
			inp = localPath;
			return;
		}
	}

	inp = localPath;
	localPath.clear();
	return;
}

int M_FileSystem::DeleteAssetFile(const char* fileDir)
{
	if (fileDir == nullptr)
		return 0;

	return PHYSFS_delete(fileDir);
}

