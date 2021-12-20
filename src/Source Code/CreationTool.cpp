#include"Globals.h"

#include"MO_Scene.h"
#include"GameObject.h"
#include"CO_Transform.h"
#include "CreationTool.h"
#include"MO_Renderer3D.h"
#include"ImGui/imgui.h"
#include"CO_MeshRenderer.h"
#include"RE_Mesh.h"

#include"GO_Wall.h"

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
	
	std::vector<GO_Wall*> storedSideIndex;
	std::vector<int> sideToIgnore; 
	for (size_t i = 0; i < NODE_SIDES; i++)
	{
		if (thisNode->children[i] != nullptr && thisNode->children[i]->go != nullptr)
		{
			storedSideIndex.push_back(dynamic_cast<GO_Wall*>(thisNode->children[i]->go));
			sideToIgnore.push_back(i);
		}
	}


	if (storedSideIndex.size() == 0)
	{
		GO_Wall* wall = new GO_Wall("Wall", EngineExternal->moduleScene->root);
		int ret = wall->InitWall(float3(EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_X(), 0, EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_Z()));
		thisNode->go = wall;
		wall->trailNodes.push_back({ ret, thisNode });

		LOG("New wall created");
	}
	else if (storedSideIndex.size() == 1) {
		//Add node to existing wall

		GO_Wall* originalWall = storedSideIndex[0];
		C_MeshRenderer* render = originalWall->GetComponent<C_MeshRenderer>();



		
		int retIndices= GO_Wall::GenerateWall(originalWall->subDivisions, thisNode->GetGridPositionF3() - originalWall->transform->position, render->vertices, render->indices, &sideToIgnore);
		originalWall->trailNodes.push_back({retIndices, thisNode });

		thisNode->go = originalWall;


		for (size_t k = 0; k < sideToIgnore.size(); k++)
		{

			std::vector<int> trianglesToRemove;
			int trioffset = 0;
			int removedFromTrailIndex = 0;
			for (size_t j = 0; j < originalWall->trailNodes.size(); j++)
			{
				if (originalWall->trailNodes[j].second == thisNode->children[sideToIgnore[k]]) {
					removedFromTrailIndex = j;
					break;
				}
				else {
					trioffset += originalWall->trailNodes[j].first;
				}
			}

			int firstTri = (trioffset / 3);

			//TODO: This for is the current problem, we won't always have 4 sides, we need to know how many sides our node has
			trianglesToRemove.push_back(firstTri + (4* GridManager::OPPOSITE_CHILDREN( static_cast<GridNode::Direction>(sideToIgnore[k]))));
			trianglesToRemove.push_back(firstTri + (4* GridManager::OPPOSITE_CHILDREN( static_cast<GridNode::Direction>(sideToIgnore[k]))) + 1);
			trianglesToRemove.push_back(firstTri + (4* GridManager::OPPOSITE_CHILDREN( static_cast<GridNode::Direction>(sideToIgnore[k]))) + 2);
			trianglesToRemove.push_back(firstTri + (4* GridManager::OPPOSITE_CHILDREN( static_cast<GridNode::Direction>(sideToIgnore[k]))) + 3);

			//TODO: Not working, We need to update the node's indices value inside the trailNoedes pair after every removeTriangles
			originalWall->trailNodes[removedFromTrailIndex].first -= trianglesToRemove.size() * 3;
			render->_mesh->renderObject.RemoveTriangles(render->indices, trianglesToRemove);
		}

		originalWall->UpdateWallGL();
		//dynamic_cast<GO_Wall*>(thisNode->children[storedSideIndex[0]]->go)->UpdateWallGL();


		LOG("Node added to existing wall");
	}
	else if (storedSideIndex.size() > 1)
	{
		//TODO: Merge multiple existing walls
		LOG("Multiple merge expected, good luck");
	}




	//for (size_t i = 0; i < NODE_SIDES; i++)
	//{
	//	if (thisNode->children[i] != nullptr && thisNode->children[i]->go != nullptr)
	//	{
	//		LOG("Side %i has a wall", i);

	//		//std::vector<int>
	//		C_MeshRenderer* childRend = thisNode->children[i]->go->GetComponent<C_MeshRenderer>();


	//		std::vector<int> test;
	//		test.reserve(4);

	//		uint oppositeSide = GridManager::OPPOSITE_CHILDREN(static_cast<GridNode::Direction>(i));
	//		test.push_back(0 + (oppositeSide * 4));
	//		test.push_back(1 + (oppositeSide *4));
	//		test.push_back(2 + (oppositeSide *4));
	//		test.push_back(3 + (oppositeSide *4));

	//		childRend->_mesh->renderObject.RemoveTriangles(childRend->indices, test);

	//		childRend->_mesh->renderObject.Bind();
	//		childRend->_mesh->renderObject.LoadEBO(childRend->indices.data(), childRend->indices.size());
	//		childRend->_mesh->indices_count = childRend->indices.size();
	//		childRend->_mesh->renderObject.UnBind();

	//		//wall.
	//	}
	//}

	//Check for nearby walls
		//If nearby wall and new use breaks the direction of the other wall
			//Add vertices to created wall
	//else 
		//create wall object with new vertices


	//Wall creation
		//create base points with determinated spacing (for a more advance weared wall)


}