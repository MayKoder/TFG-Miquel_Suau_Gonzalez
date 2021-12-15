#include"Globals.h"

#include"MO_Scene.h"
#include"GameObject.h"
#include"CO_Transform.h"
#include "CreationTool.h"
#include"MO_Renderer3D.h"
#include"ImGui/imgui.h"
#include"CO_MeshRenderer.h"
#include"RE_Mesh.h"

void ToolNode::Use(int button_id)
{
	LOG("Add/Remove node");

	switch (button_id)
	{
	case 3:
		EngineExternal->moduleRenderer3D->gridInstance.DeleteHoveredNode();
		break;

	case 1:
		switch (typeIndex)
		{

		case 0:
			EngineExternal->moduleRenderer3D->gridInstance.CreateNode();
			break;

		case 1:
			EngineExternal->moduleRenderer3D->gridInstance.CreateNodesCircular(10);
			break;

		case 2:
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}
}

void ToolNode::DrawEditor()
{
	ImGui::Text("Type: ");
	ImGui::SameLine();
	ImGui::Combo("##tooltype", &typeIndex, data, 3);
}


void ToolWall::Use(int button_id)
{
	if (EngineExternal->moduleRenderer3D->gridInstance.CanBuildOnMouseNode() == false)
		return;

	GridNode* thisNode = EngineExternal->moduleRenderer3D->gridInstance.GetNodeAt_Slow(EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_X(), EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_Z());

	GameObject* wall = EngineExternal->moduleScene->CreateGameObject("Wall", EngineExternal->moduleScene->root);
	wall->transform->SetTransformMatrix(float3(EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_X(), 0, EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_Z()), Quat::identity, float3::one);
	wall->AddComponent(Component::Type::MeshRenderer);







	thisNode->go = wall;
	for (size_t i = 0; i < NODE_SIDES; i++)
	{
		if (thisNode->children[i] != nullptr && thisNode->children[i]->go != nullptr)
		{
			LOG("Side %i has a wall", i);

			//std::vector<int>
			C_MeshRenderer* childRend = dynamic_cast<C_MeshRenderer*>(thisNode->children[i]->go->GetComponent(Component::Type::MeshRenderer));


			std::vector<int> test;
			test.reserve(4);

			uint oppositeSide = GridManager::OPPOSITE_CHILDREN(static_cast<GridNode::Direction>(i));
			test.push_back(0 + (oppositeSide * 4));
			test.push_back(1 + (oppositeSide *4));
			test.push_back(2 + (oppositeSide *4));
			test.push_back(3 + (oppositeSide *4));

			childRend->_mesh->renderObject.RemoveTriangles(childRend->indices, test);

			childRend->_mesh->renderObject.Bind();
			childRend->_mesh->renderObject.LoadEBO(childRend->indices.data(), childRend->indices.size());
			childRend->_mesh->indices_count = childRend->indices.size();
			childRend->_mesh->renderObject.UnBind();

			//wall.
		}
	}

	//Check for nearby walls
		//If nearby wall and new use breaks the direction of the other wall
			//Add vertices to created wall
	//else 
		//create wall object with new vertices


	//Wall creation
		//create base points with determinated spacing (for a more advance weared wall)


}