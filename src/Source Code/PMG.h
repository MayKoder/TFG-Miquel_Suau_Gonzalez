#pragma once
#include"Primitive.h"

namespace PMG
{

	Primitive CreateCube(float4x4& transform);
	Primitive CreateCylinder(float4x4& transform, int hDivisions = 1, int vDivisions = 3, float3 curveOffset = float3::zero);
	Primitive CreateQuad(float4x4& transform);


}