#pragma once
#include "Module.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/float4.h"
#include<vector>
#include<functional>

template<class>
class function;

template<class R, class... Args>
class function<R(Args...)>;

class ResourceShader;

static float uiPlaneData[] = 
{
-1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0
};

//template<typename... Args>
class M_GUI : public Module
{

public:
	struct UIElement 
	{

		//UIElement();
		UIElement(UIElement* _parent, float2 pos, float2 rot, float2 scale);
		~UIElement();

		//float2 GetPosition() {
		//	return globalTransform.Col3(3).xy(); 
		//}

		virtual void OnClick();
		/*virtual*/ void RenderElement(unsigned int VAO, ResourceShader* shader);
		bool IsInside(float2 point);
		void UpdateTransform();
		
		UIElement* parent; //Just a pointer, non dynamic, does not need to be deleted
		std::vector<UIElement*> children;

		float4x4 localTransform;
		float4x4 globalTransform;

		float4 colorRGBA;
	};

	struct Button : public UIElement 
	{
		std::function<void()> callback;
	};


public:
	M_GUI(Application* app, bool start_enabled = true);
	virtual ~M_GUI();

	bool Start() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	void RenderUIElements();
	bool RecursiveUpdateElements(UIElement* element);
	UIElement* AddUIElement(UIElement* parent, float2, float2, float2);

	unsigned int VAO = 0;
	unsigned int VBO = 0;

	UIElement* root;

	ResourceShader* uiShader;
};