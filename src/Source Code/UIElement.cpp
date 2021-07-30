#include "UIElement.h"

#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

#include"OpenGL.h"
#include"RE_Shader.h"
#include"MO_Window.h"
#include"Application.h"
#include"glmath.h"

UIElement::UIElement(UIElement* _parent, float2 pos, float2 rot, float2 scale) : parent(_parent), colorRGBA(float4::one),
rectOffset(float4::zero)
{
	//vec2 onePix = vec2(2.0) / vec2((float)EngineExternal->moduleWindow->s_width, (float)EngineExternal->moduleWindow->s_height);
	//float aspect = (float)EngineExternal->moduleWindow->s_width / (float)EngineExternal->moduleWindow->s_height;
	//float onePixLength = onePix.y; // note: onePix.x * u_aspect equals to onePix.y


	localTransform = float4x4::FromTRS(float3(pos.x, pos.y, 0.f), Quat::FromEulerXYZ(rot.x, rot.y, 0.0f), float3(scale.x/* * onePixLength*/, scale.y/* * (onePixLength * aspect)*/, 0.f));
	this->globalTransform = (parent == nullptr ? float4x4::identity : parent->globalTransform) * this->localTransform;
	
	if (parent != nullptr)
	{
		LCG rng;
		colorRGBA = colorRGBA.RandomDir(rng, 1.0);
		colorRGBA = colorRGBA.Abs();
		colorRGBA.w = 1.0;
	}
	else
	{
		//colorRGBA = float4::zero;
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
}

void UIElement::RenderElement(unsigned int VAO, ResourceShader* shader)
{

	float4x4 transMat = this->globalTransform.Transposed();
	GLint modelLoc = glGetUniformLocation(shader->shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transMat.ptr());

	//mat4x4 mat = ortho(0.0f, EngineExternal->moduleWindow->s_width, EngineExternal->moduleWindow->s_height, 0.0f, -1.0f, 1.0f);
	//modelLoc = glGetUniformLocation(shader->shaderProgramID, "projection");
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, mat.M);

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
	
	this->globalTransform = (parent == nullptr ? float4x4::identity : parent->globalTransform)/*.Transposed()*/ * this->localTransform/*.Transposed()*/;
		//this->transformGL.Transpose();

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->UpdateTransform();
	}
}

void UIElement::SetOffset(float right, float left, float top, float bottom)
{

	vec2 onePix = vec2(2.0) / vec2((float)EngineExternal->moduleWindow->s_width, (float)EngineExternal->moduleWindow->s_height);
	float aspect = (float)EngineExternal->moduleWindow->s_width / (float)EngineExternal->moduleWindow->s_height;
	float onePixLength = onePix.y; // note: onePix.x * u_aspect equals to onePix.y

	right *= onePixLength;
	left *= onePixLength;

	top *= onePix.y;
	bottom *= onePix.y;

	rectOffset.Set(right, left, top, bottom);

	float3 scale = this->localTransform.GetScale();

	scale.x -= (right + left);
	scale.y -= (top + bottom);

	this->localTransform = float4x4::FromTRS(localTransform.TranslatePart(), localTransform.RotatePart(), scale);
	this->UpdateTransform();
}
