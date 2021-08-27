#pragma once

template<class T>
struct Tween
{

	T maxValue;
	T minValue;

	float speed;
	float t;

	bool isActive = false;

	T value;
	T* ptrValue = nullptr; //Maybe we could use only the pointer? not sure

	void Set(T min, T max, float _speed, bool playOnStart = false) 
	{
		minValue = min;
		maxValue = max;

		(playOnStart == false) ? value = min : value = max;

		speed = _speed;
		t = 0.f;
	}
	void Set(T min, T max, float _speed, T* _pointer, bool playOnStart = false)
	{
		minValue = min;
		maxValue = max;

		(playOnStart == false) ? value = min : value = max;
		ptrValue = _pointer;

		speed = _speed;
		t = 0.f;
	}

	T Get() {
		return value;
	}
	T* GetPTR() {
		return ptrValue;
	}

	void Invert() {
		isActive = true;
		T temp = minValue;

		minValue = maxValue;
		maxValue = temp;

		t = 0.f;
		//value = minValue;
	}

	T GetAndStep(float dt) 
	{
		if (isActive == true) 
		{
			value = Step(dt);
		}
		return value;
	}
	void StepByPointer(float dt) 
	{
		assert(ptrValue == nullptr, "PTR is nullptr, something is wrong")
		*ptrValue = Step(dt)
	}

	T Step(float dt) 
	{
		t += dt * speed;

		//t = (t < 0.5 ? pow(2, 20 * t - 10) / 2
		//	: (2 - pow(2, -20 * t + 10)) / 2) * 40;

		if (t >= 1.0) {
			t = 0.0;
			isActive = false;
		}

		//T tmp = -2 * value + 2;
		//value = t < 0.5 ? 8 * value * value * value * value : 1 - (tmp*tmp*tmp*tmp) / 2;

		return (value * (1.0 - t)) + (maxValue * t);
		//return (t * maxValue) + (1.0 - t) * value;
	}
};
