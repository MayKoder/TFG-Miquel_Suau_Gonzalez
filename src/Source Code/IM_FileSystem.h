#pragma once

#include "MO_Renderer3D.h"
#include <vector>

enum class ImportType {
	NOTYPE = -1,
	MESH,
	TEXTURE,
};

namespace StringLogic {
	std::string GlobalToLocalPath(const char* _globalPath);
}

#define ASSETS_PATH "Assets/"

namespace FileSystem
{
	ImportType GetTypeFromPath(const char* path);

	void FSInit();
	void FSDeInit();
	void GetFileName(const char* file, std::string& fileName, bool extension);
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) /*const*/;

	// Utility functions
	bool Exists(const char* file) /*const*/;
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file) /*const*/;
	bool AddPath(const char* path_or_zip);

	std::string NormalizePath(const char* path) /*const*/;
	std::string UnNormalizePath(const char* full_path);


	// Open for Read/Write
	unsigned int LoadToBuffer(const char* file, char** buffer) /*const*/;
	unsigned int Load(const char* path, const char* file, char** buffer) /*const*/;

	uint Save(const char* file, char* buffer, uint size, bool append);
	uint Copy(const char* file, const char* dir, std::string& outputFile);

	//std::string FileToText(const char* path);
}