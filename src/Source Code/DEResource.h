#pragma once

#include<string>

class Resource
{

public:

	//REMEBER: To update all the switch methods on Resource Manager module when adding a new type
	enum class Type {
		TEXTURE,
		MODEL,
		MESH,
		SCENE,
		//SCRIPT,
		SHADER,
		MATERIAL,
		UNKNOWN
	};

public:
	Resource(std::string& _uid, Resource::Type _type);
	virtual ~Resource();

	inline Resource::Type GetType() const { return type; }
	inline const char* GetUID() const { return uid.c_str(); }
	inline std::string GetAssetPath() const { return assetsFile; }

	void SetAssetsPath(const char*);

	//inline bool IsLoadedToMemory() const { return (referenceCount >= 0) ? true : false; }

	inline unsigned int GetReferenceCount() const { return referenceCount; }
	inline void IncreaseReferenceCount() { referenceCount++; }
	inline void DecreaseReferenceCount() { referenceCount--; }

	//virtual void Save(Config& config) const;
	//virtual void Load(const Config& config);

	//Can't be pure virtual "=0" until there is a resoruce model class
	virtual bool LoadToMemory() { return false; }
	virtual bool UnloadFromMemory() { return false; }

protected:

	std::string uid;
	std::string assetsFile;
	Type type = Type::UNKNOWN;
	unsigned int referenceCount;
};