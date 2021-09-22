#pragma once

#include <string>
#include"ImGui/imgui.h"
#include"ImTextEditor/TextEditor.h"

struct LogMessage
{
	LogMessage(std::string&);
	bool EqualsStr(const char*);

	std::string msg;
	unsigned int prints;
};

class Window
{

public:
	Window();
	virtual ~Window();

	virtual void Draw();
	void AddLog(const char*);

	void SetTextFromFile(const char* path);

private:
	void DrawConsole();
	void DrawShaderEditor();

private:
	void SetErrorsOnScreen(const char* infoLog);
	void SplitErrors(const char* infoLog, std::vector<std::pair<int, std::string>>& error_list);
	std::string txtName;


public:
	std::string name;
	bool active;

	std::vector<LogMessage> logs;

private:

	TextEditor txtEditor;
	bool scrollToBottom;

};