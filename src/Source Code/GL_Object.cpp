#include "GL_Object.h"
#include <algorithm>

GL_Object::GL_Object(RENDER_TYPE _type) : resShader(nullptr), type(_type), VAO(0),
EBO(0), usingVAO(false), lastElementSize(0)
{
}

GL_Object::~GL_Object()
{
	UnloadBuffers();
}

void GL_Object::InitBuffers()
{
	glGenVertexArrays(1, &VAO);

	if (type == RENDER_TYPE::RE_INDICES)
	{
		glGenBuffers(1, (GLuint*)&(EBO));
	}
}

void GL_Object::UnloadBuffers()
{
	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0u;
	}

	if (EBO != 0)
	{
		glDeleteBuffers(1, &EBO);
		EBO = 0u;
	}

	for (size_t index = 0; index < VBOs.size(); ++index)
	{
		glDeleteBuffers(1, &VBOs[index]);
		VBOs[index] = 0u;
	}
}

void GL_Object::Bind()
{
	glBindVertexArray(VAO);
	usingVAO = true;
}

void GL_Object::UnBind()
{
	glBindVertexArray(0);

	if (EBO != 0)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (VBOs.size() != 0)
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	usingVAO = false;
}

uint GL_Object::CreateVBO()
{
	assert(usingVAO == true, "Not using VAO");

	VBOs.push_back(0);
	glGenBuffers(1, (GLuint*)&(VBOs[VBOs.size() - 1]));


	return (VBOs.size() - 1);
}

void GL_Object::RemoveVertices(std::vector<float>& gridMeshVertices, std::vector<int>& gridMeshIndices, std::vector<float>& uniqueVertices, int localFirstIndex)
{
	//Remove unique vertices from vertex vector, not valid with new system [TESTING NEW VERSION]//
	std::vector<int> deletedVerticesID;
	for (size_t i = 0; i < uniqueVertices.size(); i += 3)
	{
		int indicator = FloatArrayToIndex(gridMeshVertices, float3(uniqueVertices[i], uniqueVertices[i + 1], uniqueVertices[i + 2]));
		deletedVerticesID.push_back(indicator);
	}
	for (size_t i = 0; i < uniqueVertices.size(); i += 3)
	{
		int indicator = FloatArrayToIndex(gridMeshVertices, float3(uniqueVertices[i], uniqueVertices[i + 1], uniqueVertices[i + 2]));

		auto first = gridMeshVertices.cbegin() + (indicator * 3);
		auto last = gridMeshVertices.cbegin() + ((indicator * 3) + 3);
		gridMeshVertices.erase(first, last);
	}

	//Delete indices from index vector, this is still valid//
	auto indFirts = gridMeshIndices.cbegin() + localFirstIndex;
	auto indLast = gridMeshIndices.cbegin() + (localFirstIndex + 6);
	gridMeshIndices.erase(indFirts, indLast);

	std::sort(deletedVerticesID.begin(), deletedVerticesID.end());
	for (size_t i = 0; i < deletedVerticesID.size(); ++i)
	{

		for (size_t j = 0; j < gridMeshIndices.size(); ++j)
		{
			if (gridMeshIndices[j] >= deletedVerticesID[i])
			{
				gridMeshIndices[j]--;

			}
		}
		for (size_t k = 0; k < deletedVerticesID.size(); ++k)
		{
			deletedVerticesID[k]--;
		}


	}

	Bind();
	SetVBO(0, gridMeshVertices.data(), gridMeshVertices.size(), GL_DYNAMIC_DRAW);
	LoadEBO(gridMeshIndices.data(), gridMeshIndices.size());
	UnBind();
}

void GL_Object::RemoveTriangles(std::vector<int>& gridMeshIndices, std::vector<int>& triangleIndex)
{
	//TODO: Let's cluster the delete method
	for (std::vector<int>::iterator it = triangleIndex.begin(); it != triangleIndex.end(); ++it)
	{
		//When we delete, we move the index of the following triangles, so this does not work
		int startIndicator = (*it) * 3;
		auto indFirts = gridMeshIndices.cbegin() + startIndicator;
		auto indLast = gridMeshIndices.cbegin() + (startIndicator + 3);
		gridMeshIndices.erase(indFirts, indLast);

		for (size_t i = 0; i < triangleIndex.size(); i++)
		{
			if (triangleIndex[i] > (*it)) {
				triangleIndex[i] -= 1;
			}
		}
	}


	Bind();
	LoadEBO(gridMeshIndices.data(), gridMeshIndices.size());
	UnBind();
}

int GL_Object::FloatArrayToIndex(std::vector<float>& vector, float3 value)
{
	int ret = -1;

	//TODO: Would iterator be faster?
	for (size_t i = 0; i < vector.size(); i += 3)
	{
		if (vector[i] == value.x && vector[i + 1] == value.y && vector[i + 2] == value.z)
		{
			ret = i / 3;
			break;
		}
	}

	return ret;
}

void GL_Object::RenderAsArray(GLenum mode, GLsizei firstElementPosition, GLenum numberOfIndices)
{
	Bind();
	glDrawArrays(mode, firstElementPosition, numberOfIndices);
	UnBind();
}

void GL_Object::RenderAsIndices(GLenum mode, GLsizei elementCount, GLenum type)
{
	Bind();
	glDrawElements(mode, elementCount, type, NULL);
	UnBind();
}

void GL_Object::RenderAsInstancing(int renderType, int first, int indicesToRender, int instancecount)
{
	Bind();
	glDrawArraysInstanced(
		renderType, first, indicesToRender, instancecount
	);
	UnBind();
}

//TODO: Change packElements to input packElements * sizeof() and not doit inside this method
void GL_Object::SetVertexAttrib(int index, int attribSize, int packElementsBytes, int offsetBytes, int type = GL_FLOAT)
{
	assert(usingVAO == true, "Not using VAO");

	//TODO: Why only GL_FLOAT? and will never normalize?
	glVertexAttribPointer(index, attribSize, type, GL_FALSE, packElementsBytes, (GLvoid*)(offsetBytes));
	glEnableVertexAttribArray(index);

	//if (type == RENDER_TYPE::RE_INSTANCING || attribDivisor >= 0)
	//{
	//	glVertexAttribDivisor(index, attribDivisor);
	//}
}

void GL_Object::SetAttribDivisor(int index, uint attribDivisor)
{
	glVertexAttribDivisor(index, attribDivisor);
}
