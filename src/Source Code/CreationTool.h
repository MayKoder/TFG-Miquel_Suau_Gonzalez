#pragma once


class Tool
{
public:
	Tool(const char* _tName): toolName(_tName) {

	}
	virtual ~Tool() {

	}

	virtual void Use() {};
	const char* GetName() {
		return toolName;
	}

private:
	const char* toolName;

};

#include"Globals.h"
class ToolAddRemove : public Tool
{
public:
	ToolAddRemove(const char* _tName) : Tool(_tName) 
	{

	}
	virtual ~ToolAddRemove() {

	}

	void Use() override 
	{
		LOG(LogType::L_NORMAL, "Add/Remove node");
		EngineExternal->moduleRenderer3D->gridInstance.DivideHoveredClick();
	};

private:

};
