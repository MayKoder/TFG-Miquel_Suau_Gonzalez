#include "DE_Cubemap.h"
#include"OpenGL.h"
#include"RE_Shader.h"
#include"CO_Camera.h"
#include"MathGeoLib/include/Math/float4x4.h"
#include"Application.h"
#include"MO_ResourceManager.h"

DE_Cubemap::DE_Cubemap() : shaderRes(nullptr), textureID(0) /*vboId(0),*//* EBO(0), VBO(0), VAO(0)*/
{
}

DE_Cubemap::~DE_Cubemap()
{
}

void DE_Cubemap::CreateGLData()
{
	//glGenBuffers(1, &vboId);
	//// bind VBO in order to use
	//glBindBuffer(GL_ARRAY_BUFFER, vboId);
	//// upload data to VBO
	//glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), skyboxVertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*)0);

	//------------------------//


	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, (GLuint*)&(VBO));
	//glGenBuffers(1, (GLuint*)&(EBO));

	//glBindVertexArray(VAO);
	skyboxObject.InitBuffers();
	skyboxObject.Bind();

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxIndexVertices), skyboxIndexVertices, GL_STATIC_DRAW);
	//skyboxObject.SetVBO(skyboxObject.CreateVBO(), skyboxIndexVertices, sizeof(skyboxIndexVertices) / sizeof(float));
	skyboxObject.CreateAndSetVBO(skyboxIndexVertices, sizeof(skyboxIndexVertices) / sizeof(float));

	////indices
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), skyboxIndices, GL_STATIC_DRAW);
	skyboxObject.LoadEBO(skyboxIndices, sizeof(skyboxIndices) / sizeof(int));

	//position attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	skyboxObject.SetVertexAttrib(0, 3, 6 * sizeof(float), 0 * sizeof(float), GL_FLOAT);

	//texcoords attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	skyboxObject.SetVertexAttrib(1, 3, 6 * sizeof(float), 3 * sizeof(float), GL_FLOAT);

	skyboxObject.UnBind();
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DE_Cubemap::ClearMemory()
{
	if (textureID != 0)
		glDeleteTextures(1, &textureID);

	//if (vboId != 0)
	//	glDeleteBuffers(1, &vboId);

	if (shaderRes)
		EngineExternal->moduleResources->UnloadResource(shaderRes->GetUID());


	//glDeleteVertexArrays(1, &VAO);
	//VAO = 0u;
	//glDeleteBuffers(1, &VBO);
	//VBO = 0u;
	//glDeleteBuffers(1, &EBO);
	//EBO = 0u;

	skyboxObject.UnloadBuffers();
}

void DE_Cubemap::DrawAsSkybox(C_Camera* _camera)
{
	glDepthFunc(GL_LEQUAL);
	//glDisable(GL_DEPTH_TEST);
	//glDepthFunc(GL_EQUAL);

	//glDepthMask(GL_FALSE);
	//glDepthRange(1.f, 1.f);
	//glEnable(GL_DEPTH_CLAMP);

	shaderRes->Bind();

	bool cameraNeedsChange = false;
	FrustumType cameraType = _camera->camFrustrum.type;

	Frustum saveCopy = _camera->camFrustrum;
	if (cameraType != FrustumType::PerspectiveFrustum)
	{
		cameraNeedsChange = true;
		_camera->camFrustrum.type = FrustumType::PerspectiveFrustum;
		_camera->camFrustrum.verticalFov = 60.0 * DEGTORAD;
		_camera->camFrustrum.horizontalFov = 2.0f * atanf(tanf(_camera->camFrustrum.verticalFov / 2.0f) * 1.7f);
	}

	GLint modelLoc = glGetUniformLocation(shaderRes->shaderProgramID, "view");
	float4x4 test = _camera->ViewMatrixOpenGL().Float3x3Part();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, test.ptr());

	modelLoc = glGetUniformLocation(shaderRes->shaderProgramID, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, _camera->ProjectionMatrixOpenGL().ptr());

	//glEnableClientState(GL_VERTEX_ARRAY);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	/*glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);*/
	//glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(skyboxObject.GetVAO());
	glDrawElements(GL_TRIANGLES, sizeof(skyboxIndices) / sizeof(int), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);

	//glDisableVertexAttribArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	//glDepthRange(0.f, 1.f);
	glDepthFunc(GL_LESS);
	//glDisable(GL_DEPTH_CLAMP);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	//glBindVertexArray(0);

	if (cameraNeedsChange)
		_camera->camFrustrum = saveCopy;

	shaderRes->Unbind();
}
