#pragma once
#include "Module.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/float4.h"
#include<vector>

class ResourceShader;

static float uiPlaneData[] = 
{
-1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0
};

class M_GUI : public Module
{

	struct UIElement 
	{

		UIElement(UIElement* _parent, float2 pos, float2 rot, float2 scale);
		~UIElement();

		/*virtual*/ void RenderElement(unsigned int VAO, ResourceShader* shader);
		bool IsInside(float2 point);
		
		UIElement* parent; //Just a pointer, non dynamic, does not need to be deleted
		std::vector<UIElement*> children;
		float4x4 transformGL;

		float4 colorRGBA;
	};

	//struct Button : public UIElement 
	//{

	//};


public:
	M_GUI(Application* app, bool start_enabled = true);
	virtual ~M_GUI();

	bool Start() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	void RenderUIElements();
	UIElement* AddUIElement(UIElement* parent, float2, float2, float2);

	unsigned int VAO = 0;
	unsigned int VBO = 0;

	std::vector<UIElement*> elements;

	ResourceShader* uiShader;

};