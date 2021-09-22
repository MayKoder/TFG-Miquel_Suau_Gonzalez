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
	GL_Object(RENDER_TYPE _type);
	~GL_Object();

	void InitBuffers();
	void UnloadBuffers();

	void Bind();
	void UnBind();

	/*Returns the index for the new VBO inside the VBOs' vector*/
	uint CreateVBO();

	template <class T>
	void SetVBO(int VBOindex, T* data, int nElements, int drawType = GL_STATIC_DRAW) 
	{
		lastElementSize = nElements;
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[VBOindex]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * nElements, data, drawType);
	}

	template <class T>
	/*
	* Only use to CHANGE data values or ranges, never use if the data value is larger than the first set, for that, use SetVBO()
	*/
	void UpdateVBO(int VBOindex, int offset, T* data, int nElements) {
		assert(nElements == lastElementSize, "Never use UpdateVBO is the data array is growing!");
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[VBOindex]);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(T) * nElements, data);
	}

	template <class T>
	void CreateAndSetVBO(T* data, int nElements, int drawType = GL_STATIC_DRAW) 
	{
		SetVBO(CreateVBO(), data, nElements, drawType);
	}


	/*
	* Index = attrib index
	* attribSize = number of elements (ex: Vec3 = 3 elements)
	* packElements = in case of packed elements, the total element size (ex: position + color = vec3 + vec4 = 3 + 4)
	* offset = in case of packed elements, the number of elements to the start of this attrib (ex: position + color, color would be 3, as there is a 3 element offset, 0 to 3 for position, then color starts at 3)
	*/
	void SetVertexAttrib(int index, int attribSize, int packElements, int offset);

	void SetAttribDivisor(int index, uint attribDivisor);

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

	//Safe logic
	bool usingVAO = false;
	int lastElementSize = 0;
};