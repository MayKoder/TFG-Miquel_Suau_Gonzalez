#pragma once

#include<vector>
class ResourceShader;


#define NODE_SIDES 4
struct GridNode
{
	GridNode* children[4];
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

private:

	GridNode baseNode;
	std::vector<GridNode*> linealNodes;

	//GridNode grid[GRID_SIZE_X * GRID_SIZE_Y];
	ResourceShader* shaderRes;

	uint VBO;
	uint VAO;
};