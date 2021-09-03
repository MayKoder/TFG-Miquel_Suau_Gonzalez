
#include "Globals.h"
#include "GridManager.h"
#include "RE_Shader.h"

#include "Application.h"

#include "MO_ResourceManager.h"
#include "MO_Renderer3D.h"
#include "MO_Input.h"

#include "CO_Camera.h"

#include <iostream>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

#include"MathGeoLib/include/Geometry/Plane.h"
#include"MathGeoLib/include/Math/float4x4.h"
#include"MO_Window.h"

#include "CreationTool.h"


GridManager::GridManager() : shaderRes(nullptr), VBO(0), instanceVBO(0), VAO(0), hoveredNode(nullptr)
{
	//shaderRes = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(EngineExternal->GetRandomInt(), "Library/Shaders/1554189485.shdr"));



	mapTest[GridManager::CANTOR_MAPPING(0, 0)] = GridNode();
	mapTest[GridManager::CANTOR_MAPPING(0, 0)].SetGridPosition(0, 0);
	//linealNodes.push_back(&baseNode);

	//mapTest[GridManager::CANTOR_MAPPING(0, 0)].DivideNodeSquare(this, 500);

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
	//if (linealNodes.size() > 1) 
	//{
	//	for (size_t i = 1; i < linealNodes.size(); i++)
	//	{
	//		delete	linealNodes[i];
	//		linealNodes[i] = nullptr;
	//	}
	//}
	//linealNodes.clear();

	//std::map<uint, GridNode*>::iterator it;
	//for (it = mapTest.begin(); it != mapTest.end(); it++)
	//{
	//	delete (*it).second;
	//	//(*it).second = nullptr;
	//}
	mapTest.clear();
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
	hoveredNode = this->GetNodeAt_Slow(static_cast<int>(rint(ret.x)), static_cast<int>(rint(ret.z)));

	if (selectedTool != nullptr && hoveredNode != nullptr && EngineExternal->moduleInput->GetMouseButton(1) == KEY_STATE::KEY_DOWN) {
		selectedTool->Use();
	}


	//if (EngineExternal->moduleInput->GetMouseButton(1) == KEY_STATE::KEY_DOWN) {
	//	DivideHoveredClick();
	//}

}

void GridManager::DivideHoveredClick()
{
	if (hoveredNode != nullptr)
	{
		hoveredNode->DivideNodeCross(this);

		UpdateRenderData();
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void GridManager::LoadShader(const char* path)
{
	shaderRes = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(1554189485, path));


	float data[] = {
		-0.5, 0.0, -0.5, 
		-0.5, 0.0, 0.5,
		0.5, 0.0, 0.5,
		0.5, 0.0, -0.5,
		-0.5, 0.0, -0.5,
	};


	//for (size_t x = 0; x <= GRID_SIZE_X; x++)
	//{
	//	data.push_back(x);
	//	data.push_back(0.0f);
	//	data.push_back(0.0f);

	//	data.push_back(x);
	//	data.push_back(0.0f);
	//	data.push_back(GRID_SIZE_Y);
	//}

	//for (size_t y = 0; y <= GRID_SIZE_Y; y++)
	//{
	//	data.push_back(0.0f);
	//	data.push_back(0.0f);
	//	data.push_back(y);

	//	data.push_back(GRID_SIZE_X);
	//	data.push_back(0.0f);
	//	data.push_back(y);
	//}


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, (GLuint*)&(VBO));
	glGenBuffers(1, (GLuint*)&(instanceVBO));

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	UpdateRenderData();

	//instance data attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	
	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GridManager::UpdateRenderData()
{
	std::vector<float> instanceData;
	instanceData.reserve(mapTest.size() * 2);

	std::map<uint, GridNode>::iterator it;
	for (it = mapTest.begin(); it != mapTest.end(); it++)
	{
		instanceData.push_back(static_cast<float>(it->second.GetGridPositionX()));
		instanceData.push_back(static_cast<float>(it->second.GetGridPositionY()));
	}
	instanceData.shrink_to_fit();


	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(float), instanceData.data(), GL_DYNAMIC_DRAW);
}

void GridManager::ClearMemory()
{
	EngineExternal->moduleResources->UnloadResource(shaderRes->GetUID());

	glDeleteVertexArrays(1, &VAO);
	VAO = 0u;
	glDeleteBuffers(1, &VBO);
	VBO = 0u;

	glDeleteBuffers(1, &instanceVBO);
	instanceVBO = 0u;
}

//GridNode* GridManager::GetGridNode(int x, int y) {
//	return &grid[(y * GRID_SIZE_X) + x];
//}

void GridManager::RenderGridTemporal()
{
	//auto t1 = Clock::now();

	shaderRes->Bind();
	EngineExternal->moduleRenderer3D->activeRenderCamera->PushCameraShaderVars(shaderRes->shaderProgramID);

	glBindVertexArray(VAO);
	glDrawArraysInstanced(
		GL_LINE_STRIP, 0, 5, mapTest.size()
	);
	glBindVertexArray(0);
	shaderRes->Unbind();

	std::map<uint, GridNode>::iterator it;
	for (it = mapTest.begin(); it != mapTest.end(); it++)
	{
		it->second.RenderLines(shaderRes);
	}

	if (hoveredNode != nullptr) {
		glColor3f(1., 0.f, 0.f);
		glBegin(GL_LINES);

		float3 position = float3(static_cast<float>(hoveredNode->GetGridPositionX()), 0.0, static_cast<float>(hoveredNode->GetGridPositionY()));
		float crossSize = 0.03;

		glVertex3f(position.x - crossSize, 0.0, position.z + crossSize);
		glVertex3f(position.x + crossSize, 0.0, position.z - crossSize);

		glVertex3f(position.x + crossSize, 0.0, position.z + crossSize);
		glVertex3f(position.x - crossSize, 0.0, position.z - crossSize);

		glEnd();
		glColor3f(1., 1.f, 1.f);
	}

	//auto t2 = Clock::now();
	//LOG(LogType::L_NORMAL, "Rendering took: %dms should be like 7 at max", std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());

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
	if (mapTest.find(GridManager::CANTOR_MAPPING(x, y)) != mapTest.end()) {
		return &mapTest[GridManager::CANTOR_MAPPING(x, y)];
	}
	return nullptr;

	//auto t2 = Clock::now();
	//std::cout << "Delta t2-t1: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " nanoseconds" << std::endl;
}

GridNode::GridNode()
{
	memset(children, NULL, sizeof(children));
	memset(gridPosition, 0.0, sizeof(gridPosition));
}

void GridNode::SearchAndFillChildren(GridManager* instance)
{
	int x = this->GetGridPositionX();
	int y = this->GetGridPositionY();

	if (children[0] == nullptr) {
		children[0] = instance->GetNodeAt_Slow(x, y + 1);

		if (children[0]) {
			children[0]->SearchAndFillChildren(instance);
		}
	}

	if (children[1] == nullptr) {
		children[1] = instance->GetNodeAt_Slow(x, y - 1);
		if (children[1]) {
			children[1]->SearchAndFillChildren(instance);
		}
	}

	if (children[2] == nullptr) {
		children[2] = instance->GetNodeAt_Slow(x + 1, y);
		if (children[2]) {
			children[2]->SearchAndFillChildren(instance);
		}
	}

	if (children[3] == nullptr) {
		children[3] = instance->GetNodeAt_Slow(x - 1, y);
		if (children[3]) {
			children[3]->SearchAndFillChildren(instance);
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
		instance->mapTest[GridManager::CANTOR_MAPPING(position[0], position[1])] = GridNode();

		*childArrayPos = &instance->mapTest[GridManager::CANTOR_MAPPING(position[0], position[1])];
		(*childArrayPos)->SetGridPosition(position[0], position[1]);

		//instance->linealNodes.push_back((*childArrayPos));

		(*childArrayPos)->SearchAndFillChildren(instance);

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
			case 0:
				position[0] = x;
				position[1] = y+1;
				break;

			case 1:
				position[0] = x;
				position[1] = y - 1;
				break;

			case 2:
				position[0] = x + 1;
				position[1] = y;
				break;

			case 3:
				position[0] = x - 1;
				position[1] = y;
				break;

			default:
				break;
			}

			GridNode* ref = instance->GetNodeAt_Slow(position[0], position[1]);
			if (ref == nullptr)
			{
				instance->mapTest[GridManager::CANTOR_MAPPING(position[0], position[1])] = GridNode();

				this->children[i] = &instance->mapTest[GridManager::CANTOR_MAPPING(position[0], position[1])];
				this->children[i]->SetGridPosition(position[0], position[1]);

				//instance->linealNodes.push_back(this->children[i]);


				(*(this->children[i]->GetChildrenMemAddr(x - position[0], y - position[1]))) = this;
				this->children[i]->SearchAndFillChildren(instance);
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

GridNode** GridNode::GetChildrenMemAddr(int x, int y)
{
	GridNode** ret = nullptr;
	if (x > 0) 
	{
		ret = &children[2];
	}
	else if (x < 0) 
	{
		ret = &children[3];
	}
	else if (y > 0) 
	{
		ret = &children[0];
	}
	else if (y < 0) 
	{
		ret = &children[1];
	}

	return ret;
}
