#pragma once

#include"GameObject.h"
#include<vector>

class GridNode;

struct WallNode
{
	WallNode() 
	{
		for (size_t i = 0; i < sizeof(faceIndices) / sizeof(int); ++i)
		{
			faceIndices[i] = 0;
		}
	}

	int faceIndices[4];

	void ClearFace(int side) {
		faceIndices[side] = 0;
	}

	int GetTotalIndices() {
		int ret = 0;
		for (size_t i = 0; i < sizeof(faceIndices) / sizeof(int); i++)
		{
			ret += faceIndices[i];
		}
		return ret;
	}

};


class GO_Wall : public GameObject 
{

public:
	GO_Wall(const char* name, GameObject* parent, int _uid = -1);
	~GO_Wall() {

	}

	WallNode InitWall(float3 wallPosition);
	void UpdateWallGL();
	static WallNode GenerateWall(uint, float3, std::vector<float>& vertices, std::vector<int>& indices, std::vector<int>* sidesToIgnore);


public:

	std::vector<std::pair<WallNode, GridNode*>> trailNodes;
	uint subDivisions;
	int currentIndices;


};