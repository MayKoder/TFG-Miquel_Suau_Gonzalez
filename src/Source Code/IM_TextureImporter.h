#pragma once

#include<vector>
#include<string>
typedef unsigned int GLuint;
class Resource;
class DE_Cubemap;

namespace TextureImporter
{

	GLuint LoadToMemory(char* buffer, int size, int* w = nullptr, int* h = nullptr);

	void TakeScreenshot(int frameBuffer);

	void LoadCubeMap(std::vector<std::string>& faces, DE_Cubemap& cubeMap);
}