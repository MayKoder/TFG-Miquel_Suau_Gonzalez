#include "MO_GUI.h"
#include "MO_Window.h"
#include "MO_ResourceManager.h"
#include "MO_Renderer3D.h"
#include "MO_Input.h"

#include "CO_Camera.h"

#include "Application.h"
#include "OpenGL.h"

#include"RE_Shader.h"

M_GUI::M_GUI(Application* app, bool start_enabled) : Module(app, start_enabled), uiShader(nullptr)
{
	root = new UIElement(nullptr, float2::zero, float2::zero, float2::one);
}

M_GUI::~M_GUI()
{
	//assert(root == nullptr, "UI Not cleaned up");
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
	UIElement* test = AddUIElement(nullptr, float2(0.0, -1.0f + 0.2f), float2(0, 0), float2(0.5f, 0.5f));

	UIElement* left = AddUIElement(nullptr, float2(-1.0 + 0.15, 0.0f), float2(0, 0), float2(0.15f, 0.8f));

	std::function<void(UIElement*, bool&)> testu = [](UIElement* element, bool& state)
	{
		element->parent->colorRGBA = float4::one / 2.f;

		//float3 test = transformGL.Row3(3);
		//test.y += 0.02;
		//transformGL.SetRow3(3, test);

		float3 test = element->parent->localTransform.Col3(3);
		float3 size = element->parent->localTransform.GetScale();

		(state == true) ? test.x -= size.x * 2 : test.x += size.x * 2;
		state = !state;

		element->parent->localTransform.SetCol3(3, test);

		element->parent->UpdateTransform();
	};
	AddUIButton(left, float2(1.0 + 0.1, 0.f), float2(0.f, 0.f), float2(0.1f, 0.15f), testu);

	AddUIElement(nullptr, float2(1.0 - 0.15, 0.0f), float2(0, 0), float2(0.15f, 0.8f));
	AddUIElement(test, float2(0.0f, 0.0f), float2(0, 0), float2(0.95, 0.95));

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

bool M_GUI::RecursiveUpdateElements(UIElement* element)
{
	//Doing a bottom-up iteration to respect ordering?
	for (size_t i = 0; i < element->children.size(); i++)
	{
		if (RecursiveUpdateElements(element->children[i])) {
			return false;
		}
	}

	//TODO: Maybe iteration would be better than recursivity?
	if (element != root && element->IsInside(float2(App->moduleInput->GetMouseX(), App->moduleInput->GetMouseY()))) 
	{
		//Use
		element->OnClick();
		return true;
	}

	return false;
	//Iterate tree checking if pointer is inside
	//Use button elements, use the first one
	//exit loop
}

UIElement* M_GUI::AddUIElement(UIElement* parent, float2 pos, float2 rot, float2 scale)
{
	if (parent == nullptr) {
		parent = root;
	}

	UIElement* ret = new UIElement(parent, pos, rot, scale);
	parent->children.push_back(ret);

	return ret;
}
