#pragma once
#include "Module.h"
#include "UIElement.h"

static float uiPlaneData[] = 
{
-1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0
	/*0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0,*/
};

#include"ImGui/imgui.h"
#include"Tween.h"
struct PanelTemp 
{
	PanelTemp() : pos(0.f, 0.f), size(0.f, 0.f), pivot(0.f, 0.f), buttonRect(0.f, 0.f), closeOffset(0.f, 0.f) {}

	void Set(ImVec2 _pivot, ImVec2 _pos, ImVec2 _size, ImVec2 _acPoint, ImVec2 _closedOffset)
	{
		pos = _pos;
		size = _size;
		pivot = _pivot;
		buttonRect = _acPoint;
		closeOffset = _closedOffset;
		//: pos(_pos), size(_size), pivot(_pivot)
	}

	/*Shift vertical and horizontal by half the size, set to 0 to avoid shift, -1 to invert or 2 to shift full size*/
	//void ShiftHalfSize(float x = 1, float y = 1) 
	//{
	//	buttonRect.x += (buttonRect.z * 0.5f) * x;
	//	buttonRect.y += (buttonRect.w * 0.5f) * y;
	//}

	void OnToggleClick(ImVec2 buttonPos) 
	{

		ImVec2 p0 = buttonPos;
		ImVec2 size = ImVec2(buttonRect.x, buttonRect.y);

		ImVec4 rect = ImVec4(p0.x, p0.y, size.x, size.y);
		ImVec2 point = ImGui::GetMousePos();
		if ((point.x <= rect.x + rect.z && point.x >= rect.x) &&
			point.y <= rect.y + rect.w && point.y >= rect.y)
		{
			//isOpen = !isOpen;
			animator.Invert();
		}
	}

public: 
	ImVec2 size;
	ImVec2 pos;
	ImVec2 pivot;

	Tween<float2> animator;

	std::function<void(int)> drawCallback; //I'm fucking done, this is dumb, should have hardcoded all this shit

	/*X, Y = position // Z, W = button size*/
	ImVec2 buttonRect;
	ImVec2 closeOffset; //May think of a better way to do this
	
	bool isOpen = true;
};

//template<typename... Args>
#define UI_TOOLS_MAX 1
class Tool;
class M_GUI : public Module
{

public:
	M_GUI(Application* app, bool start_enabled = true);
	virtual ~M_GUI();

	bool Init() override;
	bool Start() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	void RenderUIElements();
	bool RecursiveUpdateElements(UIElement* element);

	UIElement* AddUIElement(UIElement* parent, float2, float2, float2);
	void AddUIElementRow(UIElement* parent, int numberOfElements);

	void OnResize(int, int) override;

	void SetPanelData(int, int);

	/*template <typename Func, typename... Args>*/
	UIElement* AddUIButton(UIElement* parent, float2 pos, float2 rot, float2 scale)
	{
		if (parent == nullptr) {
			parent = root;
		}

		UIButton* ret = new UIButton(parent, pos, rot, scale);
		parent->children.push_back(ret);
		return ret;
	}

	unsigned int VAO = 0;
	unsigned int VBO = 0;

	UIElement* root;

	ResourceShader* uiShader;

	PanelTemp imGuiPanels[3];

	Tool* uiTools[UI_TOOLS_MAX];
	Tool* selectedTool;

	Tween<float> test;
};