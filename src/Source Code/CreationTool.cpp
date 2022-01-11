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

	int WALL_SIDES = 4;

	GridNode* mouseNode = EngineExternal->moduleRenderer3D->gridInstance.GetNodeAt_Slow(EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_X(), EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_Z());
	
	std::vector<GO_Wall*> wallGoAround;
	std::vector<int> sideToIgnore; 
	for (size_t i = 0; i < NODE_SIDES; i++)
	{
		if (mouseNode->children[i] != nullptr && mouseNode->children[i]->go != nullptr)
		{
			if (std::find(wallGoAround.begin(), wallGoAround.end(), dynamic_cast<GO_Wall*>(mouseNode->children[i]->go)) == wallGoAround.end()) {
				wallGoAround.push_back(dynamic_cast<GO_Wall*>(mouseNode->children[i]->go));
			}
			sideToIgnore.push_back(i);
		}
	}


	if (wallGoAround.size() == 0)
	{
		GO_Wall* wall = new GO_Wall("Wall", EngineExternal->moduleScene->root);
		WallNode ret = wall->InitWall(float3(EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_X(), 0, EngineExternal->moduleRenderer3D->gridInstance.GetMouseGridPos_Z()));
		mouseNode->go = wall;
		wall->trailNodes.push_back({ ret, mouseNode });

		LOG("New wall created");
	}
	else if (wallGoAround.size() == 1) {

		GO_Wall* originalWall = wallGoAround[0];
		C_MeshRenderer* render = originalWall->GetComponent<C_MeshRenderer>();
		
		WallNode retIndices= GO_Wall::GenerateWall(originalWall->subDivisions, mouseNode->GetGridPositionF3() - originalWall->transform->position, render->vertices, render->indices, &sideToIgnore);
		originalWall->trailNodes.push_back({retIndices, mouseNode });

		mouseNode->go = originalWall;
		int vertexPackJump = 6;


		for (size_t i = 0; i < sideToIgnore.size(); i++)
		{

			int side = GridManager::OPPOSITE_CHILDREN(static_cast<GridNode::Direction>(sideToIgnore[i]));

			int trioffset = 0;
			WallNode* nodeToRemoveFrom = nullptr;
			for (size_t j = 0; j < originalWall->trailNodes.size(); j++)
			{
				if (originalWall->trailNodes[j].second == mouseNode->children[sideToIgnore[i]]) {
					nodeToRemoveFrom = &originalWall->trailNodes[j].first;
					break;
				}
				else {
					trioffset += originalWall->trailNodes[j].first.GetTotalIndices();
				}
			}

			int indexJump = trioffset;
			for (size_t j = 0; j < WALL_SIDES; j++)
			{
				if (side == j)
				{
					break;
				}
				else 
				{
					indexJump += nodeToRemoveFrom->faceIndices[j];
				}
			}
			

			int vertexIDBeforeNode = render->vertices.size() - (((( (originalWall->subDivisions*2) * WALL_SIDES)+4) * vertexPackJump));
			vertexIDBeforeNode /= vertexPackJump;

			int offSetIndex = vertexIDBeforeNode;
			offSetIndex += sideToIgnore[i] * (originalWall->subDivisions*2);

			if (sideToIgnore[i] != WALL_SIDES - 1) {
				offSetIndex += (originalWall->subDivisions * 2);
			}
			else {
				offSetIndex = vertexIDBeforeNode;
			}



			std::vector<int> midIndexInsertion;
			midIndexInsertion.reserve(6 + ((originalWall->subDivisions-1) *6));

			////TOP
			int firstTopVertexIndex = (render->vertices.size() - (4 * vertexPackJump)) / vertexPackJump;
			int oldNodeTopVertex = render->indices[trioffset + nodeToRemoveFrom->GetTotalIndices() - 6];



			midIndexInsertion.push_back((sideToIgnore[i] == WALL_SIDES-1) ? firstTopVertexIndex : firstTopVertexIndex + sideToIgnore[i]+1);
			midIndexInsertion.push_back(firstTopVertexIndex + sideToIgnore[i]);
			midIndexInsertion.push_back(oldNodeTopVertex + side);

			midIndexInsertion.push_back(firstTopVertexIndex + sideToIgnore[i]);
			midIndexInsertion.push_back((side == WALL_SIDES-1) ? oldNodeTopVertex : oldNodeTopVertex + side+1);
			midIndexInsertion.push_back(oldNodeTopVertex + side);


			int originalStart = render->indices[indexJump + (0 * 6) + 0];
			int firstVertex = render->indices[trioffset];
			int newNormalVertex = (side != 0) ? render->indices[indexJump + (0 * 6) + 0] - originalWall->subDivisions : firstVertex + ((originalWall->subDivisions * 2) * WALL_SIDES) - originalWall->subDivisions;
			
			//RIGHT
			for (size_t x = 0; x < originalWall->subDivisions - 1; ++x)
			{
				int test = (firstTopVertexIndex - ((WALL_SIDES - sideToIgnore[i]) * (originalWall->subDivisions * 2)));
				if (sideToIgnore[i] != 0) 
				{
					test -= originalWall->subDivisions;
				}
				else 
				{
					test = firstTopVertexIndex - originalWall->subDivisions;
				}
				midIndexInsertion.push_back(test + x);
				int test2 = (side == 3) ? oldNodeTopVertex - (originalWall->subDivisions * 2 * WALL_SIDES) : oldNodeTopVertex - (originalWall->subDivisions * 2 * WALL_SIDES) + (originalWall->subDivisions * 2) + (side * originalWall->subDivisions * 2);
				midIndexInsertion.push_back(test2+x);
				midIndexInsertion.push_back(test+(x+1));

				midIndexInsertion.push_back(test2 + x);
				midIndexInsertion.push_back(test2 + x+1);
				midIndexInsertion.push_back(test + (x + 1));
			}

			for (size_t x = 0; x < originalWall->subDivisions - 1; ++x)
			{

				render->indices[indexJump + (x * 6) + 0] = newNormalVertex+x;
				render->indices[indexJump + (x*6)+1] = offSetIndex +x;
				render->indices[indexJump + (x*6)+2] = newNormalVertex+x+1;

				render->indices[indexJump + (x*6)+3] = offSetIndex +x;
				render->indices[indexJump + (x*6)+4] = offSetIndex + (x+1);
				render->indices[indexJump + (x*6)+5] = newNormalVertex+x+1;
			}

			nodeToRemoveFrom->faceIndices[side] += midIndexInsertion.size();
			
			render->indices.insert(render->indices.begin() + (indexJump + ((originalWall->subDivisions - 1) * 6)), midIndexInsertion.begin(), midIndexInsertion.end());
		}

		originalWall->UpdateWallGL();


		LOG("Node added to existing wall");
	}
	else if (wallGoAround.size() > 1)
	{
		//TODO: Merge multiple existing walls
		LOG("Multiple merge expected, good luck");
	}
}





//OLD: Old way to merge non-shared walls
//for (size_t i = 0; i < sideToIgnore.size(); i++)
//{
//
//	int side = GridManager::OPPOSITE_CHILDREN(static_cast<GridNode::Direction>(sideToIgnore[i]));
//
//	int trioffset = 0;
//	WallNode* nodeToRemoveFrom = nullptr;
//	for (size_t j = 0; j < originalWall->trailNodes.size(); j++)
//	{
//		if (originalWall->trailNodes[j].second == mouseNode->children[sideToIgnore[i]]) {
//			nodeToRemoveFrom = &originalWall->trailNodes[j].first;
//			break;
//		}
//		else {
//			trioffset += originalWall->trailNodes[j].first.GetTotalIndices();
//		}
//	}
//
//	int indexJump = trioffset;
//	for (size_t j = 0; j < NODE_SIDES; j++)
//	{
//		if (side == j)
//		{
//			//Todo this 6 is hardcoded
//			//nodeToRemoveFrom->faceIndices[j] -= 12;
//			break;
//		}
//		else
//		{
//			indexJump += nodeToRemoveFrom->faceIndices[j];
//		}
//	}
//
//
//	int vertexIDBeforeNode = render->vertices.size() - (((originalWall->subDivisions * NODE_SIDES) * 3));
//	vertexIDBeforeNode /= 3;
//
//	//int accNextJump = vertexIDBeforeNode;
//	//accNextJump += sideToIgnore[i] * originalWall->subDivisions;
//
//	int offSetIndex = vertexIDBeforeNode;
//	offSetIndex += sideToIgnore[i] * originalWall->subDivisions;
//	if (sideToIgnore[i] != NODE_SIDES - 1) {
//		offSetIndex += originalWall->subDivisions;
//	}
//	else {
//		offSetIndex = vertexIDBeforeNode;
//	}
//
//
//
//	std::vector<int> midIndexInsertion;
//	midIndexInsertion.reserve(6 + originalWall->subDivisions * NODE_SIDES);
//
//	//TOP
//	midIndexInsertion.push_back(render->indices[indexJump + 11]);
//	midIndexInsertion.push_back(offSetIndex + 2);
//	midIndexInsertion.push_back(vertexIDBeforeNode + sideToIgnore[i] * originalWall->subDivisions + 2);
//	midIndexInsertion.push_back(vertexIDBeforeNode + sideToIgnore[i] * originalWall->subDivisions + 2);
//	midIndexInsertion.push_back(render->indices[indexJump + 10]);
//	midIndexInsertion.push_back(render->indices[indexJump + 11]);
//
//	//RIGHT
//	for (size_t x = 0; x < originalWall->subDivisions - 1; ++x)
//	{
//		midIndexInsertion.push_back(vertexIDBeforeNode + sideToIgnore[i] * originalWall->subDivisions + x);
//		midIndexInsertion.push_back(render->indices[indexJump + ((x * 6) + 1)]);
//		midIndexInsertion.push_back(vertexIDBeforeNode + sideToIgnore[i] * originalWall->subDivisions + (x + 1));
//		midIndexInsertion.push_back(render->indices[indexJump + ((x * 6) + 1)]);
//		midIndexInsertion.push_back(render->indices[indexJump + ((x * 6) + 4)]);
//		midIndexInsertion.push_back(vertexIDBeforeNode + sideToIgnore[i] * originalWall->subDivisions + (x + 1));
//	}
//
//	for (size_t x = 0; x < originalWall->subDivisions - 1; ++x)
//	{
//		render->indices[indexJump + (x * 6) + 1] = offSetIndex + x;
//		render->indices[indexJump + (x * 6) + 3] = offSetIndex + x;
//		render->indices[indexJump + (x * 6) + 4] = offSetIndex + (x + 1);
//	}
//
//	nodeToRemoveFrom->faceIndices[side] += midIndexInsertion.size();
//
//	render->indices.insert(render->indices.begin() + (indexJump + 12), midIndexInsertion.begin(), midIndexInsertion.end());
//}