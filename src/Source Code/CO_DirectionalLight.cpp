#include "CO_DirectionalLight.h"
#include "Application.h"
#include"MO_Renderer3D.h"

#include"GameObject.h"
#include"CO_Transform.h"
#include"CO_Camera.h"
#include"ImGui/imgui.h"
#include"MMGui.h"
#include"MO_ResourceManager.h"
#include"RE_Shader.h"

#include"RE_Shader.h"
#include"MO_Window.h"

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
C_DirectionalLight::C_DirectionalLight(GameObject* _gm) : Component(_gm), orthoSize(60.0f, 60.0f), lightColor(float3::one),
biasRange(float2(0.000f, 0.000f))
{
	name = "Directional Light";

	orthoFrustum.type = FrustumType::OrthographicFrustum;
	orthoFrustum.nearPlaneDistance = 0.1f;
	orthoFrustum.farPlaneDistance = 500.f;
	orthoFrustum.front = gameObject->transform->GetForward();
	orthoFrustum.up = gameObject->transform->GetUp();
	orthoFrustum.pos = gameObject->transform->position;
	orthoFrustum.orthographicWidth = SHADOW_WIDTH / orthoSize.x;
	orthoFrustum.orthographicHeight = SHADOW_HEIGHT / orthoSize.y;

	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	depthShader = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource("Assets/Shaders/simpleShadowMap.glsl", Resource::Type::SHADER));
}

C_DirectionalLight::~C_DirectionalLight()
{
	if (depthMapFBO != 0)
		glDeleteFramebuffers(1, (GLuint*)&depthMapFBO);

	if (depthMap != 0)
		glDeleteTextures(1, (GLuint*)&depthMap);

	EngineExternal->moduleResources->UnloadResource(depthShader->GetAssetPath());
	EngineExternal->moduleRenderer3D->directLight = nullptr;
}

void C_DirectionalLight::Update()
{
	//Maybe dont update every frame?
	//if (gameObject->transform->updateTransform) 
	//{
		orthoFrustum.pos = gameObject->transform->globalTransform.TranslatePart();
		orthoFrustum.front = gameObject->transform->GetForward();
		orthoFrustum.up = gameObject->transform->GetUp();

		C_Camera::LookAt(orthoFrustum, float3::zero);
		spaceMatrixOpenGL = (orthoFrustum.ProjectionMatrix() * orthoFrustum.ViewMatrix()).Transposed();
	//}

	if (EngineExternal->moduleRenderer3D->displayDebug) {
		float3 points[8];
		orthoFrustum.GetCornerPoints(points);

		ModuleRenderer3D::DrawBox(points, float3(0.0f, 1.f, 0.0f));
	}


}

#ifndef STANDALONE
bool C_DirectionalLight::OnEditor()
{
	ImGui::AddMenuHeaderCustom("Light Settings", 10);

	ImGui::Image((ImTextureID)depthMap, ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::DragFloat2("Ortho size", orthoSize.ptr(), 1.0f)) 
	{
		orthoFrustum.orthographicWidth = SHADOW_WIDTH / orthoSize.x;
		orthoFrustum.orthographicHeight = SHADOW_HEIGHT / orthoSize.y;
	}

	//ImGui::DragFloat2("Bias range", biasRange.ptr(), 0.00001f, 0.0f, 1.0f, "%.5f");

	ImGui::ColorPicker3("Color", lightColor.ptr());

	return true;
}
#endif // !STANDALONE

void C_DirectionalLight::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);
	
	DEConfig data(nObj);
	data.WriteVector2("orthoSize", orthoSize.ptr());
	data.WriteVector3("lightColor", lightColor.ptr());
}
void C_DirectionalLight::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	orthoSize = nObj.ReadVector2("orthoSize");
	lightColor = nObj.ReadVector3("lightColor");

}

void C_DirectionalLight::StartPass()
{

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glDisable(GL_CULL_FACE);

	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// 1. first render to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat*)orthoFrustum.ProjectionMatrix().Transposed().v);

	math::float4x4 mat = orthoFrustum.ViewMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat*)mat.Transposed().v);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//glClearColor(0.08f, 0.08f, 0.08f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT);

	depthShader->Bind();

}

void C_DirectionalLight::PushLightUniforms(ResourceShader* shader)
{
	shader->SetMatrix4("lightSpaceMatrix", this->spaceMatrixOpenGL);
	shader->SetVector3("lightPos", gameObject->transform->position);
	shader->SetVector3("viewPos", EngineExternal->moduleRenderer3D->activeRenderCamera->GetPosition());
	shader->SetVector3("lightColor", lightColor);
	shader->SetVector3("altColor", float3(1.0, 1.0, 1.0));
	shader->SetVector2("biasRange", biasRange);

	//glUniform1i(glGetUniformLocation(material->shader->shaderProgramID, shadowMap), used_textures);

	glActiveTexture(GL_TEXTURE5);
	GLint modelLoc = glGetUniformLocation(shader->shaderProgramID, "shadowMap");
	glUniform1i(modelLoc, 5);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}

void C_DirectionalLight::EndPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	depthShader->Unbind();

	//glEnable(GL_CULL_FACE);
	glViewport(0, 0, EngineExternal->moduleWindow->s_width, EngineExternal->moduleWindow->s_height);
}

//#version 330 core
//layout(location = 0) in vec3 aPos;
//
//uniform mat4 lightSpaceMatrix;
//uniform mat4 model;
//
//void main()
//{
//	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
//}

//#version 330 core
//
//void main()
//{
//	// gl_FragDepth = gl_FragCoord.z;
//}
