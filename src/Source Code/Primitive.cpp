
#include "Globals.h"
#include "Primitive.h"
#include "RE_Shader.h"

#include "MO_ResourceManager.h"
#include "Application.h"

#include "MO_Renderer3D.h"
#include "CO_Camera.h"

#include <iostream> 
#include <ctime>


GridManager::GridManager() : shaderRes(nullptr), VBO(0), VAO(0)
{
	//shaderRes = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(EngineExternal->GetRandomInt(), "Library/Shaders/1554189485.shdr"));
	//LOG(LogType::L_NORMAL, "wtf");

	baseNode.SetGridPosition(0, 0);
	linealNodes.push_back(&baseNode);

	baseNode.DivideNode(this);

	baseNode.children[0]->DivideNode(this);
	baseNode.children[1]->DivideNode(this);
	baseNode.children[2]->DivideNode(this);
	baseNode.children[3]->DivideNode(this);
}

GridManager::~GridManager()
{
	if (linealNodes.size() > 1) 
	{
		for (size_t i = 1; i < linealNodes.size(); i++)
		{
			delete	linealNodes[i];
			linealNodes[i] = nullptr;
		}
	}
	linealNodes.clear();
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

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GridManager::ClearMemory()
{
	EngineExternal->moduleResources->UnloadResource(shaderRes->GetUID());

	glDeleteVertexArrays(1, &VAO);
	VAO = 0u;
	glDeleteBuffers(1, &VBO);
	VBO = 0u;
}

//GridNode* GridManager::GetGridNode(int x, int y) {
//	return &grid[(y * GRID_SIZE_X) + x];
//}

void GridManager::RenderGridTemporal()
{

	for (size_t i = 0; i < linealNodes.size(); i++)
	{
		linealNodes[i]->RenderLines(shaderRes, VAO);
	}

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
	std::clock_t start = std::clock();

	//code here
	for (auto it = linealNodes.begin(); it != linealNodes.end(); ++it)
	{
		if ((*it)->IsPosition(x, y)) 
		{
			return (*it);
		}
	}
	return nullptr;

	double duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::cout << duration << std::endl;

}

GridNode::GridNode()
{
	memset(children, NULL, sizeof(children));
	memset(gridPosition, 0.0, sizeof(gridPosition));
}

void GridNode::RenderLines(ResourceShader* shaderRes, uint VAO)
{
	shaderRes->Bind();
	EngineExternal->moduleRenderer3D->activeRenderCamera->PushCameraShaderVars(shaderRes->shaderProgramID);

	GLint modelLoc = glGetUniformLocation(shaderRes->shaderProgramID, "gridPosition");
	glUniform2fv(modelLoc, 1, gridPosition);

	glBindVertexArray(VAO);
	//glDrawArrays(GL_LINES, 0, (GRID_SIZE_X * GRID_SIZE_Y) + 2);
	glDrawArrays(GL_LINE_STRIP, 0, 5);
	glBindVertexArray(0);

	shaderRes->Unbind();

	glColor3f(1., 0.f, 0.f);
	glBegin(GL_LINES);

	float3 position = float3(gridPosition[0], 0.0, gridPosition[1]);
	float crossSize = 0.03;

	glVertex3f(position.x - crossSize, 0.0, position.z + crossSize);
	glVertex3f(position.x + crossSize, 0.0, position.z - crossSize);

	glVertex3f(position.x + crossSize, 0.0, position.z + crossSize);
	glVertex3f(position.x - crossSize, 0.0, position.z - crossSize);

	glEnd();
	glColor3f(1., 1.f, 1.f);
}

bool GridNode::IsPosition(int x, int y)
{
	return (gridPosition[0] == static_cast<float>(x) && gridPosition[1] == static_cast<float>(y));
}

void GridNode::SetGridPosition(int x, int y)
{
	gridPosition[0] = static_cast<float>(x);
	gridPosition[1] = static_cast<float>(y);
}

float* GridNode::GetGridPosition()
{
	return gridPosition;
}

int GridNode::GetGridPositionX()
{
	return static_cast<int>(gridPosition[0]);
}

int GridNode::GetGridPositionY()
{
	return static_cast<int>(gridPosition[1]);
}

void GridNode::DivideNode(GridManager* instance)
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
				this->children[i] = new GridNode();
				this->children[i]->SetGridPosition(position[0], position[1]);
				instance->linealNodes.push_back(this->children[i]);
			}
			else 
			{
				this->children[i] = ref;
			}

		}
	}
}
