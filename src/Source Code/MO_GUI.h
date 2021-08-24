#pragma once
#include "Module.h"
#include "UIElement.h"

static float uiPlaneData[] = 
{
-1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0
	/*0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0,*/
};

#include"ImGui/imgui.h"
struct PanelTemp 
{
	PanelTemp() : pos(ImVec2(0.f, 0.f)), size(ImVec2(0.f, 0.f)), pivot(ImVec2(0.f, 0.f)), buttonAnchor(0.f, 0.f) {}

	void Set(ImVec2 _pivot, ImVec2 _pos, ImVec2 _size, ImVec2 _acPoint)
	{
		pos = _pos;
		size = _size;
		pivot = _pivot;
		buttonAnchor = _acPoint;
		//: pos(_pos), size(_size), pivot(_pivot)
	}

public: 
	ImVec2 size;
	ImVec2 pos;
	ImVec2 pivot;

	ImVec2 buttonAnchor;
	
	bool isOpen = false;
};

//template<typename... Args>
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
};