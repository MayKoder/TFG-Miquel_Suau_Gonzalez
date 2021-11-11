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
class ToolNode : public Tool
{
public:
	ToolNode(const char* _tName) : Tool(_tName), typeIndex(0)
	{

	}
	virtual ~ToolNode() {

	}

	void Use(int button_id) override 
	{
		LOG(  "Add/Remove node");

		switch (button_id)
		{
		case 3:
			EngineExternal->moduleRenderer3D->gridInstance.DeleteHoveredNode();
			break;

		case 1:
			//EngineExternal->moduleRenderer3D->gridInstance.CreateNode();
			EngineExternal->moduleRenderer3D->gridInstance.CreateNodesCircular(10);
			break;

		default:
			break;
		}
	};

	void DrawEditor() override
	{
		ImGui::Text("Type: ");
		ImGui::SameLine();
		ImGui::Combo("##tooltype", &typeIndex, data, 3);
	};

private:

	int typeIndex;
	const char* data[3] = { "Single", "Drag", "Line" };
	

};

#include"MO_Scene.h"
#include"GameObject.h"
#include"CO_Transform.h"
class  ToolWall : public Tool
{
public:
	ToolWall(const char* _tName) : Tool(_tName)
	{

	}
	virtual ~ToolWall() {

	}

	void Use(int button_id) override
	{
		if (EngineExternal->moduleRenderer3D->gridInstance.CanBuildOnMouseNode() == false)
			return;


		GameObject* wall = EngineExternal->moduleScene->CreateGameObject("Wall", EngineExternal->moduleScene->root);
		wall->transform->SetTransformMatrix(float3(EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_X(), 0, EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_Z()), Quat::identity, float3::one);
		wall->AddComponent(Component::Type::MeshRenderer);

		EngineExternal->moduleRenderer3D->gridInstance.GetNodeAt_Slow(EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_X(), EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_Z())->go = wall;
		//Check for nearby walls
			//If nearby wall and new use breaks the direction of the other wall
				//Add vertices to created wall
		//else 
			//create wall object with new vertices


		//Wall creation
			//create base points with determinated spacing (for a more advance weared wall)


	};

	void DrawEditor() override
	{

	};

private:

};
