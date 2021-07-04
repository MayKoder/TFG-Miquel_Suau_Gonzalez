#include "MO_GUI.h"
#include "MO_Window.h"
#include "MO_ResourceManager.h"

#include "MO_Renderer3D.h"
#include "CO_Camera.h"

#include "MathGeoLib/include/Math/Quat.h"
#include "Application.h"
#include "OpenGL.h"

#include"RE_Shader.h"

M_GUI::M_GUI(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	float positionX = 0.0f / 100.f;
	float positionY = 0.0f / 100.f;

	float sizeX = 1.0f / App->moduleWindow->s_width;
	float sizeY = 1.0f / App->moduleWindow->s_height;


	float scaleX = sizeX / 100.f;
	float scaleY = sizeY / 100.f;


	uiWorldProjection = float4x4::FromTRS(float3(positionX, positionY, 0), Quat::FromEulerXYZ(0, 0, 0.0f), float3(scaleX, scaleY, 1));
}

M_GUI::~M_GUI()
{
	glDeleteBuffers(1, &VAO);
	VAO = 0;
}

bool M_GUI::Start()
{

	uiShader = dynamic_cast<ResourceShader*>(App->moduleResources->RequestResource(App->GetRandomInt(), "Library\/Shaders\/1557502301.shdr"));

	//Generate ui buffer
	glGenBuffers(1, &VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(uiPlaneData), uiPlaneData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)(2 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

bool M_GUI::CleanUp()
{
	App->moduleResources->UnloadResource(uiShader->GetUID());
	return true;
}

void M_GUI::RenderUIElements()
{

	uiShader->Bind();

	GLint modelLoc = glGetUniformLocation(uiShader->shaderProgramID, "view");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, App->moduleRenderer3D->activeRenderCamera->ViewMatrixOpenGL().ptr());

	modelLoc = glGetUniformLocation(uiShader->shaderProgramID, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, uiWorldProjection.Transposed().ptr());


	glBindBuffer(GL_ARRAY_BUFFER, VAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)(2 * sizeof(GLfloat)));

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	uiShader->Unbind();
}
