#include "PMG.h"
#include"Globals.h"
#include"MathGeoLib/include/Math/Quat.h"
#include"BezierCurve.h"

Primitive PMG::CreateCube(float4x4& transform)
{
	Primitive ret;
	ret.modelMatrix.Set(transform);

	float3 size = float3(1.0, 1.0, 1.0);

	float initPoints[] = { 

		//Y
		0.5, 0.5, 0.5,			0.0, 1.0, 0.0,
		0.5, 0.5, -0.5,			0.0, 1.0, 0.0,
		-0.5, 0.5, -0.5,		0.0, 1.0, 0.0,
		-0.5, 0.5, 0.5,			0.0, 1.0, 0.0,

		//Z
		-0.5, 0.5, 0.5,			0.0, 0.0, 1.0,
		-0.5, -0.5, 0.5,		0.0, 0.0, 1.0,
		0.5, -0.5, 0.5,			0.0, 0.0, 1.0,
		0.5, 0.5, 0.5,			0.0, 0.0, 1.0,

		//X
		0.5, 0.5, 0.5,			1.0, 0.0, 0.0,
		0.5, -0.5, 0.5,			1.0, 0.0, 0.0,
		0.5, -0.5, -0.5,		1.0, 0.0, 0.0,
		0.5, 0.5, -0.5,			1.0, 0.0, 0.0,

		//-Z
		0.5, 0.5, -0.5,			0.0, 0.0, -1.0,
		0.5, -0.5, -0.5,		0.0, 0.0, -1.0,
		-0.5, -0.5, -0.5,		0.0, 0.0, -1.0,
		-0.5, 0.5, -0.5,		0.0, 0.0, -1.0,

		//-X
		-0.5, 0.5, -0.5,		-1.0, 0.0, 0.0,
		-0.5, -0.5, -0.5,		-1.0, 0.0, 0.0,
		-0.5, -0.5, 0.5,		-1.0, 0.0, 0.0,
		-0.5, 0.5, 0.5,			-1.0, 0.0, 0.0,

		//-Y
		0.5, -0.5, -0.5,		0.0, -1.0, 0.0,
		0.5, -0.5, 0.5,			0.0, -1.0, 0.0,
		-0.5, -0.5, 0.5,		0.0, -1.0, 0.0,
		-0.5, -0.5, -0.5,		0.0, -1.0, 0.0,
	};

	ret.dataPacks.insert(ret.dataPacks.end(), initPoints, initPoints + (sizeof(initPoints) / sizeof(float)));
	ret.CapMeshVertices(6);

	//for (size_t i = 0; i < 5; i++)
	//{
	//	initPoints[i] *= size.x;
	//	//initPoints[i] *= size.y;
	//	initPoints[i+2] *= size.z;
	//}

	//int horizontalLines = 20;
	//int verticalLines = 20;
	//for (int n = 0; n < verticalLines; n++)
	//{
	//	for (int m = 0; m < horizontalLines; m++)
	//	{
	//		float x = sinf(PI * m / horizontalLines) * cosf(2 * PI * n / verticalLines);
	//		float y = sinf(PI * m / horizontalLines) *sinf(2 * PI * n / verticalLines);
	//		float z = cosf(PI * m / horizontalLines);

	//		ret.AddVertexData(x, y, z);
	//		ret.AddVertexData(0.0, 1.0, 0.0);
	//	}
	//}
	//ret.CapMeshVertices(6);


	return ret;
}

Primitive PMG::CreateCylinder(float4x4& transform, int hDivisions, int vDivisions, float3 curveOffset)
{
	Primitive ret;
	ret.modelMatrix.Set(transform);

	int upPointer = 0;
	int downPointer = 0;
	float step = (2.0 * PI) / static_cast<float>(vDivisions);

	float halfUpperSize = 1.0;
	float upperIncrement = (2.0 * halfUpperSize) / static_cast<float>(hDivisions);

	BezierCurve ropeCurve = BezierCurve(std::vector<float3>{float3(0.0, halfUpperSize, 0.0), curveOffset, float3(0.0, -halfUpperSize, 0.0)});

	float normalizedIncrementA = 0.0;
	float normalizedIncrementB = 0.0;
	//WARNING: This may need a revision for overflow Y
	for (float yh = halfUpperSize; yh > -halfUpperSize; yh -= upperIncrement)
	{

		normalizedIncrementA = (yh - (halfUpperSize)) / (-halfUpperSize - halfUpperSize);
		float3 lerpedFinal = ropeCurve.GetValue(normalizedIncrementA);
		lerpedFinal.y = 0.0;
		//normalizedIncrementA = pow(2, 10 * normalizedIncrementA - 10);

		float clampY = yh - upperIncrement;
		CLAMP(clampY,-halfUpperSize, halfUpperSize);

		normalizedIncrementB = (clampY - (halfUpperSize)) / (-halfUpperSize - halfUpperSize);
		float3 lerpedFinal2 = ropeCurve.GetValue(normalizedIncrementB);
		lerpedFinal2.y = 0.0;
		//normalizedIncrementB = pow(2, 10 * normalizedIncrementB - 10);

		/*LOG("%f", pow(2, 10 * normalizedIncrementA - 10));*/

		for (float i = 0; i <= (2.0 * PI); i += step)
		{

			float3 a, b, c, d;

			float x = 0.0 + 1.0 * cos(i);
			float z = 0.0 + 1.0 * sin(i);


			a.Set(x, clampY, z);
			a = a + lerpedFinal2;
			b.Set(x, yh, z);
			b = b + lerpedFinal;

			x = 0.0 + 1.0 * cos(i + step);
			z = 0.0 + 1.0 * sin(i + step);

			c.Set(x, yh, z);
			c = c + lerpedFinal;
			d.Set(x, clampY, z);
			d = d + lerpedFinal2;

			float3 normal = ret.CalculateQuadNormal(a, b, c, d);

			ret.AddVertexData(a.x, a.y, a.z);
			ret.AddVertexData(normal.x, normal.y, normal.z);

			ret.AddVertexData(b.x, b.y, b.z);
			ret.AddVertexData(normal.x, normal.y, normal.z);

			ret.AddVertexData(c.x, c.y, c.z);
			ret.AddVertexData(normal.x, normal.y, normal.z);

			ret.AddVertexData(d.x, d.y, d.z);
			ret.AddVertexData(normal.x, normal.y, normal.z);
		}
	}

	ret.CapMeshVertices(6);

	for (int sign = 1; sign >= (-1); sign -=2)
	{
		float3 upperCapVertex = float3(0.0, halfUpperSize * sign, 0.0);
		int upperVertexIndex = ret.AddVertexData(upperCapVertex.x, upperCapVertex.y, upperCapVertex.z);
		ret.AddVertexData(0.0, 1.0 * sign, 0.0);
		for (float i = 0; i <= (2.0 * PI); i += step)
		{
			float3 a, b;

			float x = 0.0 + 1.0 * cos(i);
			float z = 0.0 + 1.0 * sin(i);
			a.Set(x, halfUpperSize * sign, z);

			x = 0.0 + 1.0 * cos(i + step);
			z = 0.0 + 1.0 * sin(i + step);
			b.Set(x, halfUpperSize * sign, z);


			int bVertexIndex = ret.AddVertexData(a.x, a.y, a.z);
			ret.AddVertexData(0.0, 1.0 * sign, 0.0);

			int cVertexIndex = ret.AddVertexData(b.x, b.y, b.z);
			ret.AddVertexData(0.0, 1.0 * sign, 0.0);

			if (sign > 0) {
				ret.AddIndexData(bVertexIndex, upperVertexIndex, cVertexIndex);
			}
			else {
				ret.AddIndexData(upperVertexIndex, bVertexIndex, cVertexIndex);
			}
		}
	}

	return ret;
}

Primitive PMG::CreateQuad(float4x4& transform)
{
	Primitive ret;
	ret.modelMatrix.Set(transform);

	float initPoints[] = {

		//Y
		0.5, 0.0, 0.5,			0.0, 1.0, 0.0,
		0.5, 0.0, -0.5,			0.0, 1.0, 0.0,
		-0.5, 0.0, -0.5,		0.0, 1.0, 0.0,
		-0.5, 0.0, 0.5,			0.0, 1.0, 0.0,
	};

	ret.dataPacks.insert(ret.dataPacks.end(), initPoints, initPoints + (sizeof(initPoints) / sizeof(float)));
	ret.CapMeshVertices(6);

	return ret;
}
