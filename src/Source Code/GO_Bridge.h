#pragma once

#include"GameObject.h"
#include"MathGeoLib/include/Geometry/OBB.h"
#include"MathGeoLib/include/Geometry/AABB.h"

class GO_Bridge : public GameObject
{
public:
	GO_Bridge(const char* name, GameObject* parent, int _uid = -1);
	~GO_Bridge();

	void Draw(C_DirectionalLight* light = nullptr) override;
	void DrawOptionsMenu() override;
	void PureGLDraw(ResourceShader& selectedShader);

	void CreatBridge();
	float4x4 CalculateRopeTransform(float3& pointA, float3& pointB, float thickness);

	AABB localAABB;
private:
	float counter;

	OBB orientedBox;

	//Base settings
	float3 basePositionA;
	float3 basePositionB;
	
	//Pole settings
	float poleSeparation;

	//Rope settings
	float2 ropeOffset;
	float ropeThickess;
	int ropeDivisions;

	//Table settings
	float3 tableCurveOffset;
	float tableOffset;
	float3 tableSize;
	
	float3 colorArray[4];

};
