#include "MO_GUI.h"
#include "MO_Window.h"
#include "MO_ResourceManager.h"
#include "MO_Renderer3D.h"
#include "MO_Input.h"

#include "CO_Camera.h"

#include "Application.h"

#include"RE_Shader.h"

#include "ImGui/backends/imgui_impl_sdl.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "OpenGL.h"

M_GUI::M_GUI(Application* app, bool start_enabled) : Module(app, start_enabled), uiShader(nullptr)
{
	root = nullptr;
	//root = new UIElement(nullptr, float2::zero, float2::zero, float2::one);
}

M_GUI::~M_GUI()
{
	//assert(root == nullptr, "UI Not cleaned up");
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	VAO = 0; VBO = 0;
}

bool M_GUI::Init()
{
	//TODO: Move this to gui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls

	ImGui::StyleColorsDark();
	bool test2 = ImGui_ImplSDL2_InitForOpenGL(App->moduleWindow->window, App->moduleRenderer3D->context);
	bool test = ImGui_ImplOpenGL3_Init();

	io.MouseDrawCursor = false;
	io.IniFilename = NULL;

	return true;
}

bool M_GUI::Start()
{

	float positionX = 0.0f / 100.f;
	float positionY = 50.0f / 100.f;

	float sizeX = 200.0f / App->moduleWindow->s_width;
	float sizeY = 200.0f / App->moduleWindow->s_height;

	//TODO: Lots to do here, UI is trash, moving on onto new things atm
	root = new UIElement(nullptr, float2::zero, float2::zero, float2::one);
	//root = new UIElement(nullptr, float2::zero, float2::zero, float2(EngineExternal->moduleWindow->s_width, EngineExternal->moduleWindow->s_height));

	UIElement* floatingParent = AddUIElement(nullptr, float2(-1.0 + 0.15, 0.0f), float2(0, 0), float2(0.15f, 0.8f));
	//Not a fan of "auto" but in this case, it is faster
	auto testu = [](UIElement* element, bool& state, int side)
	{
		//for side, 0 = left, 1 = right, 2 = bottom
		float3 test = element->parent->localTransform.Col3(3);
		float3 size = element->parent->localTransform.GetScale();

		switch (side)
		{

		case 0:
			(state == true) ? test.x -= size.x * 2.0f : test.x += size.x * 2.0f;
			break;

		case 1:
			(state != true) ? test.x -= size.x * 2.0f : test.x += size.x * 2.0f;
			break;

		case 2: 
			(state == true) ? test.y -= size.y * 2.0f : test.y += size.y * 2.0f;
			break;

		default:
			break;
		}

		state = !state;

		element->parent->localTransform.SetCol3(3, test);

		element->parent->UpdateTransform();
	};
	UIButton* b1 = dynamic_cast<UIButton*>(AddUIButton(floatingParent, float2(1.0 + 0.1, 0.f), float2(0.f, 0.f), float2(0.1f, 0.15f)));
	b1->SetupCallback(testu, b1, true, 0);

	floatingParent = AddUIElement(nullptr, float2(1.0 - 0.15, 0.0f), float2(0, 0), float2(0.15f, 0.8f));
	b1 = dynamic_cast<UIButton*>(AddUIButton(floatingParent, float2(-1.0 - 0.1, 0.f), float2(0.f, 0.f), float2(0.1f, 0.15f)));
	b1->SetupCallback(testu, b1, true, 1);


	/*UIElement* test =*/floatingParent = AddUIElement(nullptr, float2(0.0, -1.0f + 0.2f), float2(0, 0), float2(0.5f, 0.2f));

	float aspect = (float)EngineExternal->moduleWindow->s_width / (float)EngineExternal->moduleWindow->s_height;
	//AddUIElement(floatingParent, float2(0.0, 0.0f), float2(0, 0), float2(.975f, .9f))/*->SetOffset(10, 10, 10, 10)*/;


	b1 = dynamic_cast<UIButton*>(AddUIButton(floatingParent, float2(0.0, 1.0 + 0.1), float2(0.f, 0.f), float2(0.15f, 0.1f)));
	b1->SetupCallback(testu, b1, true, 2);

	AddUIElementRow(floatingParent, 6);

	/*AddUIElement(AddUIElement(root, float2::zero, float2::zero, float2(0.9, 0.5)), float2::zero, float2::zero, float2(0.9, 0.9));*/

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



	//WILL USE IMGUI FOR NOW
	imGuiPanels[0].Set(ImVec2(0.0, 0.5), ImVec2(0, App->moduleWindow->s_height / 2), ImVec2(App->moduleWindow->s_width / 7, App->moduleWindow->s_height / 1.4), ImVec2(1.0, 0.0));
	imGuiPanels[1].Set(ImVec2(1.0, 0.5), ImVec2(App->moduleWindow->s_width, App->moduleWindow->s_height / 2), ImVec2(App->moduleWindow->s_width / 7, App->moduleWindow->s_height / 1.4), ImVec2(-1.0, 0.0));
	imGuiPanels[2].Set(ImVec2(0.5, 1.0), ImVec2(App->moduleWindow->s_width / 2, App->moduleWindow->s_height), ImVec2(App->moduleWindow->s_width / 2, App->moduleWindow->s_height / 4), ImVec2(0.0, -1.0));

	return true;
}

update_status M_GUI::Update(float dt)
{
	//TODO: Move this inside renderer or maybe input update loop?
	if (App->moduleInput->GetMouseButton(1) == KEY_STATE::KEY_DOWN) 
	{
		RecursiveUpdateElements(root);
	}
	return update_status::UPDATE_CONTINUE;
}

bool M_GUI::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();


	App->moduleResources->UnloadResource(uiShader->GetUID());
	
	delete root;
	root = nullptr;

	return true;
}

void M_GUI::RenderUIElements()
{
	//uiShader->Bind();

	//root->RenderElement(VAO, uiShader);

	//uiShader->Unbind();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();


	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
	for (size_t i = 0; i < 3; i++)
	{
		PanelTemp* panel = &imGuiPanels[i];
		ImGui::SetNextWindowPos(panel->pos, 0, panel->pivot);
		ImGui::SetNextWindowSize(panel->size);
		if (ImGui::Begin(std::to_string(i).c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
		{
		}
		ImGui::End();




		ImVec2 size = ImVec2(App->moduleWindow->s_width / 10, App->moduleWindow->s_height / 25);
		ImVec2 p0 = ImVec2(panel->pos.x + (panel->size.x * panel->buttonAnchor.x) - (size.x / 2), panel->pos.y + (panel->size.y * panel->buttonAnchor.y) - size.y);
		ImVec2 p1 = ImVec2(p0.x + size.x, p0.y + size.y);

		ImU32 col_b = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
		draw_list->AddRectFilled(p0, p1, col_b);
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) == true) {
			ImVec4 rect = ImVec4(p0.x, p0.y, size.x, size.y);
			ImVec2 point = ImGui::GetMousePos();
			if ((point.x <= rect.x + rect.z && point.x >= rect.x) &&
				point.y <= rect.y + rect.w && point.y >= rect.y)
			{
				LOG(LogType::L_NORMAL, "yyyyyyay %f, %f", point.x, point.y);
			}
		}
	}


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

//Will work with correct offsets?
void M_GUI::AddUIElementRow(UIElement* parent, int numberOfElements)
{
	if (parent == nullptr)
		return;

	float3 position = parent->localTransform.Col3(3);
	float3 size = parent->localTransform.GetScale();

	float spanSize = size.x * 2.0f;
	float spanOffset = spanSize / (numberOfElements - 1);

	float currentOffset = position.x - size.x;
	for (size_t i = 0; i < numberOfElements - 1; i++)
	{
		AddUIElement(parent, float2(currentOffset, 0.0f), float2::zero, float2(spanOffset, 0.4));
		currentOffset += spanOffset;
	}

	return;
}

void M_GUI::OnResize(int width, int height)
{
	//delete root;
	//root = new UIElement(nullptr, float2::zero, float2::zero, float2((float)width, (float)height));
	//root->localTransform = float4x4::FromTRS(float3::zero, Quat::FromEulerXYZ(0.0f, 0.0f, 0.0f), float3((float)width, (float)height, 0.0f));
	//root->UpdateTransform();
}
