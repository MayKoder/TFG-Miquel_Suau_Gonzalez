#pragma once


class Tool
{
public:
	Tool(const char* _tName): toolName(_tName) {

	}
	virtual ~Tool() {

	}

	virtual void Use(int button_id) = 0;
	virtual void DrawEditor() = 0;

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

	void Use(int button_id) override 
	{
		LOG(LogType::L_NORMAL, "Add/Remove node");

		switch (button_id)
		{
		case 3:
			EngineExternal->moduleRenderer3D->gridInstance.DeleteHoveredNode();
			break;

		case 1:
			EngineExternal->moduleRenderer3D->gridInstance.CreateNode();
			break;

		default:
			break;
		}
	};

	void DrawEditor() override
	{
		for (size_t i = 0; i < 150; i++)
		{
			ImGui::Text("Holaaa");
		}
	};

private:

};
