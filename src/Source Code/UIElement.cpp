#include "UIElement.h"

#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

#include"OpenGL.h"
#include"RE_Shader.h"
#include"MO_Window.h"
#include"Application.h"

UIElement::UIElement(UIElement* _parent, float2 pos, float2 rot, float2 scale) : parent(_parent), colorRGBA(float4::one)
{
	//this->callback = [this]() 
	//{
	//	LOG(LogType::L_NORMAL, "%f", colorRGBA.x);
	//	colorRGBA = float4::one / 2.f;

	//	//float3 test = transformGL.Row3(3);
	//	//test.y += 0.02;
	//	//transformGL.SetRow3(3, test);

	//	float3 test = localTransform.Col3(3);
	//	float3 size = localTransform.GetScale();

	//	test.x -= size.x * 2;

	//	localTransform.SetCol3(3, test);

	//	this->UpdateTransform();
	//};
	localTransform = float4x4::FromTRS(float3(pos.x, pos.y, 0), Quat::FromEulerXYZ(rot.x, rot.y, 0.0f), float3(scale.x, scale.y, 1));
	if (parent != nullptr)
	{
		LCG rng;
		colorRGBA = colorRGBA.RandomDir(rng, 1.0);
		colorRGBA = colorRGBA.Abs();
		colorRGBA.w = 1.0;

		this->globalTransform = parent->globalTransform/*.Transposed()*/ * localTransform;
		//this->transformGL.Transpose();
	}
	else
	{
		colorRGBA = float4::zero;
		this->globalTransform = localTransform/*.Transposed()*/;
	}
}

UIElement::~UIElement()
{
	LOG(LogType::L_NORMAL, "Delete element");
	for (size_t i = 0; i < children.size(); i++)
	{
		delete children[i];
		children[i] = nullptr;
	}
	children.clear();
}

void UIElement::OnClick()
{
	//this->callback();
}

void UIElement::RenderElement(unsigned int VAO, ResourceShader* shader)
{

	float4x4 transMat = this->globalTransform.Transposed();
	GLint modelLoc = glGetUniformLocation(shader->shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transMat.ptr());

	modelLoc = glGetUniformLocation(shader->shaderProgramID, "inputColor");
	glUniform4fv(modelLoc, 1, &this->colorRGBA.x);

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->RenderElement(VAO, shader);
	}
}

bool UIElement::IsInside(float2 point)
{
	float3 pos;
	Quat rot;
	float3 size;

	//Normalize point
	point.x /= EngineExternal->moduleWindow->s_width;
	point.y /= EngineExternal->moduleWindow->s_height;

	point.x = point.x - (1 - point.x);
	point.y = -(point.y - (1 - point.y));

	this->globalTransform./*Transposed().*/Decompose(pos, rot, size);

	if ((point.x <= pos.x + size.x && point.x >= pos.x - size.x) &&
		point.y <= pos.y + size.y && point.y >= pos.y - size.y)
	{
		return true;
	}

	return false;
}

void UIElement::UpdateTransform()
{
	if (parent != nullptr)
	{
		this->globalTransform = parent->globalTransform/*.Transposed()*/ * this->localTransform/*.Transposed()*/;
		//this->transformGL.Transpose();
	}

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->UpdateTransform();
	}
}
