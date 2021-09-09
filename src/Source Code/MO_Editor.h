#ifndef STANDALONE
#pragma once

#include "Module.h"
#include "Globals.h"
#include "MA_IconSystem.h"
//#include "Window.h"

#define STYLES_PATH "Settings/styles.json"
#define MAX_STY_INPUT 15

typedef unsigned int ImGuiID;
enum class LogType;
class GameObject;
class ResourceTexture;
struct AssetDir;

class M_Editor : public Module
{

public:
	M_Editor(Application* app, bool start_enabled = true);
	virtual ~M_Editor();

	bool Init() override;
	bool Start() override;

	void Draw();
	void DrawMenuBar();
	//void DrawTopBar();
	//void CreateDockSpace();

	bool CleanUp() override;

	void DrawCreateMenu();

	void SaveStyle(const char* styleName);
	void ChangeStyleTo(const char* styleName);
	void DeleteStyle(const char* styleName);
	void UpdateLoadedStylesVector(std::vector<std::string>* _styles);

	IconManager editorIcons;

private:
	std::vector<std::string> styles;

	//WARNING TODO: This is bad find a better way
	char styleInput[MAX_STY_INPUT] = "";

	bool displayWindow;

	float viewportCorSize;
};

#endif