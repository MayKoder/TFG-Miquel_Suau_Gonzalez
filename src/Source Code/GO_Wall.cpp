#include "GO_Wall.h"
#include"GridManager.h"
#include"CO_Transform.h"
#include"CO_MeshRenderer.h"
#include"glmath.h"
#include"Application.h"
#include"RE_Mesh.h"

GO_Wall::GO_Wall(const char* name, GameObject* parent, int _uid) : GameObject(name, parent, _uid),
subDivisions(0), currentIndices(0)
{










}

int GO_Wall::InitWall(float3 wallPosition)
{

	transform->SetTransformMatrix(wallPosition, Quat::identity, float3::one);
	C_MeshRenderer* render = dynamic_cast<C_MeshRenderer*>(AddComponent(Component::Type::MeshRenderer));
	this->subDivisions = 3;

	int ret = GO_Wall::GenerateWall(this->subDivisions, float3::zero, render->vertices, render->indices, NULL);

	
	render->_mesh = new ResourceMesh(EngineExternal->GetRandomInt());

	render->_mesh->renderObject.InitBuffers();
	render->_mesh->renderObject.Bind();

	render->_mesh->renderObject.CreateAndSetVBO(render->vertices.data(), render->vertices.size(), GL_DYNAMIC_DRAW);
	render->_mesh->renderObject.LoadEBO(render->indices.data(), render->indices.size(), GL_DYNAMIC_DRAW);
	render->_mesh->indices_count = render->indices.size();

	render->_mesh->renderObject.SetVertexAttrib(0, 3, 3 * sizeof(float), 0 * sizeof(float), GL_FLOAT);

	render->_mesh->renderObject.UnBind();

	return ret;
}

void GO_Wall::UpdateWallGL()
{
	C_MeshRenderer* render = GetComponent<C_MeshRenderer>();
	render->_mesh->renderObject.Bind();

	render->_mesh->renderObject.SetVBO(0, render->vertices.data(), render->vertices.size(), GL_DYNAMIC_DRAW);
	render->_mesh->renderObject.LoadEBO(render->indices.data(), render->indices.size(), GL_DYNAMIC_DRAW);
	render->_mesh->indices_count = render->indices.size();

	render->_mesh->renderObject.UnBind();
}

int GO_Wall::GenerateWall(uint subDivisions, float3 positionOffset, std::vector<float>& vertices, std::vector<int>& indices, std::vector<int>* sidesToIgnore)
{
	float maxH = 1;
	float hIncrement = maxH / (subDivisions - 1);

	int indexOffset = vertices.size() / 3;
	int angle = -45;

	int beforeIndices = indices.size();

	for (size_t i = 0; i < 4; i++)
	{
		//TODO: We could add a vertical sum to avoid lots of rotations, but it would fuck up the vertex order
		float k = 0.0f;
		vec3 dir = vec3(0.4, k, 0);
		dir = rotate(dir, angle, vec3(0, 1, 0));

		for (size_t j = 0; j < subDivisions; ++j)
		{

			vec3 ret = dir;
			ret.y = k;
			vertices.push_back(ret.x + positionOffset.x);
			vertices.push_back(ret.y + positionOffset.y);
			vertices.push_back(ret.z + positionOffset.z);
			//glVertex3fv(&ret.x);

			k += hIncrement;
		}
		angle += 90;
	}

	for (size_t i = 0; i < 4; i++)
	{

		if (sidesToIgnore != nullptr && std::find(sidesToIgnore->begin(), sidesToIgnore->end(), i) != sidesToIgnore->end()) {
			continue;
		}

		for (size_t h = 0; h < subDivisions - 1; h++)
		{
			int a, b, c, d;

			a = ((i * subDivisions) + h) + indexOffset;
			b = (a + 1);
			c = (((i == 3) ? 0 + h + indexOffset : a + subDivisions));
			d = (c + 1);


			LOG("Quad indices %i, %i, %i, %i", a, b, c, d);

			indices.push_back(a);
			indices.push_back(c);
			indices.push_back(b);

			indices.push_back(c);
			indices.push_back(d);
			indices.push_back(b);

		}
	}

	int top = indexOffset + subDivisions - 1;
	int pilarOffset = (subDivisions - 1);
	indices.push_back(top);
	indices.push_back(top + pilarOffset + 1);
	indices.push_back(top + (pilarOffset*2) + 2);

	indices.push_back(top);
	indices.push_back(top + (pilarOffset * 2) + 2);
	indices.push_back(top + (pilarOffset * 3) + 3);

	return indices.size() - beforeIndices;
}
