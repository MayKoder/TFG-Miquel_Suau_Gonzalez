#include "GL_Object.h"

GL_Object::GL_Object(RENDER_TYPE _type) : resShader(nullptr), type(_type), VAO(0),
EBO(0)
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
