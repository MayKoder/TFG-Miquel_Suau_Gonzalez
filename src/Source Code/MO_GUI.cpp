#include "MO_GUI.h"
#include "MO_Window.h"
#include "MO_ResourceManager.h"
#include "MO_Renderer3D.h"
#include "MO_Input.h"
#include "MO_Scene.h"
#include "MO_Camera3D.h"

#include "CO_Camera.h"
#include"CO_Transform.h"

#include "Application.h"

#include"RE_Shader.h"

#include"MMGui.h"

//ImGui Includes
#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_sdl.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "SDL/include/SDL.h"
#include "OpenGL.h"

#include "Tween.h"
#include"CreationTool.h"

#include"GameObject.h"
#include"IM_TextureImporter.h"

M_GUI::M_GUI(Application* app, bool start_enabled) : Module(app, start_enabled), uiShader(nullptr), selectedTool(nullptr)
{
	root = nullptr;
	memset(uiTools, NULL, sizeof(uiTools));
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
	//this->uiTools[0] = new ToolNode("Nodes");
	//this->uiTools[1] = new ToolWall("Walls");

	developerWindow.SetTextFromFile("Assets/Shaders/cubeMap.glsl");

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

	uiShader = dynamic_cast<ResourceShader*>(App->moduleResources->RequestResource("Assets/Shaders/uiRender.glsl", Resource::Type::SHADER));

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uiPlaneData), uiPlaneData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	//------------------------------------ WILL USE IMGUI FOR NOW ----------------------------------------//
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls

	ImGui::GetStyle().WindowRounding = 13.f;
	ImGui::GetStyle().FrameRounding = 13.f;

	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(App->moduleWindow->window, App->moduleRenderer3D->context);
	ImGui_ImplOpenGL3_Init();

	io.MouseDrawCursor = false;
	io.IniFilename = NULL;

	SetPanelData(App->moduleWindow->s_width, App->moduleWindow->s_height);

	PanelTemp* send = &imGuiPanels[0];
	std::function<void(int)> customDrawCalls = [&, send] (int i)
	{
		ImGui::GetStyle().RoundingStyleFlag = ImDrawCornerFlags_Right;
		ImGui::SetNextWindowPos(send->animator.GetAndStep(EngineExternal->GetDT()), 0, send->pivot);
		//ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowRounding, ImDrawCornerFlags_Right);
		if (ImGui::Begin(std::to_string(i).c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
		{
			//if (selectedTool != nullptr) {
			//	selectedTool->DrawEditor();
			//}
			GameObject* light = App->moduleScene->root->children[1];
			C_Transform* trasform = dynamic_cast<C_Transform*>(light->components[0]);


			ImGui::AddMenuHeaderCustom("Light settings", 0);

			int offset = ImGui::CalcTextSize("Sky tint: ").x + 15;
			ImGui::AddTitleCustom("Position: ", offset);
			if (ImGui::DragFloat3("##lPos", &trasform->position[0], 0.1f))
				trasform->updateTransform = true;


			for (auto it = light->components.begin()+1; it < light->components.end(); ++it)
			{
				(*it)->OnEditor();
			}

			ImGui::AddMenuHeaderCustom("Skybox settings", 10);

			ImGui::AddTitleCustom("Enable: ", offset);
			ImGui::Checkbox("##renderSkybox", &App->moduleRenderer3D->renderSkybox);

			ImGui::AddTitleCustom("Sky tint: ", offset);
			ImGui::ColorEdit3("##skyTint", App->moduleRenderer3D->skybox.cubemapTintColor.ptr());

			ImGui::AddTitleCustom("Tint mult: ", offset);
			ImGui::DragFloat("##tintMultiplier", &App->moduleRenderer3D->skybox.tintIntensity, .01, 0.0, 1.0);
			
			ImGui::AddMenuHeaderCustom("Debug toggles", 10);
			
			ImGui::AddTitleCustom("Draw vert: ", offset);
			ImGui::Checkbox("##debugRenderLines", &App->moduleRenderer3D->displayDebugVertices);

			ImGui::AddTitleCustom("Draw box: ", offset);
			ImGui::Checkbox("##debugRenderBoxes", &App->moduleRenderer3D->displayDebugBoxes);

			for (int i = 0; i < 5; i++) {
				ImGui::Spacing();
			}
			if (ImGui::MiddleButton("Take screenshot", ImGui::GetWindowSize(), 0.5f)) {
				TextureImporter::TakeScreenshot(App->moduleCamera->editorCamera.msaaFBO.GetFrameBuffer());
			}

		}
		ImGui::End();
		//ImGui::PopStyleVar();
		//ImGui::GetStyle().RoundingStyleFlag = ImDrawCornerFlags_None;
	};
	send->drawCallback = customDrawCalls;

	send = &imGuiPanels[1];
	customDrawCalls = [&, send](int i)
	{
		ImGui::GetStyle().RoundingStyleFlag = ImDrawCornerFlags_Left;
		ImGui::SetNextWindowPos(send->animator.GetAndStep(EngineExternal->GetDT()), 0, send->pivot);
		if (ImGui::Begin(std::to_string(i).c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
		{
			//DrawGameObjectsTree(App->moduleScene->root, false);
			if (App->moduleRenderer3D->renderQueue[0] != nullptr)
				App->moduleRenderer3D->renderQueue[0]->DrawOptionsMenu();
		}
		ImGui::End();
	};
	send->drawCallback = customDrawCalls;

	//send = &imGuiPanels[1];
	//customDrawCalls = [this, send](int i)
	//{
	//	ImGui::GetStyle().RoundingStyleFlag = ImDrawCornerFlags_Top;
	//	ImGui::SetNextWindowPos(send->animator.GetAndStep(EngineExternal->GetDT()), 0, send->pivot);
	//	if (ImGui::Begin(std::to_string(i).c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysHorizontalScrollbar))
	//	{
	//		if (ImGui::IsWindowFocused() && ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x != 0.0)
	//			ImGui::SetScrollX(ImGui::GetScrollX() - ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x);
	//		for (size_t i = 0; i < UI_TOOLS_MAX; i++)
	//		{

	//			bool colorChanged = false;
	//			if (selectedTool == uiTools[i]) {
	//				colorChanged = true;
	//				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
	//			}
	//			//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, b, b));
	//			//ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, c, c));

	//			//if (ImGui::Button(uiTools[i]->GetName(), ImVec2(ImGui::GetContentRegionAvail().y, ImGui::GetContentRegionAvail().y))) {
	//			//	
	//			//	(this->selectedTool == uiTools[i]) ? selectedTool = nullptr : this->selectedTool = uiTools[i];
	//			//}
	//			ImGui::SameLine();

	//			if (colorChanged == true)
	//				ImGui::PopStyleColor(1);
	//		}
	//	}
	//	ImGui::End();
	//};
	//send->drawCallback = customDrawCalls;

	//test.Set(0, 1, 0.1, false);
	//test.isActive = true;
	//test.stepOverride = CustomEasings::easeInOutBounce;

	return true;
}

update_status M_GUI::Update(float dt)
{
	//TODO: Move this inside renderer or maybe input update loop?
	if (App->moduleInput->GetMouseButton(1) == KEY_STATE::KEY_DOWN) 
	{
		RecursiveUpdateElements(root);
	}
	if (App->moduleInput->GetKey(SDL_SCANCODE_P) == KEY_STATE::KEY_DOWN)
	{
		developerWindow.active = !developerWindow.active;
	}
	return update_status::UPDATE_CONTINUE;
}

bool M_GUI::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	for (size_t i = 0; i < UI_TOOLS_MAX; i++)
	{
		delete uiTools[i];
		uiTools[i] = nullptr;
	}

	App->moduleResources->UnloadResource(uiShader->GetAssetPath());
	
	delete root;
	root = nullptr;

	return true;
}

void M_GUI::RenderUIElements()
{
	//uiShader->Bind();

	//root->RenderElement(VAO, uiShader);

	//uiShader->Unbind();

	//glPointSize(50);
	//glBegin(GL_POINTS);
	//glVertex3f(0.f, test.GetAndStep(App->GetDT()), 0.0f);
	//glEnd();
	//glPointSize(1.0);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();


	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
	ImGuiStyle& style = ImGui::GetStyle();

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AnyWindow)) 
	{
		App->moduleInput->SetMouseLayer(MOUSE_LAYER::HOVERING_UI);
	}


	for (size_t i = 0; i < 2; i++)
	{
		PanelTemp* panel = &imGuiPanels[i];
		if (panel->isOpen == true) 
		{
			ImGui::SetNextWindowSize(panel->size);
			panel->drawCallback(i);
		}


		ImVec2 size = ImVec2(panel->buttonRect.x, panel->buttonRect.y);
		ImVec2 p0 = ImVec2(panel->animator.Get().x + panel->closeOffset.x, panel->animator.Get().y + panel->closeOffset.y);
		ImVec2 p1 = ImVec2(p0.x + size.x, p0.y + size.y);

		ImU32 col_b = ImGui::GetColorU32(IM_COL32(200, 200, 200, 230));
		draw_list->AddRectFilled(p0, p1, col_b, style.WindowRounding, style.RoundingStyleFlag);

		if (panel->OnHover(p0)) {
			App->moduleInput->SetMouseLayer(MOUSE_LAYER::HOVERING_UI);
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) == true)
			{
				panel->OnToggleClick(p0);
			}
		}
	}

	if(developerWindow.active)
		developerWindow.Draw();


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
	SetPanelData(width, height);
}

void M_GUI::SetPanelData(int w, int h)
{

	PanelTemp* send = &imGuiPanels[0];
	send->Set(
		ImVec2(0.0, 0.5), 
		ImVec2(0, h / 2), 
		ImVec2(w / 7, h / 1.4), 
		ImVec2(w / 50, h / 8),
		ImVec2(w / 7, -(h/8) / 2));
	send->animator.Set(send->pos, float2(send->pos.x - send->size.x, send->pos.y), .25f, true);
	send->animator.Invert();

	send = &imGuiPanels[1];
	imGuiPanels[1].Set(
		ImVec2(1.0, 0.5), 
		ImVec2(w, h / 2), 
		ImVec2(w / 7, h / 1.4), 
		ImVec2(w / 50, h / 8),
		ImVec2(-(w / 7) - (w/50), -(h/8)/2));
	send->animator.Set(send->pos, float2(w + send->size.x, send->pos.y), .25f, true);
	send->animator.Invert();

	//send = &imGuiPanels[2];
	//imGuiPanels[2].Set(
	//	ImVec2(0.5, 1.0), 
	//	ImVec2(w / 2, h), 
	//	ImVec2(w / 2, h / 4), 
	//	ImVec2(w / 10, h / 25),
	//	ImVec2(-(w/10)/2, -(h / 4) - h/25));
	//send->animator.Set(send->pos, float2(send->pos.x, send->pos.y + send->size.y), .25f, true);
	//send->animator.Invert();
}

//void M_GUI::DrawGameObjectsTree(GameObject* node, bool drawAsDisabled)
//{
//	if (drawAsDisabled == false)
//		drawAsDisabled = !node->isActive();
//
//	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
//
//	if (node->children.size() == 0)
//		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
//
//	//if (node == EngineExternal->moduleEditor->GetSelectedGO())
//	//	flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;
//
//
//	if (drawAsDisabled)
//		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
//
//	bool nodeOpen = ImGui::TreeNodeEx(node, flags, node->name.c_str());
//
//	if (drawAsDisabled)
//		ImGui::PopStyleColor();
//
//	//Only can use if this is not the root node
//	//ASK: Should the root node really be a gameobject? Problems with checks
//	if (!node->IsRoot())
//	{
//		//Start drag for reparent
//		if (ImGui::BeginDragDropSource(/*ImGuiDragDropFlags_SourceNoDisableHover*/))
//		{
//			ImGui::SetDragDropPayload("_GAMEOBJECT", node, sizeof(GameObject*));
//
//			//dropTarget = node;
//
//			ImGui::Text("Change parent to...");
//			ImGui::EndDragDropSource();
//		}
//
//		//if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
//		//{
//		//	EngineExternal->moduleEditor->SetSelectedGO(node);
//		//	if (EngineExternal->moduleEditor->GetSelectedAsset() != nullptr)
//		//		EngineExternal->moduleEditor->SetSelectedAsset(nullptr);
//		//}
//	}
//
//	node->showChildren = (node->children.size() == 0) ? false : nodeOpen;
//
//	//All nodes can be a drop target
//	if (ImGui::BeginDragDropTarget())
//	{
//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT"))
//		{
//
//			GameObject* dropGO = static_cast<GameObject*>(payload->Data);
//			//memcpy(dropGO, payload->Data, payload->DataSize);
//
//			//dropTarget->ChangeParent(node);
//			//LOG(  "%s", dropTarget->name.c_str());
//			//dropTarget = nullptr;
//		}
//		ImGui::EndDragDropTarget();
//	}
//
//
//	if (node->showChildren == true)
//	{
//
//		for (unsigned int i = 0; i < node->children.size(); i++)
//		{
//			DrawGameObjectsTree(node->children[i], drawAsDisabled);
//		}
//		ImGui::TreePop();
//	}
//}
