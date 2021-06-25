#pragma once

#include<vector>
#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Algorithm/Random/LCG.h"

class ResourceMesh;

#define MAX_PLANE_TEMP 50u
typedef unsigned int uint;

class ProceduralMesh
{
public:
	ProceduralMesh();
	virtual ~ProceduralMesh();

	virtual void GenerateData() = 0;
	void RenderOGL();

	ResourceMesh* _mesh;
protected:
	//std::vector<float3> vertices;
	//std::vector<uint> indices;


	LCG randomizer;

};



class MMP_Plane : public ProceduralMesh
{
public:
	MMP_Plane();
	virtual ~MMP_Plane();


	void GenerateData() override;

	static void GenerateDataTo(ResourceMesh* _mesh);

private:

};