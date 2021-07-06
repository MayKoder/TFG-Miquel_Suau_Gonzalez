#include "MO_GUI.h"
#include "MO_Window.h"
#include "MO_ResourceManager.h"

#include "MO_Renderer3D.h"
#include "CO_Camera.h"

#include "MathGeoLib/include/Math/Quat.h"
#include "Application.h"
#include "OpenGL.h"

#include"RE_Shader.h"

M_GUI::M_GUI(Application* app, bool start_enabled) : Module(app, start_enabled), uiShader(nullptr)
{
}

M_GUI::~M_GUI()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	VAO = 0; VBO = 0;
}

bool M_GUI::Start()
{

	float positionX = 0.0f / 100.f;
	float positionY = 50.0f / 100.f;

	float sizeX = 200.0f / App->moduleWindow->s_width;
	float sizeY = 200.0f / App->moduleWindow->s_height;

	//float scaleX = sizeX / 100.f;
	//float scaleY = sizeY / 100.f;
	this->elements.push_back(UIElement(float2(0.0, -1.0f + 0.2f), float2(0, 0), float2(0.5f, 0.2f)));
	this->elements.push_back(UIElement(float2(-1.0 + 0.15, 0.0f), float2(0, 0), float2(0.15f, 0.8f)));
	this->elements.push_back(UIElement(float2(1.0 - 0.15, 0.0f), float2(0, 0), float2(0.15f, 0.8f)));

	uiShader = dynamic_cast<ResourceShader*>(App->moduleResources->RequestResource(App->GetRandomInt(), "Library\/Shaders\/1557502301.shdr"));

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uiPlaneData), uiPlaneData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

bool M_GUI::CleanUp()
{
	App->moduleResources->UnloadResource(uiShader->GetUID());
	elements.clear();
	return true;
}

void M_GUI::RenderUIElements()
{
	uiShader->Bind();

	for (size_t i = 0; i < elements.size(); i++)
	{
		elements[i].RenderElement(VAO, uiShader);
	}

	uiShader->Unbind();
}

M_GUI::UIElement::UIElement(float2 pos, float2 rot, float2 scale)
{
	this->transformGL = float4x4::FromTRS(float3(pos.x, pos.y, 0), Quat::FromEulerXYZ(rot.x, rot.y, 0.0f), float3(scale.x, scale.y, 1)).Transposed();
}

M_GUI::UIElement::~UIElement()
{
	children.clear();
}

void M_GUI::UIElement::RenderElement(unsigned int VAO, ResourceShader* shader)
{
	GLint modelLoc = glGetUniformLocation(shader->shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, this->transformGL.ptr());

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}
