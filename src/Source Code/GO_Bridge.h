#pragma once

#include"GameObject.h"

class GO_Bridge : public GameObject
{
public:
	GO_Bridge(const char* name, GameObject* parent, int _uid = -1);
	~GO_Bridge();

private:

};
