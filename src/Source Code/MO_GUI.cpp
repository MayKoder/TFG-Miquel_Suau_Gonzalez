#include "MO_GUI.h"
#include "MO_Window.h"
#include "MO_ResourceManager.h"
#include "MO_Renderer3D.h"
#include "MO_Input.h"

#include "CO_Camera.h"

#include "MathGeoLib/include/Math/Quat.h"
#include "Application.h"
#include "OpenGL.h"

#include"RE_Shader.h"

M_GUI::M_GUI(Application* app, bool start_enabled) : Module(app, start_enabled), uiShader(nullptr)
{
	root = new UIElement(nullptr, float2::zero, float2::zero, float2::one);
}

M_GUI::~M_GUI()
{
	assert(root == nullptr, "UI Not cleaned up");
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
	UIElement* test = AddUIElement(nullptr, float2(0.0, -1.0f + 0.2f), float2(0, 0), float2(0.5f, 0.2f));
	AddUIElement(nullptr, float2(-1.0 + 0.15, 0.0f), float2(0, 0), float2(0.15f, 0.8f));
	AddUIElement(nullptr, float2(1.0 - 0.15, 0.0f), float2(0, 0), float2(0.15f, 0.8f));
	AddUIElement(test, float2(0.0f, 0.0f), float2(0, 0), float2(0.15f, 0.15f));

	uiShader = dynamic_cast<ResourceShader*>(App->moduleResources->RequestResource(App->GetRandomInt(), "Library\/Shaders\/1569048839.shdr"));

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

update_status M_GUI::Update(float dt)
{
	if (App->moduleInput->GetMouseButton(1) == KEY_STATE::KEY_DOWN) 
	{
		RecursiveUpdateElements(root);
	}
	return update_status::UPDATE_CONTINUE;
}

bool M_GUI::CleanUp()
{
	App->moduleResources->UnloadResource(uiShader->GetUID());
	
	delete root;
	root = nullptr;

	return true;
}

void M_GUI::RenderUIElements()
{
	uiShader->Bind();

	root->RenderElement(VAO, uiShader);

	uiShader->Unbind();
}

void M_GUI::RecursiveUpdateElements(UIElement* element)
{
	//Doing a bottom-up iteration to respect ordering?
	for (size_t i = 0; i < element->children.size(); i++)
	{
		RecursiveUpdateElements(element->children[i]);
	}

	//TODO: Maybe iteration would be better than recursivity?
	if (element != root && element->IsInside(float2(App->moduleInput->GetMouseX(), App->moduleInput->GetMouseY()))) 
	{
		//Use
		element->OnClick();
	}

	//Iterate tree checking if pointer is inside
	//Use button elements, use the first one
	//exit loop
}

M_GUI::UIElement* M_GUI::AddUIElement(UIElement* parent, float2 pos, float2 rot, float2 scale)
{
	if (parent == nullptr) {
		parent = root;
	}

	UIElement* ret = new UIElement(parent, pos, rot, scale);
	parent->children.push_back(ret);

	return ret;
}

//M_GUI::UIElement::UIElement() : parent(nullptr), colorRGBA(float4::one)
//{
//	this->transformGL = float4x4::FromTRS(float3(0, 0, 0), Quat::FromEulerXYZ(0.f, 0.f, 0.0f), float3(1.f, 1.f, 1.f)).Transposed();
//}

M_GUI::UIElement::UIElement(UIElement* _parent, float2 pos, float2 rot, float2 scale) : parent(_parent), colorRGBA(float4::one)
{
	if (parent != nullptr) 
	{
		this->transformGL = parent->transformGL * float4x4::FromTRS(float3(pos.x, pos.y, 0), Quat::FromEulerXYZ(rot.x, rot.y, 0.0f), float3(scale.x, scale.y, 1)).Transposed();
	}
	else
	{
		colorRGBA = float4::zero;
		this->transformGL = float4x4::FromTRS(float3(pos.x, pos.y, 0), Quat::FromEulerXYZ(rot.x, rot.y, 0.0f), float3(scale.x, scale.y, 1)).Transposed();
	}
}

M_GUI::UIElement::~UIElement()
{
	for (size_t i = 0; i < children.size(); i++)
	{
		delete children[i];
		children[i] = nullptr;
	}
	children.clear();
}

void M_GUI::UIElement::OnClick()
{
	colorRGBA = float4::one / 2.f;
}

void M_GUI::UIElement::RenderElement(unsigned int VAO, ResourceShader* shader)
{
	GLint modelLoc = glGetUniformLocation(shader->shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, this->transformGL.ptr());

	modelLoc = glGetUniformLocation(shader->shaderProgramID, "inputColor");
	glUniform4fv(modelLoc, 1, &this->colorRGBA.x);

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->RenderElement(VAO, shader);
	}
}

bool M_GUI::UIElement::IsInside(float2 point)
{
	float3 pos;
	Quat rot;
	float3 size;

	//Normalize point
	point.x /= EngineExternal->moduleWindow->s_width;
	point.y /= EngineExternal->moduleWindow->s_height;

	point.x = point.x - (1 - point.x);
	point.y = -(point.y - (1 - point.y));

	this->transformGL.Transposed().Decompose(pos, rot, size);

	if ((point.x <= pos.x + size.x && point.x >= pos.x - size.x) && 
		point.y <= pos.y + size.y && point.y >= pos.y - size.y) 
	{
		return true;
	}

	return false;
}
