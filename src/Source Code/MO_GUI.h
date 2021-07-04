#pragma once
#include "Module.h"
#include "MathGeoLib/include/Math/float4x4.h"

class ResourceShader;

static float uiPlaneData[24] = {
	// pos      // tex
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f
};

class M_GUI : public Module
{
public:
	M_GUI(Application* app, bool start_enabled = true);
	virtual ~M_GUI();

	bool Start() override;
	bool CleanUp() override;

	void RenderUIElements();

	float4x4 uiWorldProjection;

	unsigned int VAO = 0;
	ResourceShader* uiShader;

};