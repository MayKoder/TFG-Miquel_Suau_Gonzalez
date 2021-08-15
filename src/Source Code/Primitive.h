#pragma once

#include<vector>
class ResourceShader;
class GridManager;

#define NODE_SIDES 4
struct GridNode
{
	//enum Direction
	//{
	//	UP, DOWN, RIGHT, LEFT
	//};

	GridNode();

	void RenderLines(ResourceShader*, uint);
	
	bool IsPosition(int x, int y);
	void SetGridPosition(int x, int y);

	float* GetGridPosition();

	int GetGridPositionX();
	int GetGridPositionY();

	void DivideNode(GridManager* instance);
	
	GridNode* children[NODE_SIDES];
private:
	float gridPosition[2];
};

#define GRID_SIZE_X 5
#define GRID_SIZE_Y 4
#define GRID_NODE_SIZE 1.8f
class GridManager
{
public:
	GridManager();
	~GridManager();

	void LoadShader(const char* path);
	void ClearMemory();

	//GridNode* GetGridNode(int x, int y);
	void RenderGridTemporal();

	GridNode* GetNodeAt_Slow(int x, int y);

	std::vector<GridNode*> linealNodes;
private:

	GridNode baseNode;

	//GridNode grid[GRID_SIZE_X * GRID_SIZE_Y];
	ResourceShader* shaderRes;

	uint VBO;
	uint VAO;
};