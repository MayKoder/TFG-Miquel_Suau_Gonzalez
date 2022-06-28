#pragma once

#include"GameObject.h"
#include"MathGeoLib/include/Geometry/OBB.h"
#include"MathGeoLib/include/Geometry/AABB.h"

static const char* typeDisplay[4] = {
	"Cube",
	"Cylinder",
	"Pyramid",
	"Plane"
};
static const char* plankTypeItem;

class GO_Bridge : public GameObject
{

	enum class PRIM_TYPE
	{
		CUBE,
		CYLINDER,
		PYRAMID,
		PLANE,
		MAX
	};


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
	int poleSubDivisions;

	//Rope settings
	float2 ropeOffset;
	float ropeThickess;
	int ropeDivisions;

	//Table settings
	float3 tableCurveOffset;
	float tableOffset;
	float3 tableSize;
	PRIM_TYPE plankType;

	float3 colorArray[4];

};
