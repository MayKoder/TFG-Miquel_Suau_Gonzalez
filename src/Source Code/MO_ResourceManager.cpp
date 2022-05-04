#include "MO_ResourceManager.h"
#include"Application.h"
#include"ImGui/imgui.h"

//Importers
#include"IM_FileSystem.h"
#include"IM_TextureImporter.h"
#include"IM_ShaderImporter.h"

#include"MO_Scene.h"
#include"Globals.h"

#include"RE_Texture.h"
#include"RE_Shader.h"

#include"DEJsonSupport.h"
#include"MO_Window.h"

M_ResourceManager::M_ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

M_ResourceManager::~M_ResourceManager()
{
}

bool M_ResourceManager::Init()
{
	return true;
}

#ifndef STANDALONE
bool M_ResourceManager::Start()
{

	return true;
}

update_status M_ResourceManager::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}
#endif // !STANDALONE

bool M_ResourceManager::CleanUp()
{
	return true;
}

#ifndef STANDALONE
void M_ResourceManager::OnGUI()
{
	//TODO: Move this to a new editor window
	if (ImGui::CollapsingHeader("Resource Display", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i resources loaded", resources.size());
		for (auto it = resources.begin(); it != resources.end(); ++it)
		{
			ImGui::Text("%i: %i and %s", (*it).second->GetReferenceCount(), (*it).second->GetUID(), (*it).second->GetAssetPath().c_str());
		}
	}
}
#endif // !STANDALONE

int M_ResourceManager::ExistsOnLibrary(const char* file_in_assets) const
{
	std::string metaFile(file_in_assets);
	metaFile += ".meta";

	if (FileSystem::Exists(metaFile.c_str())) 
	{
		JSON_Value* metaJSON = json_parse_file(metaFile.c_str());
		DEConfig rObj(json_value_get_object(metaJSON));

		std::string libPath = rObj.ReadString("Library Path");

		if (FileSystem::Exists(libPath.c_str()))
			return rObj.ReadInt("UID");

		//Free memory
		json_value_free(metaJSON);
	}


	return 0;
}

int M_ResourceManager::GenerateNewUID()
{
	return App->GetRandomInt();
}

Resource* M_ResourceManager::RequestResource(std::string assetPath, Resource::Type type)
{
	return RequestResource(assetPath);
}

//Returns a resource* if the resource is loaded or creates a new resource from the library file
Resource* M_ResourceManager::RequestResource(std::string& uid)
{
	//Find if the resource is already loaded
	if (uid.empty())
		return nullptr;

	std::map<std::string, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		it->second->IncreaseReferenceCount();
		return it->second;
	}

	//Find the library file (if exists) and load the custom file format
	if (!uid.empty())
	{
		Resource* ret = nullptr;

		static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 6, "Update all switches with new type");

		//Save check
		if (FileSystem::Exists(uid.c_str()))
		{
			//uid = 0; //This should be the uid from library
			switch (GetTypeFromAssetExtension(uid.c_str()))
			{
				case Resource::Type::TEXTURE: ret = (Resource*) new ResourceTexture(uid); break;
				case Resource::Type::SHADER: ret = dynamic_cast<Resource*>(new ResourceShader(uid)); break;
			}

			if (ret != nullptr)
			{
				resources[uid] = ret;
				ret->SetAssetsPath(uid.c_str());
				ret->IncreaseReferenceCount();

				ret->LoadToMemory();
			}
			LOG(  "Requested resource loaded as new"); //UNCOMMENT
		}
		else
			LOG( "Requested resource does not exist");

		return ret;
	}

	return nullptr;
	//return TryToLoadResource();
}

void M_ResourceManager::ReleaseResource(std::string& uid)
{
	std::map<std::string, Resource*>::iterator it = resources.find(uid);
	if (it == resources.end())
		return;
	
	Resource* res = (*it).second;
	(*it).second->UnloadFromMemory();
	resources.erase(uid);
	delete res;
}

void M_ResourceManager::LoadResource(std::string& uid)
{
	Resource* res = nullptr;

	std::map<std::string, Resource*>::iterator it = resources.find(uid);
	if (it == resources.end())
		return;

	res = it->second;
	res->IncreaseReferenceCount();

	//Load to memory [Buffers etcetc]
	res->LoadToMemory();

}

void M_ResourceManager::UnloadResource(std::string& uid)
{
	Resource* res = nullptr;
	
	std::map<std::string, Resource*>::iterator it = resources.find(uid);
	if (it == resources.end())
		return;
	
	res = it->second;
	res->DecreaseReferenceCount();

	if (res->GetReferenceCount() <= 0) 
		ReleaseResource(uid);

}

bool M_ResourceManager::IsResourceLoaded(std::string& uid)
{
	std::map<std::string, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return true;

	return false;
}

Resource::Type M_ResourceManager::GetTypeFromAssetExtension(const char* assetFile) const
{
	std::string ext(assetFile);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (size_t i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "fbx" || ext == "dae")
		return Resource::Type::MODEL;
	if (ext == "tga" || ext == "png" || ext == "jpg" || ext == "dds")
		return Resource::Type::TEXTURE;
	if (ext == "des")
		return Resource::Type::SCENE;
	//if (ext == "cs")
	//	return Resource::Type::SCRIPT;

	if (ext == "glsl")
		return Resource::Type::SHADER;
	if (ext == "mat")
		return Resource::Type::MATERIAL;


	return Resource::Type::UNKNOWN;
}