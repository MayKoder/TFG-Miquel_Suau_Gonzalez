#pragma once
#include "Module.h"
#include "UIElement.h"

static float uiPlaneData[] = 
{
-1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0
	/*0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0,*/
};

//template<typename... Args>
class M_GUI : public Module
{

public:
	M_GUI(Application* app, bool start_enabled = true);
	virtual ~M_GUI();

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
};