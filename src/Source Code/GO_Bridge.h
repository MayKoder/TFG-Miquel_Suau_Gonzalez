#pragma once

#include"GameObject.h"

class GO_Bridge : public GameObject
{
public:
	GO_Bridge(const char* name, GameObject* parent, int _uid = -1);
	~GO_Bridge();

	void Draw(C_DirectionalLight* light = nullptr) override;
	void DrawOptionsMenu() override;
	void PureGLDraw(ResourceShader& selectedShader);

	void CreatBridge();
	float4x4 CalculateRopeTransform(float3& pointA, float3& pointB);

private:
	float counter;

	//Base settings
	float3 basePositionA;
	float3 basePositionB;
	
	//Pole settings
	float poleSeparation;

	//Rope settings
	float3 ropeOffset;

	//Table settings
	float3 tableCurveOffset;
	float tableOffset;
	float3 tableSize;
	

};
