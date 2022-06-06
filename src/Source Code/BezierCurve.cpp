#include"BezierCurve.h"

BezierCurve::BezierCurve(std::vector<float3>& initData)
{
	this->originalPointSet = initData;
}

BezierCurve::~BezierCurve()
{
	this->originalPointSet.clear();
}