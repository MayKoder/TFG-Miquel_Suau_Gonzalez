#pragma once

#include "Module.h"
#include<map>

#include"MO_FileSystem.h"
#include"DEResource.h"

class M_ResourceManager : public Module
{

public:
	M_ResourceManager(Application* app, bool start_enabled = true);
	virtual ~M_ResourceManager();

	bool Init() override;

#ifndef STANDALONE
	bool Start() override;
	update_status PreUpdate(float dt) override;
#endif // !STANDALONE

	bool CleanUp() override;

#ifndef STANDALONE
	void OnGUI() override;
#endif // !STANDALONE


	int GenerateNewUID();
	int ExistsOnLibrary(const char* file_in_assets) const;	

	void UnloadResource(std::string&);

	Resource* RequestResource(std::string, Resource::Type type);
	Resource* RequestResource(std::string&);
	
	Resource::Type GetTypeFromAssetExtension(const char* assetFile) const;

private:
	void LoadResource(std::string&);
	void ReleaseResource(std::string& uid);

	bool IsResourceLoaded(std::string&);


private:
	std::map<std::string, Resource*> resources;
};