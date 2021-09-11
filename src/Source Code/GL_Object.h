#pragma once

#include<vector>
#include"OpenGL.h"
#include<cassert>
class ResourceShader;
typedef unsigned int uint;

class GL_Object
{

public:
	enum class RENDER_TYPE
	{
		RE_ARRAY,
		RE_INDICES,
		RE_INSTANCING
	};

public:
	GL_Object(RENDER_TYPE _type) : resShader(nullptr), type(_type), VAO(0),
	EBO(0)
	{
		//glGenVertexArrays(1, &VAO);

		//if (type == RENDER_TYPE::RE_INDICES) 
		//{
		//	glGenBuffers(1, (GLuint*)&(EBO));
		//}
	}
	~GL_Object() 
	{
		UnloadData();
	}

	void InitBuffers() {
		glGenVertexArrays(1, &VAO);

		if (type == RENDER_TYPE::RE_INDICES)
		{
			glGenBuffers(1, (GLuint*)&(EBO));
		}
	}
	void UnloadData() 
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

		for (size_t VBO = 0; VBO < VBOs.size(); ++VBO)
		{
			glDeleteBuffers(1, &VBO);
			VBO = 0u;
		}
	}

	void Bind() 
	{
		glBindVertexArray(VAO);
		usingVAO = true;
	}
	void UnBind() 
	{
		glBindVertexArray(0);

		if(EBO != 0)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		if(VBOs.size() != 0)
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		usingVAO = false;
	}

	template <class T>
	void LoadVBO(T* data, int nElements) 
	{
		assert(usingVAO == true, "Not using VAO");

		VBOs.push_back(0);
		glGenBuffers(1, (GLuint*)&(VBOs[VBOs.size() -1]));

		uint VBO = VBOs[VBOs.size() - 1];

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * nElements, data, GL_STATIC_DRAW);
	}

	/*
	* Index = attrib index
	* attribSize = number of elements (ex: Vec3 = 3 elements)
	* packElements = in case of packed elements, the total element size (ex: position + color = vec3 + vec4 = 3 + 4)
	* offset = in case of packed elements, the number of elements to the start of this attrib (ex: position + color, color would be 3, as there is a 3 element offset, 0 to 3 for position, then color starts at 3)
	*/
	void SetVertexAttrib(int index, int attribSize, int packElements, int offset, int attribDivisor = -1) 
	{
		assert(usingVAO == true, "Not using VAO");

		//TODO: Why only GL_FLOAT? and will never normalize?
		glVertexAttribPointer(index, attribSize, GL_FLOAT, GL_FALSE, packElements * sizeof(float), (GLvoid*)(offset * sizeof(GLfloat)));
		glEnableVertexAttribArray(index);

		//if (type == RENDER_TYPE::RE_INSTANCING || attribDivisor >= 0)
		//{
		//	glVertexAttribDivisor(index, attribDivisor);
		//}
	}

	void SetAttribDivisor(int index, uint attribDivisor)
	{
		glVertexAttribDivisor(index, attribDivisor);
	}

	//TODO: Uint is a custom define and not portable, is this a bad idea?
	template <class T>
	void LoadEBO(T* data, int nElements) 
	{
		assert(usingVAO == true, "Not using VAO");
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(T) * nElements, data, GL_STATIC_DRAW);
	}

	uint GetVAO() 
	{
		return VAO;
	}

private:

	//resource shader
	ResourceShader* resShader;
	//render type enum (array, indices, instancing)
	RENDER_TYPE type;

	uint VAO;	//	General buffer
	uint EBO;	//	Indices buffer

	std::vector<uint> VBOs;	//Layout buffers


	bool usingVAO = false;
};