#pragma once

#include"GameObject.h"
#include<vector>

class GridNode;


class GO_Wall : public GameObject 
{

public:
	GO_Wall(const char* name, GameObject* parent, int _uid = -1);
	~GO_Wall() {

	}

	void InitWall(float3 wallPosition);


public:

	std::vector<GridNode*> trailNodes;
	uint subDivisions;


};