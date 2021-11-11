#pragma once

#include<vector>
#include<map>
class ResourceShader;
class GridManager;
#include"GL_Object.h"

//TODO: Temporal
#include"LineRenderer.h"
#include "GameObject.h"

#define NODE_SIDES 4
struct GridNode
{
	enum Direction
	{
		UP, DOWN, RIGHT, LEFT
	};

	GridNode();
	GridNode(int x, int y);

	void SearchAndFillChildren(GridManager* instance);

	void RenderLines(ResourceShader*);
	
	bool IsPosition(int x, int y);
	void SetGridPosition(int x, int y);

	uint GetID();
	int* GetGridPosition();

	int GetGridPositionX();
	int GetGridPositionY();

	void DivideNodeCross(GridManager* instance);
	GridNode* DivideNode(GridManager* instance, int direction[2]);
	void DivideNodeSquare(GridManager* instance, int squareLength);

	std::vector<float> GetUniqueVertices();

	GridNode** GetChildrenMemAddr(int x, int y);
	
	GridNode* children[NODE_SIDES];
	uint indicesIndexTmp;
	GameObject* go;
private:
	int gridPosition[2];

};

//#define GRID_SIZE_X 5
//#define GRID_SIZE_Y 4
#define GRID_NODE_SIZE 1.8f
class Tool;
class GridManager
{
public:
	GridManager();
	~GridManager();

	void UpdateInput(Tool*);

	void CreateNode();
	void DivideHoveredClick();
	void CreateNodesCircular(int radius);


	GridNode* AddNode(int x, int y, bool unBind = true);

	bool DeleteHoveredNode();

	void LoadShader(const char* path);
	//void UpdateRenderData(bool unBindAfter = true);
	void ClearMemory();

	void RenderGridTemporal();

	GridNode* GetNodeAt_Slow(int x, int y);
	bool CanBuildOnMouseNode();

	int GetVertexIndex(float3 value);

	int GetMouseGridPos_X() {
		return this->cursorGridPos[0];
	}
	int GetMouseGridPos_Z() {
		return this->cursorGridPos[1];
	}



	static inline uint CANTOR_MAPPING(int x, int y) 
	{
		uint _x = (x >= 0) ? 2 * x : (-2 * x) - 1;
		uint _y = (y >= 0) ? 2 * y : (-2 * y) - 1;

		return  ((_x + _y) * (_x + _y + 1) / 2) + _y;
	}

private:
	std::map<uint, GridNode> nodeMap;


	//GridNode grid[GRID_SIZE_X * GRID_SIZE_Y];
	ResourceShader* shaderRes;
	ResourceShader* meshGridShader;

	//uint VAO;
	//uint VBO; //Mesh positions
	//uint instanceVBO;

	GL_Object gridGLObject = GL_Object(GL_Object::RENDER_TYPE::RE_ARRAY);
	GL_Object gridMeshObject = GL_Object(GL_Object::RENDER_TYPE::RE_INDICES);

	std::vector<float> gridMeshVertices;
	std::vector<int> gridMeshIndices;

	GridNode* hoveredNode;
	int cursorGridPos[2];

	//LineRenderer testRender;
};