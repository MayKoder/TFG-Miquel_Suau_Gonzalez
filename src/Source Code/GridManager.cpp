
#include "Globals.h"
#include "GridManager.h"
#include "RE_Shader.h"

#include "Application.h"

#include "MO_ResourceManager.h"
#include "MO_Renderer3D.h"
#include "MO_Input.h"
#include "MO_GUI.h"

#include "CO_Camera.h"

#include <iostream>
//#include <chrono>
//typedef std::chrono::high_resolution_clock Clock;

#include"MathGeoLib/include/Geometry/Plane.h"
#include"MathGeoLib/include/Math/float4x4.h"
#include"MO_Window.h"

#include "CreationTool.h"


GridManager::GridManager() : shaderRes(nullptr), /*VBO(0), instanceVBO(0), VAO(0),*/ hoveredNode(nullptr)
{
	//shaderRes = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(EngineExternal->GetRandomInt(), "Library/Shaders/1554189485.shdr"));
	memset(cursorGridPos, 0, sizeof(cursorGridPos));

	//nodeMap[GridManager::CANTOR_MAPPING(0, 0)] = GridNode(0, 0);
	//linealNodes.push_back(&baseNode);

	//nodeMap[GridManager::CANTOR_MAPPING(0, 0)].DivideNodeSquare(this, 50);

//#pragma region Expansion Test
//
//
//	GridNode* node = &baseNode;
//	baseNode.DivideNodeCross(this);
//	for (size_t i = 0; i < NODE_SIDES; i++)
//	{
//
//		node = node->children[i];
//		for (size_t j = 0; j < 10; j++)
//		{
//			node->DivideNodeCross(this);
//			node = node->children[i];
//		}
//
//		//GridNode* right;
//		//GridNode* left;
//
//		//left = node->children[GridNode::Direction::LEFT];
//		//right = node->children[GridNode::Direction::RIGHT];
//
//		//for (size_t i = 0; i < 10/2; i++)
//		//{
//		//	right->DivideNode(this);
//		//	left->DivideNode(this);
//		//}
//
//		node = &baseNode;
//
//	}
//
//
//#pragma endregion



}

GridManager::~GridManager()
{
	nodeMap.clear();
}

void GridManager::UpdateInput(Tool* selectedTool)
{

	float2 position = float2(static_cast<float>(EngineExternal->moduleInput->GetMouseX()), static_cast<float>(EngineExternal->moduleInput->GetMouseY()));

	float3 normal = float3::zero;
	normal.x = (position.x - 0.0) / ((0.0 + EngineExternal->moduleWindow->s_width) - 0.0);
	normal.y = (position.y - 0.0) / ((0.0 + EngineExternal->moduleWindow->s_height) - 0.0);

	normal.x = (normal.x - 0.5f) / 0.5f;
	normal.y = -((normal.y - 0.5f) / 0.5f);

	LineSegment worldRay = EngineExternal->moduleRenderer3D->activeRenderCamera->ScreenToWorld(normal.x, normal.y);


	//----
	Plane up = Plane(float3(0, 1, 0), 0.0f);

	float3 ba = worldRay.b - worldRay.a;
	float nDotA = Dot(up.normal, worldRay.a);
	float nDotBA = Dot(up.normal, ba);

	float3 ret = worldRay.a + (((up.d - nDotA) / nDotBA) * ba);
	//----
	//if (cursorGridPos[0] != static_cast<int>(rint(ret.x)) || cursorGridPos[1] != static_cast<int>(rint(ret.z)))
	//{
	//	this->testRender.AddPoint(float3(static_cast<int>(rint(ret.x)), 0.15f, static_cast<int>(rint(ret.z))));
	//}
	cursorGridPos[0] = static_cast<int>(rint(ret.x));
	cursorGridPos[1] = static_cast<int>(rint(ret.z));

	hoveredNode = this->GetNodeAt_Slow(cursorGridPos[0], cursorGridPos[1]);


	if (selectedTool != nullptr) 
	{
		//TODO: What a dumb idea, the tool should check the button?
		for (size_t i = 0; i < MAX_MOUSE_BUTTONS; i++)
		{
			if (EngineExternal->moduleInput->GetMouseLayer() != MOUSE_LAYER::HOVERING_UI && EngineExternal->moduleInput->GetMouseButton(i) == KEY_STATE::KEY_DOWN) {
				selectedTool->Use(i);
				EngineExternal->moduleInput->SetMouseLayer(MOUSE_LAYER::USING_TOOL);
			}
			if (EngineExternal->moduleInput->GetMouseLayer() != MOUSE_LAYER::HOVERING_UI && EngineExternal->moduleInput->GetMouseButton(i) == KEY_STATE::KEY_REPEAT) {
				selectedTool->UseRepeat(i);
				EngineExternal->moduleInput->SetMouseLayer(MOUSE_LAYER::USING_TOOL);
			}
		}
	}
}

void GridManager::DivideHoveredClick()
{
	if (hoveredNode != nullptr)
	{
		hoveredNode->DivideNodeCross(this);

		//UpdateRenderData();
	}
}

void GridManager::CreateNode()
{
	if (hoveredNode == nullptr)
	{
		AddNode(this->cursorGridPos[0], this->cursorGridPos[1]);

		//UpdateRenderData();
	}
	else {
		DivideHoveredClick();
	}
}

void GridManager::CreateNodesCircular(int radius)
{
	int cx = this->cursorGridPos[0];
	int cy = this->cursorGridPos[1];

	float radChk = (float)radius * (float)radius;

	for (int i = (cx - radius); i <= (cx + radius); ++i)
	{
		for (int j = (cy - radius); j <= (cy + radius); ++j)
		{
			if (pow(i - cx, 2) + pow(j - cy, 2) < radChk)
			{
				int cantID = CANTOR_MAPPING(i, j);
				if (nodeMap.count(cantID) == 0)
				{
					AddNode(i, j);
				}
			}
		}
	}
}

bool GridManager::DeleteHoveredNode()
{
	if (hoveredNode == nullptr)
		return false;

	for (size_t i = 0; i < NODE_SIDES; i++)
	{
		GridNode* target = hoveredNode->children[i];
		if (target != nullptr) {
			for (size_t j = 0; j < NODE_SIDES; ++j)
			{
				if (target->children[j] == hoveredNode)
					target->children[j] = nullptr;
			}
		}
	}

	std::map<uint, GridNode>::iterator it;
	for (it = nodeMap.begin(); it != nodeMap.end(); it++)
	{
		if (it->second.indicesIndexTmp > hoveredNode->indicesIndexTmp) {
			it->second.indicesIndexTmp -= 6;
		}
	}
	



	//std::vector<float>::iterator it = gridMeshVertices.begin() + hoveredNode->meshIndexTmp;
	//for (size_t i = hoveredNode->meshIndexTmp; i < hoveredNode->meshIndexTmp+18; i++)
	//{
	//	it = gridMeshVertices.erase(it);
	//}

	//Iterate node map
	// 	   Move every node with indicesIndexTmp > hoveredNode->indicesIndexTmp -4 as value [DONE]
	// 	   Remove vertices using indices iterator  [DONE?]
	// 	   Move indices values back 6 positions from the deleted one
	// 	   Remove indices from index list

	auto uniqueVertices = hoveredNode->GetUniqueVertices();
	gridMeshObject.RemoveVertices(gridMeshVertices, gridMeshIndices, uniqueVertices, hoveredNode->indicesIndexTmp);

	//Move indices indicator back 6 positions, this should be valid with the new system//
	//std::map<uint, GridNode>::iterator it;
	//for (it = nodeMap.begin(); it != nodeMap.end(); it++)
	//{
	//	if (it->second.indicesIndexTmp > hoveredNode->indicesIndexTmp) {
	//		it->second.indicesIndexTmp -= 6;
	//	}
	//}

	////Remove unique vertices from vertex vector, not valid with new system [TESTING NEW VERSION]//
	//auto uniqueVertices = hoveredNode->GetUniqueVertices();
	//std::vector<int> deletedVerticesID;
	//for (size_t i = 0; i < uniqueVertices.size(); i += 3)
	//{
	//	int indicator = GetVertexIndex(float3(uniqueVertices[i], uniqueVertices[i + 1], uniqueVertices[i + 2]));
	//	deletedVerticesID.push_back(indicator);
	//}
	//for (size_t i = 0; i < uniqueVertices.size(); i += 3)
	//{
	//	int indicator = GetVertexIndex(float3(uniqueVertices[i], uniqueVertices[i+1], uniqueVertices[i + 2]));

	//	auto first = gridMeshVertices.cbegin() + (indicator * 3);
	//	auto last = gridMeshVertices.cbegin() + ((indicator * 3) + 3);
	//	gridMeshVertices.erase(first, last);
	//}

	////Delete indices from index vector, this is still valid//
	//auto indFirts = gridMeshIndices.cbegin() + hoveredNode->indicesIndexTmp;
	//auto indLast = gridMeshIndices.cbegin() + (hoveredNode->indicesIndexTmp + 6);
	//gridMeshIndices.erase(indFirts, indLast);

	////Change index value to adapt to deleted vertices, this is not valid with the new system
	////for (size_t i = hoveredNode->indicesIndexTmp; i < gridMeshIndices.size(); ++i)
	////{
	////	gridMeshIndices[i] -= 4;
	////}

	//std::sort(deletedVerticesID.begin(), deletedVerticesID.end());
	//for (size_t i = 0; i < deletedVerticesID.size(); ++i)
	//{

	//	for (size_t j = 0; j < gridMeshIndices.size(); ++j)
	//	{
	//		if (gridMeshIndices[j] >= deletedVerticesID[i])
	//		{
	//			gridMeshIndices[j]--;

	//		}
	//	}
	//	for (size_t k = 0; k < deletedVerticesID.size(); ++k)
	//	{
	//		deletedVerticesID[k]--;
	//	}


	//}



	//LOG("Unique vertices %d", gridMeshVertices.size() / 3);
	//

	//gridMeshObject.Bind();
	//gridMeshObject.SetVBO(0, gridMeshVertices.data(), gridMeshVertices.size(), GL_DYNAMIC_DRAW);
	//gridMeshObject.LoadEBO(gridMeshIndices.data(), gridMeshIndices.size());
	//gridMeshObject.UnBind();

	nodeMap.erase(hoveredNode->GetID());
	//UpdateRenderData();

	return true;
}

void GridManager::LoadShader(const char* path)
{
	shaderRes = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(1554189485, path));
	meshGridShader = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(1990536996, "Library/Shaders/1990536996.shdr"));

	//----       Grid mesh object init    ----//
	gridMeshObject.InitBuffers();
	gridMeshObject.Bind();

	//float initData[] = {
	//	-0.5f, 0.0, -0.5f,
	//	-0.5f, 0.0, +0.5f,
	//	+0.5f, 0.0, +0.5f,


	//	-0.5f, 0.0, -0.5f,
	//	+0.5f, 0.0, +0.5f,
	//	+0.5f, 0.0, -0.5f,
	//};
	//gridMeshObject.CreateAndSetVBO(initData, sizeof(initData) / sizeof(float));

	gridMeshObject.CreateVBO();
	AddNode(0, 0, false);

	gridMeshObject.SetVertexAttrib(0, 3, 3 * sizeof(float), 0 * sizeof(float), GL_FLOAT);

	gridMeshObject.UnBind();

	//---- Grid mesh object init finished ----//



	float data[] = {
	-0.4, 0.0, -0.4,
	-0.4, 0.0, 0.4,
	0.4, 0.0, 0.4,
	0.4, 0.0, -0.4,
	-0.4, 0.0, -0.4,
	};


	gridGLObject.InitBuffers();
	gridGLObject.Bind();

	gridGLObject.CreateAndSetVBO(data, 15);
	gridGLObject.SetVertexAttrib(0, 3, 3 * sizeof(float), 0 * sizeof(float), GL_FLOAT);

	gridGLObject.UnBind();
}

//void GridManager::UpdateRenderData(bool unBindAfter)
//{
//	std::vector<int> instanceData;
//	instanceData.reserve(nodeMap.size() * 2);
//
//	std::map<uint, GridNode>::iterator it;
//	for (it = nodeMap.begin(); it != nodeMap.end(); it++)
//	{
//		instanceData.push_back(it->second.GetGridPositionX());
//		instanceData.push_back(it->second.GetGridPositionY());
//	}
//	instanceData.shrink_to_fit();
//
//
//	//glBindVertexArray(VAO);
//	gridGLObject.Bind();
//
//	//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
//	//glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(float), instanceData.data(), GL_DYNAMIC_DRAW);
//	gridGLObject.SetVBO(1, instanceData.data(), instanceData.size(), GL_DYNAMIC_DRAW);
//
//	if (unBindAfter == true) {
//		gridGLObject.UnBind();
//	}
//}

void GridManager::ClearMemory()
{
	EngineExternal->moduleResources->UnloadResource(shaderRes->GetUID());

	//glDeleteVertexArrays(1, &VAO);
	//VAO = 0u;
	//glDeleteBuffers(1, &VBO);
	//VBO = 0u;

	//glDeleteBuffers(1, &instanceVBO);
	//instanceVBO = 0u;

	gridGLObject.UnloadBuffers();
	gridMeshObject.UnloadBuffers();
}

//GridNode* GridManager::GetGridNode(int x, int y) {
//	return &grid[(y * GRID_SIZE_X) + x];
//}

void GridManager::RenderGridTemporal()
{
	//auto t1 = Clock::now();

	shaderRes->Bind();
	EngineExternal->moduleRenderer3D->activeRenderCamera->PushCameraShaderVars(shaderRes->shaderProgramID);

	GLint modelLoc = glGetUniformLocation(shaderRes->shaderProgramID, "cPosition");
	glUniform2f(modelLoc, cursorGridPos[0], cursorGridPos[1]);

	gridGLObject.RenderAsArray(GL_LINE_STRIP, 0, 5);
	shaderRes->Unbind();


	//Render grid mesh
	meshGridShader->Bind();
	EngineExternal->moduleRenderer3D->activeRenderCamera->PushCameraShaderVars(meshGridShader->shaderProgramID);

	modelLoc = glGetUniformLocation(meshGridShader->shaderProgramID, "position");
	glUniform3f(modelLoc, 0.0f, 0.0f, 0.0f);

	modelLoc = glGetUniformLocation(meshGridShader->shaderProgramID, "color");
	glUniform4f(modelLoc, 0.8, 0.8, 0.8, 1);

	gridMeshObject.RenderAsIndices(GL_TRIANGLES, gridMeshIndices.size(), GL_UNSIGNED_INT);

	meshGridShader->Unbind();



	//LineRenderer lRender;
	// 
	///*ERROR: Loading a shader every frame? Lmao u dumb*/
	//lRender.InitLineRenderer();

	//lRender.AddPoint(float3(0, 0, 0));
	//lRender.AddPoint(float3(1, 0, 0));

	//lRender.Render();

	//if(hoveredNode != nullptr)
	//	LOG("%i, %i", hoveredNode->GetGridPositionX(), hoveredNode->GetGridPositionY());

	//TODO: Only happen when a creation tool is enabled
	if (EngineExternal->moduleGUI->selectedTool != nullptr) 
	{
		glColor3f(1., 0.f, 0.f);
		glBegin(GL_LINES);

		float3 position = float3(static_cast<float>(cursorGridPos[0]), 0.0, static_cast<float>(cursorGridPos[1]));
		float crossSize = 0.03;

		glVertex3f(position.x - crossSize, 0.0, position.z + crossSize);
		glVertex3f(position.x + crossSize, 0.0, position.z - crossSize);

		glVertex3f(position.x + crossSize, 0.0, position.z + crossSize);
		glVertex3f(position.x - crossSize, 0.0, position.z - crossSize);

		glEnd();
		glColor3f(1., 1.f, 1.f);
	}
	
	//testRender.Render();

	float data[] = {
		-0.5f, 0.0f, -0.5f,
		-0.5f, 0.0f, +0.5f,
		+0.5f, 0.0f, +0.5f,

		-0.5f, 0.0f, -0.5f,
		+0.5f, 0.0f, +0.5f,
		+0.5f, 0.0f, -0.5f,
	};
	/*std::vector<float> */


	//std::vector<float3> vertices;
	//vertices.reserve(nodeMap.size() * 4);

	//glBegin(GL_TRIANGLES);
	//for (it = nodeMap.begin(); it != nodeMap.end(); it++)
	//{

	//	int* p0 = it->second.GetGridPosition();

	//	vertices.push_back(float3(p0[0] - 0.5f, 0.0, p0[1] - 0.5f));
	//	vertices.push_back(float3(p0[0] + 0.5f, 0.0, p0[1] - 0.5f));
	//	vertices.push_back(float3(p0[0] + 0.5f, 0.0, p0[1] + 0.5f));
	//	vertices.push_back(float3(p0[0] - 0.5f, 0.0, p0[1] + 0.5f));

	//	glVertex3f(p0[0] - 0.5f, 0.0, p0[1] - 0.5f);
	//	glVertex3f(p0[0] - 0.5f, 0.0, p0[1] + 0.5f);
	//	glVertex3f(p0[0] + 0.5f, 0.0, p0[1] + 0.5f);


	//	glVertex3f(p0[0] - 0.5f, 0.0, p0[1] - 0.5f);
	//	glVertex3f(p0[0] + 0.5f, 0.0, p0[1] + 0.5f);
	//	glVertex3f(p0[0] + 0.5f, 0.0, p0[1] - 0.5f);

	//}
	//glEnd();


	//std::unique(vertices.begin(), vertices.end());
	//LOG("A");


	//auto t2 = Clock::now();
	//LOG(  "Rendering took: %dms should be like 7 at max", std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());

	//glLineWidth(1.0f);

	//glColor3f(0.5, 0.5f, 0.5f);
	//glBegin(GL_LINES);

	//for (size_t x = 0; x <= GRID_SIZE_X; x++)
	//{
	//	glVertex3f(x, 0.0f, 0.0f);
	//	glVertex3f(x, 0.0f, GRID_SIZE_Y);
	//}

	//for (size_t y = 0; y <= GRID_SIZE_Y; y++)
	//{
	//	glVertex3f(0.0f, 0.0f, y);
	//	glVertex3f(GRID_SIZE_X, 0.0f, y);
	//}

	//glEnd();

	//glPointSize(10.0f);
	//glColor3f(1., 0.f, 0.f);
	//glBegin(GL_POINTS);
	//glVertex3f(0, 0, 0);
	//glEnd();
	//glPointSize(1.0f);

	//glColor3f(1.f, 1.f, 1.f);
}

GridNode* GridManager::GetNodeAt_Slow(int x, int y)
{
	//auto t1 = Clock::now();

	//code here
	//Slow picking division took 38536ms
	//for (auto it = linealNodes.begin(); it != linealNodes.end(); ++it)
	//{
	//	if ((*it)->IsPosition(x, y)) 
	//	{
	//		return (*it);
	//	}
	//}
	//return nullptr;

	//Slow picking division took 103ms
	if (nodeMap.find(GridManager::CANTOR_MAPPING(x, y)) != nodeMap.end()) {
		return &nodeMap[GridManager::CANTOR_MAPPING(x, y)];
	}
	return nullptr;

	//auto t2 = Clock::now();
	//std::cout << "Delta t2-t1: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " nanoseconds" << std::endl;
}

bool GridManager::CanBuildOnMouseNode()
{
	GridNode* gn = GetNodeAt_Slow(this->cursorGridPos[0], this->cursorGridPos[1]);
	return gn != nullptr && gn->go == nullptr;
}

/*TODO: If we want to add a lot of nodes at once (ex: loading a scene) we should wait until the last node is created to upload the data to the GPU
to avoid useless calls*/
GridNode* GridManager::AddNode(int x, int y, bool unBind)
{
	uint cantor = CANTOR_MAPPING(x, y);
	nodeMap[cantor] = GridNode(x, y);

	GridNode* val = &nodeMap[cantor];

	val->SearchAndFillChildren(this);


	auto uniqueVector = val->GetUniqueVertices();
	gridMeshVertices.insert(gridMeshVertices.end(), uniqueVector.begin(), uniqueVector.end());


	val->indicesIndexTmp = gridMeshIndices.size();

	int indices[4] = {
		gridMeshObject.FloatArrayToIndex(gridMeshVertices, float3(-0.5+x, 0.0, -0.5+y)), 
		gridMeshObject.FloatArrayToIndex(gridMeshVertices, float3(-0.5 + x, 0.0, +0.5 + y)),
		gridMeshObject.FloatArrayToIndex(gridMeshVertices, float3(+0.5 + x, 0.0, +0.5 + y)),
		gridMeshObject.FloatArrayToIndex(gridMeshVertices, float3(+0.5 + x, 0.0, -0.5 + y)),
	};

	gridMeshIndices.push_back(indices[0]);
	gridMeshIndices.push_back(indices[1]);
	gridMeshIndices.push_back(indices[2]);
	gridMeshIndices.push_back(indices[0]);
	gridMeshIndices.push_back(indices[2]);
	gridMeshIndices.push_back(indices[3]);

	LOG("Unique vertices %d", gridMeshVertices.size() / 3);

	gridMeshObject.Bind();
	gridMeshObject.SetVBO(0, gridMeshVertices.data(), gridMeshVertices.size(), GL_DYNAMIC_DRAW);
	gridMeshObject.LoadEBO(gridMeshIndices.data(), gridMeshIndices.size());

	if (unBind == true) {
		gridMeshObject.UnBind();
	}

	return val;
}

GridNode::GridNode() : indicesIndexTmp(0), go(nullptr)
{
	memset(children, NULL, sizeof(children));
	memset(gridPosition, 0.0, sizeof(gridPosition));
}

GridNode::GridNode(int x, int y) : indicesIndexTmp(0), go(nullptr)
{
	memset(children, NULL, sizeof(children));

	gridPosition[0] = x;
	gridPosition[1] = y;
}

void GridNode::SearchAndFillChildren(GridManager* instance)
{
	int x = this->GetGridPositionX();
	int y = this->GetGridPositionY();

	if (children[RIGHT] == nullptr) {
		children[RIGHT] = instance->GetNodeAt_Slow(x + 1, y);

		if (children[RIGHT]) {
			children[RIGHT]->SearchAndFillChildren(instance);
		}
	}

	if (children[DOWN] == nullptr) {
		children[DOWN] = instance->GetNodeAt_Slow(x, y - 1);
		if (children[DOWN]) {
			children[DOWN]->SearchAndFillChildren(instance);
		}
	}

	if (children[LEFT] == nullptr) {
		children[LEFT] = instance->GetNodeAt_Slow(x - 1, y);
		if (children[LEFT]) {
			children[LEFT]->SearchAndFillChildren(instance);
		}
	}

	if (children[UP] == nullptr) {
		children[UP] = instance->GetNodeAt_Slow(x, y + 1);
		if (children[UP]) {
			children[UP]->SearchAndFillChildren(instance);
		}
	}


}

void GridNode::RenderLines(ResourceShader* shaderRes)
{
	//shaderRes->Bind();
	//EngineExternal->moduleRenderer3D->activeRenderCamera->PushCameraShaderVars(shaderRes->shaderProgramID);

	//GLint modelLoc = glGetUniformLocation(shaderRes->shaderProgramID, "gridPosition");
	//glUniform2fv(modelLoc, 1, gridPosition);

	//glBindVertexArray(VAO);
	////glDrawArrays(GL_LINES, 0, (GRID_SIZE_X * GRID_SIZE_Y) + 2);
	//glDrawArrays(GL_LINE_STRIP, 0, 5);
	//glBindVertexArray(0);

	//shaderRes->Unbind();

	glColor3f(1., 0.f, 0.f);
	glBegin(GL_LINES);

	float3 position = float3::zero;
	for (size_t i = 0; i < NODE_SIDES; i++)
	{
		if (children[i] != nullptr) 
		{
			position = float3(this->GetGridPositionX(), 0.0f, this->GetGridPositionY());
			glVertex3fv(&position.x);

			float3 target = float3(children[i]->GetGridPositionX(), 0.0f, children[i]->GetGridPositionY());
			float3 dir = target - position;
			dir /= 2.5;

			glVertex3f(position.x + dir.x, 0.0, position.z + dir.z);
		}
	}

	glEnd();
	glColor3f(1., 1.f, 1.f);
}

bool GridNode::IsPosition(int x, int y)
{
	return (gridPosition[0] == x && gridPosition[1] == y);
}

void GridNode::SetGridPosition(int x, int y)
{
	gridPosition[0] = x;
	gridPosition[1] = y;
}

int* GridNode::GetGridPosition()
{
	return gridPosition;
}

float3 GridNode::GetGridPositionF3()
{
	return float3(gridPosition[0], 0, gridPosition[1]);
}

uint GridNode::GetID()
{
	return GridManager::CANTOR_MAPPING(gridPosition[0], gridPosition[1]);
}

int GridNode::GetGridPositionX()
{
	return gridPosition[0];
}

int GridNode::GetGridPositionY()
{
	return gridPosition[1];
}

GridNode* GridNode::DivideNode(GridManager* instance, int direction[2])
{
	int x = this->GetGridPositionX();
	int y = this->GetGridPositionY();

	int position[2] = { x + direction[0], y + direction[1]};



	GridNode* ref = instance->GetNodeAt_Slow(position[0], position[1]);
	GridNode** childArrayPos = this->GetChildrenMemAddr(direction[0], direction[1]);
	if (ref == nullptr)
	{

		GridNode* added = instance->AddNode(position[0], position[1]);
		//instance->linealNodes.push_back((*childArrayPos));

		//(*childArrayPos)->SearchAndFillChildren(instance);

	}
	else
	{
		*childArrayPos = ref;
	}
	return (*childArrayPos);
}

void GridNode::DivideNodeCross(GridManager* instance)
{
	for (size_t i = 0; i < sizeof(children) / sizeof(GridNode*); i++)
	{

		if (this->children[i] == nullptr) 
		{

			int x = this->GetGridPositionX();
			int y = this->GetGridPositionY();

			int position[2] = {0, 0};

			switch (i)
			{
			case UP:
				position[0] = x;
				position[1] = y+1;
				break;

			case DOWN:
				position[0] = x;
				position[1] = y - 1;
				break;

			case RIGHT:
				position[0] = x + 1;
				position[1] = y;
				break;

			case LEFT:
				position[0] = x - 1;
				position[1] = y;
				break;

			default:
				break;
			}

			GridNode* ref = instance->GetNodeAt_Slow(position[0], position[1]);
			if (ref == nullptr)
			{

				//instance->nodeMap[GridManager::CANTOR_MAPPING(position[0], position[1])] = GridNode();
				GridNode* added = instance->AddNode(position[0], position[1]);

				//this->children[i] = &instance->nodeMap[GridManager::CANTOR_MAPPING(position[0], position[1])];
				//this->children[i]->SetGridPosition(position[0], position[1]);
				this->children[i] = added;


				//instance->linealNodes.push_back(this->children[i]);


				//(*(this->children[i]->GetChildrenMemAddr(x - position[0], y - position[1]))) = this;
				//this->children[i]->SearchAndFillChildren(instance);

				//(*(this->children[i]->GetChildrenMemAddr(position[0] - x, position[1] - y)))->SearchAndFillChildren(instance);

			}
			else 
			{
				this->children[i] = ref;
			}

		}
	}
}

void GridNode::DivideNodeSquare(GridManager* instance, int squareLength)
{

	int direction[2] = { 1, 0 };
	
	GridNode* cNode = this;
	cNode = cNode->DivideNode(instance, direction);
	for (size_t i = 0; i < squareLength; i++)
	{


		direction[0] = 0; direction[1] = 1;
		while (cNode->children[LEFT] != nullptr)
		{
			cNode = cNode->DivideNode(instance, direction);
		}

		direction[0] = -1; direction[1] = 0;
		while (cNode->children[DOWN] != nullptr)
		{
			cNode = cNode->DivideNode(instance, direction);
		}

		direction[0] = 0; direction[1] = -1;
		while (cNode->children[RIGHT] != nullptr)
		{
			cNode = cNode->DivideNode(instance, direction);
		}

		direction[0] = 1; direction[1] = 0;
		while (cNode->children[UP] != nullptr)
		{
			cNode = cNode->DivideNode(instance, direction);
		}
	}
}


//TODO: Not optimal, at all
std::vector<float> GridNode::GetUniqueVertices()
{
	int x = this->GetGridPositionX();
	int y = this->GetGridPositionY();

	float3 vertices[4] = 
	{
		float3(-0.5f + x, 0.0, -0.5f + y),
		float3(-0.5f + x, 0.0, +0.5f + y),
		float3(+0.5f + x, 0.0, +0.5f + y),
		float3(+0.5f + x, 0.0, -0.5f + y),
	};


	int indicators[4] = { 0, 0, 0, 0 };


	if (children[UP] != nullptr) {
		indicators[1] = -1;
		indicators[2] = -1;
	}


	if (children[DOWN] != nullptr) {
		indicators[0] = -1;
		indicators[3] = -1;
	}



	if (children[RIGHT] != nullptr) {
		indicators[2] = -1;
		indicators[3] = -1;
	}



	if (children[LEFT] != nullptr) {
		indicators[1] = -1;
		indicators[0] = -1;
	}


	if(EngineExternal->moduleRenderer3D->gridInstance.GetNodeAt_Slow(x-1, y+1))
	{
		indicators[1] = -1;
	}
	if(EngineExternal->moduleRenderer3D->gridInstance.GetNodeAt_Slow(x+1, y+1))
	{
		indicators[2] = -1;
	}
	if(EngineExternal->moduleRenderer3D->gridInstance.GetNodeAt_Slow(x-1, y-1))
	{
		indicators[0] = -1;
	}
	if (EngineExternal->moduleRenderer3D->gridInstance.GetNodeAt_Slow(x + 1, y - 1)) 
	{
		indicators[3] = -1;
	}

	std::vector<float> verticesRet;
	for (size_t i = 0; i < 4; i++)
	{
		if (indicators[i] == 0) {
			verticesRet.push_back(vertices[i].x);
			verticesRet.push_back(vertices[i].y);
			verticesRet.push_back(vertices[i].z);
		}
	}
	


	return verticesRet;
}

GridNode** GridNode::GetChildrenMemAddr(int x, int y)
{
	GridNode** ret = nullptr;
	if (x > 0) 
	{
		ret = &children[RIGHT];
	}
	else if (x < 0) 
	{
		ret = &children[LEFT];
	}
	else if (y > 0) 
	{
		ret = &children[UP];
	}
	else if (y < 0) 
	{
		ret = &children[DOWN];
	}

	return ret;
}
