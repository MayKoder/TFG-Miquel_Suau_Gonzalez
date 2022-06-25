#include "GO_Bridge.h"
#include"PMG.h"
#include"RE_Shader.h"

#include"Application.h"
#include"MO_ResourceManager.h"
#include"CO_Camera.h"
#include"MO_Renderer3D.h"
#include"CO_Transform.h"
#include"ImGui/imgui.h"
#include"BezierCurve.h"
#include"CO_DirectionalLight.h"

GO_Bridge::GO_Bridge(const char* name, GameObject* parent, int _uid) : GameObject(name, parent, _uid),
basePositionA(float3(abs(sinf(counter)) * 10.0, 0.5, 0.0)), basePositionB(float3(-5.0, 0.5, 0.0)),
ropeOffset(float3(-10.5, 0., 0.)), tableOffset(0.05), poleSeparation(1.), tableSize(float3(0.5, 0.1, 2.0)),
tableCurveOffset(float3::zero)
{
	//this->objPrimitives.push_back(PMG::CreateCylinder(40, 20));
	counter = 0.0f;
	CreatBridge();

	this->generalShader = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource("Assets/Shaders/shadowShader.glsl", Resource::Type::SHADER));

	//localAABB.SetNegativeInfinity();
	//orientedBox.SetNegativeInfinity();
}

GO_Bridge::~GO_Bridge()
{
	EngineExternal->moduleResources->UnloadResource(this->generalShader->GetAssetPath());
}

void GO_Bridge::Draw(C_DirectionalLight* light)
{
	float3 points[8];
	localAABB.GetCornerPoints(points);
	ModuleRenderer3D::DrawBox(points, float3(1.f, 0.5f, 0.9f));

	this->generalShader->Bind();
	EngineExternal->moduleRenderer3D->activeRenderCamera->PushCameraShaderVars(this->generalShader->shaderProgramID);

	if (light != nullptr) {
		light->PushLightUniforms(this->generalShader);
	}

	//this->generalShader->SetVector3("position", this->transform->position);
	this->generalShader->SetVector4("color", float4(1, 1, 1, 1));

	for (size_t i = 0; i < objPrimitives.size(); i++)
	{
		float3x3 normalMatrix = objPrimitives[i].modelMatrix.Transposed().InverseTransposed().Float3x3Part();
		this->generalShader->SetMatrix3("normalMatrix", normalMatrix);

		this->generalShader->SetVector3("color", objPrimitives[i].solidColor);

		this->generalShader->SetMatrix4("modelMatrix", objPrimitives[i].modelMatrix.Transposed());
		objPrimitives[i].meshObject.RenderAsIndices(GL_TRIANGLES, objPrimitives[i].GetIndicesSize(), GL_UNSIGNED_INT);
	}

	this->generalShader->Unbind();

	for (size_t i = 0; i < objPrimitives.size(); i++)
	{
		//objPrimitives[i].DebugDrawVertices();

		////TODO: Matrix math makes normals react to size, that should not happen...
		//objPrimitives[i].DebugDrawNormals();

	}
}

void GO_Bridge::DrawOptionsMenu()
{
	ImGui::Text("Bridge settings");
	ImGui::Spacing();

	if (ImGui::SliderFloat3("Base A", basePositionA.ptr(), 0.0, 5.0)) {
		CreatBridge();
	}	
	
	if (ImGui::SliderFloat3("Rope point", ropeOffset.ptr(), -20.0, 20.0)) {
		CreatBridge();
	}
	if (ImGui::SliderFloat("Pole separation", &poleSeparation, 0.1, 1.5)) {
		tableSize.z = poleSeparation * 2.0;
		CreatBridge();
	}

	ImGui::Separator();

	if (ImGui::SliderFloat3("Table curve", this->tableCurveOffset.ptr(), -5., 5.)) {
		CreatBridge();
	}
	if (ImGui::SliderFloat("Table offset", &tableOffset, 0., basePositionA.Length())) {
		CreatBridge();
	}
	if (ImGui::SliderFloat3("Table size", this->tableSize.ptr(), 0., 5.)) {
		CreatBridge();
	}
}

void GO_Bridge::PureGLDraw(ResourceShader& selectedShader)
{
	for (size_t i = 0; i < objPrimitives.size(); i++)
	{
		//float3x3 normalMatrix = objPrimitives[i].modelMatrix.Transposed().InverseTransposed().Float3x3Part();
		//this->generalShader->SetMatrix3("normalMatrix", normalMatrix);

		selectedShader.SetMatrix4("modelMatrix", objPrimitives[i].modelMatrix.Transposed());
		objPrimitives[i].meshObject.RenderAsIndices(GL_TRIANGLES, objPrimitives[i].GetIndicesSize(), GL_UNSIGNED_INT);
	}
}

void GO_Bridge::CreatBridge()
{
	this->objPrimitives.clear();
	float cubeSize = 1.0;

	//this->objPrimitives.push_back(PMG::CreateCylinder(float4x4::FromTRS(float3(0., 0., 0.), Quat::FromEulerXYZ(0., 0., 0.), float3::one) , 8, 10));
	this->objPrimitives.push_back(PMG::CreateQuad(float4x4::FromTRS(float3::zero, Quat::identity, float3(50, 50, 50))));
	//this->objPrimitives.push_back(PMG::CreateCylinder(float4x4::FromTRS(float3::zero, Quat::identity, float3(1, 1, 1)), 10, 10));

	//Base cubes
	this->objPrimitives.push_back(PMG::CreateCube(float4x4::FromTRS(basePositionA, Quat::identity, float3(cubeSize, 1.0, 3.0))));
	this->objPrimitives.push_back(PMG::CreateCube(float4x4::FromTRS(basePositionB, Quat::identity, float3(cubeSize, 1.0, 3.0))));


	//Poles
	float3 poleOffset = float3(0.3, 1.0, poleSeparation);
	this->objPrimitives.push_back(PMG::CreateCube(float4x4::FromTRS(basePositionB + poleOffset.Mul(float3(1.0, 1.0, -1.0)), Quat::identity, float3(0.15, 1.0, 0.15))));
	this->objPrimitives.push_back(PMG::CreateCube(float4x4::FromTRS(basePositionB + poleOffset, Quat::identity, float3(0.15, 1.0, 0.15))));

	this->objPrimitives.push_back(PMG::CreateCube(float4x4::FromTRS(basePositionA + poleOffset.Mul(float3(-1.0, 1.0, -1.0)), Quat::identity, float3(0.15, 1.0, 0.15))));
	this->objPrimitives.push_back(PMG::CreateCube(float4x4::FromTRS(basePositionA + poleOffset.Mul(float3(-1.0, 1.0, 1.0)), Quat::identity, float3(0.15, 1.0, 0.15))));

	//Ropes
	float3 poleAPosition = (basePositionA + poleOffset.Mul(float3(-1.0, 1.0, -1.0)));
	float3 poleBPosition = (basePositionB + poleOffset.Mul(float3(1.0, 1.0, -1.0)));
	this->objPrimitives.push_back(PMG::CreateCylinder(CalculateRopeTransform(poleAPosition, poleBPosition), 30, 20, ropeOffset));

	poleAPosition = (basePositionA + poleOffset.Mul(float3(-1.0, 1.0, 1.0)));
	poleBPosition = (basePositionB + poleOffset.Mul(float3(1.0, 1.0, 1.0)));
	this->objPrimitives.push_back(PMG::CreateCylinder(CalculateRopeTransform(poleAPosition, poleBPosition), 30, 20, ropeOffset));


	//Tables
	float tableSizeX = this->tableSize.x;
	float3 startPoint = basePositionA + float3(-0.5 - (tableSizeX / 2.0), 0.5 - tableSize.y, 0.0);
	float3 endPoint = basePositionB + float3(0.5 + (tableSizeX / 2.0), 0.5 - tableSize.y, 0.0);

	double sizeAndOffset = static_cast<double>(tableSizeX) + static_cast<double>(tableOffset);
	double distance = abs(startPoint.Distance(endPoint));
	double segmentSections = (distance / sizeAndOffset);

	double test = (segmentSections - floor(segmentSections)) / floor(segmentSections);
	test /= 2.0;

	float3 middlePoint = float3::Lerp(endPoint, startPoint, 0.5);
	BezierCurve tablesCurve = BezierCurve(std::vector<float3>{endPoint, middlePoint + tableCurveOffset, startPoint});
	for (double i = 0.0; i <= 1.0; i += 1.0 / segmentSections)
	{
		float3 cPoint = tablesCurve.GetValue(i + test);
		
		float3 direction = (tablesCurve.GetValue(i + test + (1.0 / segmentSections)) - tablesCurve.GetValue(i + test)).Normalized();

		Quat lookAtTest = Quat::LookAt(float3(1, 0, 0), direction, float3(0, 1, 0), float3(0, 1, 0));
		this->objPrimitives.push_back(PMG::CreateCube(float4x4::FromTRS(cPoint, lookAtTest, this->tableSize)));
	}

	//counter += EngineExternal->GetDT();
	std::vector<float3> allVertices;
	int numVerticesPack = 0;
	for (size_t i = 1; i < objPrimitives.size(); i++)
	{
		objPrimitives[i].ExtractVertexPositions(allVertices);
		numVerticesPack += objPrimitives[i].GetNumOfVertices();
	}
	localAABB.SetNegativeInfinity();
	localAABB.Enclose(allVertices.data(), numVerticesPack);
	//localAABB.Transform();


	//Load OpenGL data
	for (size_t i = 0; i < objPrimitives.size(); i++)
	{

		objPrimitives[i].meshObject.InitBuffers();

		objPrimitives[i].meshObject.Bind();

		objPrimitives[i].meshObject.CreateAndSetVBO(objPrimitives[i].dataPacks.data(), objPrimitives[i].dataPacks.size(), GL_STATIC_DRAW);

		//indices
		objPrimitives[i].meshObject.LoadEBO(objPrimitives[i].indices.data(), objPrimitives[i].indices.size());

		//position attribute
		objPrimitives[i].meshObject.SetVertexAttrib(0, 3, 6 * sizeof(float), 0, GL_FLOAT);

		//normals attribute
		objPrimitives[i].meshObject.SetVertexAttrib(1, 3, 6 * sizeof(float), 3 * sizeof(float), GL_FLOAT);

		objPrimitives[i].meshObject.UnBind();
	}
}

float4x4 GO_Bridge::CalculateRopeTransform(float3& pointA, float3& pointB)
{
	float3 directionA = (pointA - pointB);
	float distHalf = pointA.Distance(pointB);

	Quat rotation = Quat::LookAt(float3(1.0, 0.0, 0.0), directionA.Normalized(), float3(0.0, 1.0, 0.0), float3(0.0, 1.0, 0.0)) * Quat::FromEulerXYZ(0.0, 0.0, PI / 2.0);

	return float4x4::FromTRS((pointB + float3(.0, .4, .0)) + (directionA.Normalized() * (distHalf * 0.5)), rotation, float3(0.05, distHalf * 0.5, 0.05));
}
