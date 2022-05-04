#pragma once

#include"GameObject.h"

class GO_Bridge : public GameObject
{
public:
	GO_Bridge(const char* name, GameObject* parent, int _uid = -1);
	~GO_Bridge();

	void Draw() override;
	void DrawOptionsMenu() override;

	void CreatBridge();
	float4x4 CalculateRopeTransform(float3& pointA, float3& pointB);

private:
	float counter;

};
