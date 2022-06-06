#pragma once
#include"Globals.h"
#include"MathGeoLib/include/Math/float3.h"
#include<vector>
#include<assert.h>

class BezierCurve
{
public:
	BezierCurve(std::vector<float3>& initData);
	~BezierCurve();

	float3 GetValue(double t) {
		CLAMP(t, 0.0, 1.0);

		assert(originalPointSet.size() > 1, "Can't lerp a 1 point curve");

		std::vector<float3> pointSubSet = originalPointSet;


		while (pointSubSet.size() > 1) {

			std::vector<float3> tmpPoints;
			for (size_t i = 0; i < pointSubSet.size()-1; ++i)
			{
				float3 newPoint = float3::Lerp(pointSubSet[i], pointSubSet[i+1], t);
				tmpPoints.push_back(newPoint);
			}
			pointSubSet.clear();
			pointSubSet = tmpPoints;
		}

		return pointSubSet[0];
	}

private:

	std::vector<float3> originalPointSet;



};
