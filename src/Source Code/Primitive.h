#pragma once

#include<vector>
#include"GL_Object.h"
#include"MathGeoLib/include/Math/float4x4.h"

class Primitive
{
public:
	Primitive();
	~Primitive();

	int GetIndicesSize() {
		return indices.size();
	}

	int AddVertexData(float x, float y, float z) {
		int ret = dataPacks.size() / dataPackSize;
		dataPacks.push_back(x);
		dataPacks.push_back(y);
		dataPacks.push_back(z);
		return ret;
	}
	void AddIndexData(int a, int b, int c) {
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	}

	void CapMeshVertices(int);
	void CapMeshWithStep(int, int);
	float3 CalculateQuadNormal(float3&, float3&, float3&, float3&);

	void DebugDrawVertices();
	void DebugDrawNormals();

	GL_Object meshObject = GL_Object(GL_Object::RENDER_TYPE::RE_INDICES);

	std::vector<float> dataPacks;
	std::vector<int> indices;
	float4x4 modelMatrix;

private:
	int dataPackSize;


};
