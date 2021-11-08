#include "CO_MeshRenderer.h"
#include "RE_Mesh.h"

#include "Application.h"
#include "MO_Renderer3D.h"
#include"MO_ResourceManager.h"
#include"RE_Material.h"
#include"RE_Shader.h"
#include "IM_FileSystem.h"

#include "GameObject.h"
#include "CO_Material.h"
#include "CO_Transform.h"
#include"CO_Camera.h"

#include "ImGui/imgui.h"
#include"MO_Scene.h"

#include"DEJsonSupport.h"

#include"MathGeoLib/include/Geometry/Frustum.h"
#include"MathGeoLib/include/Geometry/Plane.h"

C_MeshRenderer::C_MeshRenderer(GameObject* _gm) : Component(_gm), _mesh(nullptr),
faceNormals(false), vertexNormals(false), showAABB(false), showOBB(false)
{
	name = "Mesh Renderer";
	alternColor = float3::one;



	//int subDivisions = 10;
	//float maxH = 1;
	//float hIncrement = maxH / (subDivisions - 1);

	//std::vector<vec3> vertices;

	//float3 position = this->GetGO()->transform->position;

	//int angle = 45;
	//for (size_t i = 0; i < 4; i++)
	//{
	//	//TODO: We could add a vertical sum to avoid lots of rotations, but it would fuck up the vertex order
	//	float k = 0.0f;
	//	vec3 dir = vec3(0.4, k, 0);
	//	dir = rotate(dir, angle, vec3(0, 1, 0));

	//	for (size_t j = 0; j < subDivisions; ++j)
	//	{

	//		vec3 ret = dir;
	//		ret.y = k;
	//		vertices.push_back(ret + vec3(position.x, position.y, position.z));
	//		//glVertex3fv(&ret.x);

	//		k += hIncrement;
	//	}
	//	angle += 90;
	//}




	//for (size_t h = 0; h < subDivisions - 1; h++)
	//{
	//	for (size_t s = 0; s < 4; s++)
	//	{
	//		int safe = s;
	//		if (safe >= 3) {
	//			safe = -1;
	//		}
	//		glVertex3fv(&vertices[h + 0 + (subDivisions * s)].x);
	//		glVertex3fv(&vertices[subDivisions + h + (subDivisions * safe)].x);
	//		glVertex3fv(&vertices[subDivisions + 1 + h + (subDivisions * safe)].x);

	//		glVertex3fv(&vertices[0 + h + (subDivisions * s)].x);
	//		glVertex3fv(&vertices[subDivisions + 1 + h + (subDivisions * safe)].x);
	//		glVertex3fv(&vertices[1 + h + (subDivisions * s)].x);
	//	}
	//}

	//int top = subDivisions - 1;
	//glVertex3fv(&vertices[top].x);
	//glVertex3fv(&vertices[top * 2 + 1].x);
	//glVertex3fv(&vertices[top * 3 + 2].x);

	//glVertex3fv(&vertices[top].x);
	//glVertex3fv(&vertices[top * 3 + 2].x);
	//glVertex3fv(&vertices[top * 4 + 3].x);


	//vertices.clear();



}

C_MeshRenderer::~C_MeshRenderer()
{
	if (_mesh != nullptr) 
	{
		EngineExternal->moduleResources->UnloadResource(_mesh->GetUID());
		_mesh = nullptr;
	}
}

void C_MeshRenderer::Update()
{
	//if (_mesh == nullptr)
	//	return;

	EngineExternal->moduleRenderer3D->renderQueue.push_back(this);

#ifndef STANDALONE
	if (showAABB ==true) 
	{

		float3 points[8];
		globalAABB.GetCornerPoints(points);
		ModuleRenderer3D::DrawBox(points, float3(0.2f, 1.f, 0.101f));
	}

	if (showOBB == true) 
	{

		float3 points[8];
		globalOBB.GetCornerPoints(points);
		ModuleRenderer3D::DrawBox(points);
	}
#endif // !STANDALONE

}

void C_MeshRenderer::RenderMesh(bool rTex)
{


	//if (_mesh == nullptr)
	//	return;


	//C_Transform* transform = gameObject->transform;

	////TODO IMPORTANT: Optimize this, save this pointer or something
	//C_Material* material = dynamic_cast<C_Material*>(gameObject->GetComponent(Component::Type::Material));
	//GLuint id = 0;

	//if (material != nullptr && material->IsActive())
	//	id = material->GetTextureID();

	//_mesh->RenderMesh(id, alternColor, rTex, (material && material->material != nullptr) ? material->material : EngineExternal->moduleScene->defaultMaterial, transform);

	//if (vertexNormals || faceNormals)
	//	_mesh->RenderMeshDebug(&vertexNormals, &faceNormals, transform->GetGlobalTransposed());

}

void C_MeshRenderer::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	if (_mesh) //TODO: I don't think this is a good idea
	{
		DEJson::WriteString(nObj, "Path", _mesh->GetLibraryPath());
		DEJson::WriteInt(nObj, "UID", _mesh->GetUID());
	}

	DEJson::WriteVector3(nObj, "alternColor", &alternColor.x);
}
void C_MeshRenderer::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);


	SetRenderMesh(dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("UID"), nObj.ReadString("Path"))));

	alternColor = nObj.ReadVector3("alternColor");

	if (_mesh == nullptr)
		return;

	_mesh->generalWireframe = &EngineExternal->moduleRenderer3D->wireframe;

	gameObject->transform->UpdateBoxes();
}

#ifndef STANDALONE
bool C_MeshRenderer::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		if (_mesh != nullptr) 
		{
			//ImGui::Image((ImTextureID)_mesh->textureID, ImVec2(128, 128));
			ImGui::Text("Vertices: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->vertices_count);
			ImGui::Text("Indices: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->indices_count);

			ImGui::Spacing();
			ImGui::Text("Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", _mesh->GetLibraryPath());
		}

		ImGui::Button("Drop .mmh to change mesh", ImVec2(200, 50));
		//TODO: Maybe move this into a function?
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MESH"))
			{
				std::string* libraryDrop = (std::string*)payload->Data;

				if (_mesh != nullptr) {
					EngineExternal->moduleResources->UnloadResource(_mesh->GetUID());
					_mesh = nullptr;
				}

				//TODO: Almost done come on TEMPORAL: This is the only way until fbx's .meta files
				//Store an array of meshes to reference
				std::string stID = "";
				FileSystem::GetFileName(libraryDrop->c_str(), stID, false);

				//ATOI is C++11 only, maybe not a good idea to use it
				int UID = std::atoi(stID.c_str());
				SetRenderMesh(dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->RequestResource(UID, libraryDrop->c_str())));
				LOG( "Mesh %s changed", (*libraryDrop).c_str());
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Checkbox("Vertex Normals", &vertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &faceNormals);

		ImGui::Checkbox("Show AABB", &showAABB);
		ImGui::SameLine();
		ImGui::Checkbox("Show OBB", &showOBB);

		ImGui::ColorPicker3("No texture color: ", &alternColor.x);

		if (ImGui::Button("Set as procedural plane")) 
		{
			if (_mesh != nullptr) {
				EngineExternal->moduleResources->UnloadResource(_mesh->GetUID());
				_mesh = nullptr;
			}

			//LCG randomizer;
			//_mesh = new ResourceMesh(randomizer.Int());
			//MMP_Plane::GenerateDataTo(_mesh);
		}

		return true;
	}
	return false;
}
#endif // !STANDALONE

void C_MeshRenderer::SetRenderMesh(ResourceMesh* mesh)
{ 
	_mesh = mesh;
	//_mesh->LoadCustomFormat(_mesh->GetLibraryPath());

	if (mesh == nullptr)
		return;

	globalOBB = _mesh->localAABB;
	globalOBB.Transform(gameObject->transform->globalTransform);

	// Generate global AABB
	globalAABB.SetNegativeInfinity();
	globalAABB.Enclose(globalOBB);

	_mesh->generalWireframe = &EngineExternal->moduleRenderer3D->wireframe;
}

ResourceMesh* C_MeshRenderer::GetRenderMesh()
{
	return _mesh;
}
