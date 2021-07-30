#pragma once
#include<functional>
#include<vector>

#include"MathGeoLib/include/Math/float4.h"
#include "MathGeoLib/include/Math/float2.h"
#include"MathGeoLib/include/Math/float4x4.h"

#include<tuple>

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

	void SetOffset(float right, float left, float top, float bottom);

public:
	UIElement* parent; //Just a pointer, non dynamic, does not need to be deleted
	std::vector<UIElement*> children;

	float4x4 localTransform;

private:
	float4 rectOffset;
	float4 colorRGBA;
	float4x4 globalTransform;
};


class UIButton : public UIElement
{
public:
	UIButton(UIElement* _parent, float2 pos, float2 rot, float2 scale) : UIElement(_parent, pos, rot, scale), tupleTest(nullptr)
	{

	}
	virtual ~UIButton() {
		delete tupleTest;
		tupleTest = nullptr;
		//LOG(LogType::L_NORMAL, "Delete button");
	}

	virtual void OnClick() override 
	{
		this->callback();
	}

	template<typename Func, typename... Args>
	void SetupCallback(Func func1, Args... args)
	{
		tupleTest = new std::tuple<Args...>(args...);

		callback = [&, func1, args...]()
		{
			(func1)(std::get<Args>(*(std::tuple<Args...>*)tupleTest)...);
		};
	}

private:

	void* tupleTest;
	std::function<void()> callback;
};