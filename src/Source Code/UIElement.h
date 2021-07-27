#pragma once
#include<functional>
#include<vector>

#include"MathGeoLib/include/Math/float4.h"
#include "MathGeoLib/include/Math/float2.h"
#include"MathGeoLib/include/Math/float4x4.h"

template<class>
class function;

template<class R, class... Args>
class function<R(Args...)>;

class ResourceShader;

class UIElement
{

public:
	//UIElement();
	UIElement(UIElement* _parent, float2 pos, float2 rot, float2 scale);
	virtual ~UIElement();

	//float2 GetPosition() {
	//	return globalTransform.Col3(3).xy(); 
	//}

	virtual void OnClick();
	/*virtual*/ void RenderElement(unsigned int VAO, ResourceShader* shader);
	bool IsInside(float2 point);
	void UpdateTransform();

public:
	UIElement* parent; //Just a pointer, non dynamic, does not need to be deleted
	std::vector<UIElement*> children;

	float4x4 localTransform;
	float4x4 globalTransform;

	float4 colorRGBA;
};

template <typename... Args>
class UIButton : public UIElement
{
public:
	UIButton(UIElement* _parent, float2 pos, float2 rot, float2 scale, std::function<void(Args...)> _callback) : UIElement(_parent, pos, rot, scale)
	{
		callback = _callback;
		temporalIndicator = true;
	}
	virtual ~UIButton() {
		//LOG(LogType::L_NORMAL, "Delete button");
	}

	virtual void OnClick() override 
	{
		
		this->callback(this, temporalIndicator);
	}

private:

	bool temporalIndicator;

	std::tuple<Args...> data;
	std::function<void(Args...)> callback;

};