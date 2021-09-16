#pragma once

#include<vector>
#include"GL_Object.h"
#include"MathGeoLib/include/Math/float3.h"

class ResourceShader;

//TODO: float3* data would be cool, should be do a second copy, or polym hered?
class LineRenderer
{
public:
	LineRenderer();
	~LineRenderer();

	void InitLineRenderer();

	void AddPoint(float3 point);

	void UpdatePoint(int index, float3& value);

	void Render();

	inline void SetThickness(float value) {
		lineThickness = value;
	}

private:

	float lineThickness;
	std::vector<float> points;
	GL_Object renderData = GL_Object(GL_Object::RENDER_TYPE::RE_ARRAY);
	ResourceShader* shaderRes;

};