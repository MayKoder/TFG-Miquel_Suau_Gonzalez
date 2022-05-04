#pragma once

#include"DEResource.h"
#include<vector>
typedef unsigned int GLuint;
class ResourceMaterial;
class ResourceShader;
#include"OpenGL.h"
#include"MathGeoLib/include/Math/float2.h"
#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Math/float4.h"
#include"MathGeoLib/include/Math/float3x3.h"

enum class ShaderType
{
	SH_Vertex,
	SH_Frag,
	SH_Max
};

class ResourceShader : public Resource
{

public:
	ResourceShader(std::string& _uid);
	~ResourceShader();

	void LinkToProgram();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

	void Bind();
	void Unbind();

	void SetFloat(const char* inputName, float& value) {
		GLint modelLoc = glGetUniformLocation(this->shaderProgramID, inputName);
		if (modelLoc != -1) {
			glUniform1fv(modelLoc, 1, &value);
		}
		else {

		}
	}
	void SetVector2(const char* inputName, float2& value) {
		GLint modelLoc = glGetUniformLocation(this->shaderProgramID, inputName);
		if (modelLoc != -1) {
			glUniform2fv(modelLoc, 1, value.ptr());
		}
		else {

		}
	}
	void SetVector3(const char* inputName, float3& value) {
		GLint modelLoc = glGetUniformLocation(this->shaderProgramID, inputName);
		if (modelLoc != -1) {
			glUniform3fv(modelLoc, 1, value.ptr());
		}
		else {

		}
	}
	void SetVector4(const char* inputName, float4& value) {
		GLint modelLoc = glGetUniformLocation(this->shaderProgramID, inputName);
		if (modelLoc != -1) {
			glUniform4fv(modelLoc, 1, value.ptr());
		}
		else {

		}
	}
	void SetMatrix3(const char* inputName, float3x3& value) {
		GLint modelLoc = glGetUniformLocation(this->shaderProgramID, inputName);
		if (modelLoc != -1) {
			glUniformMatrix3fv(modelLoc, 1, GL_FALSE, value.ptr());
		}
		else {

		}
	}
	void SetMatrix4(const char* inputName, float4x4& value) {
		GLint modelLoc = glGetUniformLocation(this->shaderProgramID, inputName);
		if (modelLoc != -1) {
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value.ptr());
		}
		else {

		}
	}

	char* SaveShaderCustomFormat(char* vertexObjectBuffer, int vofSize, char* fragObjectBuffer, int fobSize);
	void LoadShaderCustomFormat(const char*);

	GLuint shaderObjects[static_cast<int>(ShaderType::SH_Max)];

	GLuint shaderProgramID;

	std::vector<ResourceMaterial*> references;
};