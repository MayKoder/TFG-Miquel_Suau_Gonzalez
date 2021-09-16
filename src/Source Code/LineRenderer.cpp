#include "LineRenderer.h"

#include"RE_Shader.h"
#include"MO_Renderer3D.h"
#include"CO_Camera.h"
#include"MO_ResourceManager.h"

LineRenderer::LineRenderer() : shaderRes(nullptr), lineThickness(1.0)
{
}

LineRenderer::~LineRenderer()
{
	points.clear();
}

void LineRenderer::InitLineRenderer()
{
	renderData.InitBuffers();
	renderData.Bind();

	renderData.CreateAndSetVBO(points.data(), points.size());
	renderData.SetVertexAttrib(0, 3, 3, 0);

	renderData.UnBind();

	shaderRes = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(763632538, "Library/Shaders/763632538.shdr"));
}

void LineRenderer::AddPoint(float3 point)
{
	for (size_t i = 0; i < point.Size; i++)
	{
		points.push_back(point[i]);
	}

	renderData.Bind();
	renderData.SetVBO(0, points.data(), points.size(), GL_DYNAMIC_DRAW);
	renderData.UnBind();
}

void LineRenderer::UpdatePoint(int index, float3& value)
{
	assert(index < points.size() / 3, "Index out of range");

	index *= value.Size;
	for (size_t i = 0; i < value.Size; i++)
	{
		points[index + i] = value[i];
	}


	renderData.Bind();
	renderData.UpdateVBO(0, 0, points.data(), points.size());
	renderData.UnBind();
}

void LineRenderer::Render()
{
	glLineWidth(lineThickness);

	shaderRes->Bind();
	EngineExternal->moduleRenderer3D->activeRenderCamera->PushCameraShaderVars(shaderRes->shaderProgramID);

	glBindVertexArray(renderData.GetVAO());
	glDrawArrays(
		GL_LINE_STRIP, 0, points.size() / 3
	);
	glBindVertexArray(0);
	shaderRes->Unbind();

	glLineWidth(1.0);
}
