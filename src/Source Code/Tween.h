#pragma once

#include<functional>

template<class T>
struct Tween
{

	~Tween()
	{
		delete ptrValue;
	}
	void Set(T min, T max, float _duration, bool playOnStart = false)
	{
		a = min;
		b = max;

		//(playOnStart == false) ? value = min : value = max;
		ptrValue = new T();

		*ptrValue = a;

		tweenDuration = _duration;
		t = 0.f;
	}
	//void Set(T min, T max, float _speed, T* _pointer, bool playOnStart = false)
	//{
	//	minValue = min;
	//	maxValue = max;

	//	(playOnStart == false) ? value = min : value = max;
	//	ptrValue = _pointer;

	//	speed = _speed;
	//	t = 0.f;
	//}

	T Get() {
		return *ptrValue;
	}
	//T* GetPTR() {
	//	return ptrValue;
	//}

	void Invert() {
		isActive = true;
		T temp = a;

		a = b;
		b = temp;

		t = 0.f;
		//value = minValue;
	}

	T GetAndStep(float dt)
	{
		if (isActive == true)
		{
			*ptrValue = Step(dt);
		}
		return *ptrValue;
	}

	//void StepByPointer(float dt)
	//{
	//	assert(ptrValue == nullptr, "PTR is nullptr, something is wrong")
	//		* ptrValue = Step(dt)
	//}

	T Step(float dt)
	{
		return stepOverride(this, dt);
	}

private:
	T a;
	T b;

	T* ptrValue = nullptr; //Maybe we could use only the pointer? not sure

public:

	float tweenDuration;
	float t;
	bool isActive = false;

	std::function<T(Tween<T>*, float)> stepOverride = [](Tween<T>* ent, float dt) 
	{
		ent->t += dt;

		if (ent->t >= ent->tweenDuration) {
			ent->t = 0.0f;
			ent->isActive = false;
			return ent->b;
		}

		return ent->a + (ent->b - ent->a) * (ent->t / ent->tweenDuration);
		//return (ent->value * (1.0 - ent->t)) + (ent->maxValue * ent->t);
		//return (ent->minValue * (1.0f - ent->t)) + (ent->maxValue * ent->t);

		//return ent->minValue + ent->t * (ent->maxValue - ent->minValue);
	};

	//std::function<T(Tween<T>*, float)> stepOverride = [](Tween<T>* ent, float dt) 
	//{
	//	ent->t += dt * ent->speed;

	//	//t = (t < 0.5 ? pow(2, 20 * t - 10) / 2
	//	//	: (2 - pow(2, -20 * t + 10)) / 2) * 40;

	//	if (ent->t >= 1.0) {
	//		ent->t = 0.0;
	//		ent->isActive = false;
	//	}

	//	//T tmp = -2 * value + 2;
	//	//value = t < 0.5 ? 8 * value * value * value * value : 1 - (tmp*tmp*tmp*tmp) / 2;

	//	return (ent->value * (1.0 - ent->t)) + (ent->maxValue * ent->t);
	//	//return (t * maxValue) + (1.0 - t) * value;
	//};
};

//#include"Globals.h"
//namespace CustomEasings
//{
//	static std::function<float(Tween<float>*, float)> easeInOutBounce = [](Tween<float>* ent, float dt)
//	{
//		ent->t += dt;
//		float t = ent->t;
//
//		if (t < 0.5) {
//			return 8 * pow(2, 8 * (t - 1)) * abs(sin(t * PI * 7));
//		}
//		else {
//			return 1 - 8 * pow(2, -8 * t) * abs(sin(t * PI * 7));
//		}
//	};
//}
