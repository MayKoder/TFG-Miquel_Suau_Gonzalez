#pragma once

#include<vector>
#include<map>
class ResourceShader;
class GridManager;

//#define CANTOR_MAPPING(x, y) { 	\
//uint _x = (x >= 0) ? 2 * x : (-2* x) -1 ; \
//uint _y = (y >= 0) ? 2 * y : (-2 * y) - 1; \
//
//return  ((_x + _y) * (_x + _y + 1) / 2) + _y;     \
//}

#define NODE_SIDES 4
struct GridNode
{
	enum Direction
	{
		UP, DOWN, RIGHT, LEFT
	};

	GridNode();
	void SearchAndFillChildren(GridManager* instance);

	void RenderLines(ResourceShader*);
	
	bool IsPosition(int x, int y);
	void SetGridPosition(int x, int y);

	int* GetGridPosition();
	uint GetID();

	int GetGridPositionX();
	int GetGridPositionY();

	GridNode* DivideNode(GridManager* instance, int direction[2]);
	void DivideNodeCross(GridManager* instance);
	void DivideNodeSquare(GridManager* instance, int squareLength);

	GridNode** GetChildrenMemAddr(int x, int y);
	
	GridNode* children[NODE_SIDES];
private:
	int gridPosition[2];
};

#define GRID_SIZE_X 5
#define GRID_SIZE_Y 4
#define GRID_NODE_SIZE 1.8f
class Tool;
class GridManager
{
public:
	GridManager();
	~GridManager();

	void UpdateInput(Tool*);

	void LoadShader(const char* path);
	void UpdateRenderData();
	void ClearMemory();

	//GridNode* GetGridNode(int x, int y);
	void RenderGridTemporal();

	GridNode* GetNodeAt_Slow(int x, int y);



	static inline uint CANTOR_MAPPING(int x, int y) 
	{
		uint _x = (x >= 0) ? 2 * x : (-2 * x) - 1;
		uint _y = (y >= 0) ? 2 * y : (-2 * y) - 1;

		return  ((_x + _y) * (_x + _y + 1) / 2) + _y;
	}

	//std::vector<GridNode*> linealNodes;
	std::map<uint, GridNode> mapTest;
private:


	//GridNode grid[GRID_SIZE_X * GRID_SIZE_Y];
	ResourceShader* shaderRes;

	uint VAO;
	uint VBO; //Mesh positions
	uint instanceVBO;

	GridNode* hoveredNode;
};