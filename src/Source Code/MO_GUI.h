#pragma once
#include "Module.h"
#include "MathGeoLib/include/Math/float4x4.h"
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
		float4x4 transformGL;

		UIElement(float2 pos, float2 rot, float2 scale);
		~UIElement();

		/*virtual*/ void RenderElement(unsigned int VAO, ResourceShader* shader);
	};

	//struct Panel : public UIElement 
	//{

	//};


public:
	M_GUI(Application* app, bool start_enabled = true);
	virtual ~M_GUI();

	bool Start() override;
	bool CleanUp() override;

	void RenderUIElements();

	unsigned int VAO = 0;
	unsigned int VBO = 0;

	std::vector<UIElement> elements;

	ResourceShader* uiShader;

};