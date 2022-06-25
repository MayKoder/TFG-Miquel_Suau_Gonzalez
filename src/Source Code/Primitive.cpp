#include "Primitive.h"
#include"MathGeoLib/include/Math/float4.h"

Primitive::Primitive() : dataPackSize(6), modelMatrix(float4x4::identity), solidColor(float3::one)
{
}

Primitive::~Primitive()
{
	this->dataPacks.clear();
	this->indices.clear();

	this->meshObject.UnloadBuffers();
}

void Primitive::CapMeshVertices(int dataOffset)
{

	for (size_t i = 0; i < (this->dataPacks.size() / dataOffset); i += 4)
	{
		int a = i;
		int b = i+1;
		int c = i+2;
		int d = i+3;

		this->AddIndexData(a, b, c);
		this->AddIndexData(c, d, a);

	}




}

void Primitive::CapMeshWithStep(int dataOffset, int stepToNextRow)
{
	for (size_t i = 0; i < (this->dataPacks.size() / dataOffset); i += 4)
	{
		int a = i;
		int b = i + 1;
		int c = i + 2;
		int d = i + 3;

		this->AddIndexData(a, b, c);
		this->AddIndexData(c, d, a);
	}
}

float3 Primitive::CalculateQuadNormal(float3& a, float3& b, float3& c, float3& d)
{
	float3 cross = (b-a).Cross((c-a));




	return cross.Normalized();
}

void Primitive::DebugDrawVertices()
{

	glPointSize(10.0f);
	glBegin(GL_POINTS);

	for (size_t i = 0; i < dataPacks.size(); i += dataPackSize)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		float3 point = (this->modelMatrix * float4(dataPacks[i], dataPacks[i + 1], dataPacks[i + 2], 1.0)).xyz();
		glVertex3fv(point.ptr());
	}

	glEnd();
}

void Primitive::DebugDrawNormals()
{
	//glBegin(GL_LINES);

	//for (size_t i = 0; i < dataPacks.size(); i += dataPackSize)
	//{
	//	glColor3f(1.0f, 1.0f, 1.0f);
	//	float3 point = (this->modelMatrix * float4(dataPacks[i], dataPacks[i + 1], dataPacks[i + 2], 1.0)).xyz();
	//	glVertex3fv(point.ptr());

	//	glColor3f(1.0f, 1.0f, 1.0f);

	//	float3 normal = (this->modelMatrix * float4(dataPacks[i + 3], dataPacks[i + 4], dataPacks[i + 5], 1.0)).xyz();
	//	normal = modelMatrix.Transposed().InverseTransposed().Float3x3Part() * normal.Normalized();
	//	
	//	
	//	glVertex3fv((point + (normal.Normalized() * 0.3f)).ptr());
	//}
	//glEnd();

	glPushMatrix();
	glMultMatrixf(this->modelMatrix.Transposed().ptr());

	float normalLenght = 0.25f;
	//Vertex normals
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	for (size_t i = 0; i < dataPacks.size(); i += dataPackSize)
	{
		float3 point = float3(dataPacks[i], dataPacks[i + 1], dataPacks[i + 2]);
		float3 normal = float3(dataPacks[i + 3], dataPacks[i + 4], dataPacks[i + 5]);
		normal.Normalize();

		glVertex3fv(point.ptr());
		glVertex3f(point.x + (normal.x * normalLenght), point.y + (normal.y * normalLenght), point.z + (normal.z * normalLenght));
	}
	glEnd();
	glColor3f(1, 1, 1);

	glPopMatrix();
}
