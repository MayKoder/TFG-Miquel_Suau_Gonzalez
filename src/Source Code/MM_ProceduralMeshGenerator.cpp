//#include"MM_ProceduralMeshGenerator.h"
//#include"OpenGL.h"
//
//#include"DEResource.h"
//#include"RE_Mesh.h"
//#include"Globals.h"
//#include"MathGeoLib/include/Math/float2.h"
//
//
//ProceduralMesh::ProceduralMesh()
//{
//	_mesh = new ResourceMesh(randomizer.Int());
//}
//
//ProceduralMesh::~ProceduralMesh()
//{
//	_mesh->UnloadFromMemory();
//	delete _mesh;
//	_mesh = nullptr;
//}
//
//void ProceduralMesh::RenderOGL()
//{
//
//	//glPointSize(10.0);
//	//glColor3f(1, 0, 0);
//	////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	//glBegin(GL_TRIANGLES);
//
//
//	//for (size_t i = 0; i < indices.size(); i += 3)
//	//{
//	//	glVertex3fv(&vertices[indices[i]].x);
//	//	glVertex3fv(&vertices[indices[i + 1]].x);
//	//	glVertex3fv(&vertices[indices[i + 2]].x);
//	//}
//
//
//	//glEnd();
//	//glPointSize(1.0);
//	//glColor3f(1, 1, 1);
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//	_mesh->OGL_GPU_Render();
//}
//
//
//
//MMP_Plane::MMP_Plane() : ProceduralMesh()
//{
//}
//
//MMP_Plane::~MMP_Plane()
//{
//}
//
//void MMP_Plane::GenerateData()
//{
//	int xSize = 100, zSize = 100;
//	int EBO_ATTR = 8;
//
//	//for (size_t z = 0; z <= zSize; ++z)
//	//{
//	//	for (size_t x = 0; x <= xSize; ++x)
//	//	{
//	//		this->vertices.push_back(float3(x, randomizer.Float(), z));
//	//	}
//	//}
//
//	//int vert = 0;
//	//for (size_t z = 0; z < zSize; z++)
//	//{
//	//	for (size_t x = 0; x < xSize; x++)
//	//	{
//	//		indices.push_back(vert+0);
//	//		indices.push_back(vert+xSize + 1);
//	//		indices.push_back(vert+1);
//	//		indices.push_back(vert+1);
//	//		indices.push_back(vert+xSize+1);
//	//		indices.push_back(vert+xSize+2);
//
//	//		vert++;
//	//	}
//	//	vert++;
//	//}
//
//
//	_mesh->vertices_count = (xSize + 1) * (zSize + 1);
//	_mesh->vertices = new float[_mesh->vertices_count * EBO_ATTR];
//
//	int i = 0;
//	float uvIncrement = 1.0 / 101.0;
//
//	float2 currentUV = float2::zero;
//	for (size_t z = 0; z <= zSize; ++z)
//	{
//		for (size_t x = 0; x <= xSize; ++x)
//		{
//			//this->vertices.push_back(float3(x, randomizer.Float(), z));
//
//			//vERTICES
//			_mesh->vertices[i * EBO_ATTR] = x;
//			_mesh->vertices[(i * EBO_ATTR) + 1] = 0.0f/*randomizer.Float()*/;
//			_mesh->vertices[(i * EBO_ATTR) + 2] = z;
//
//			//TexCoords
//			_mesh->vertices[i * EBO_ATTR + 3] = currentUV.y;
//			_mesh->vertices[(i * EBO_ATTR) + 4] = currentUV.x;
//
//			//Normals
//			_mesh->vertices[i * EBO_ATTR + 5] = 0.0;
//			_mesh->vertices[(i * EBO_ATTR) + 6] = 0.0;
//			_mesh->vertices[(i * EBO_ATTR) + 7] = 0.0;
//
//			i++;
//			currentUV.x += uvIncrement;
//		}
//		currentUV.y += uvIncrement;
//		currentUV.x = 0;
//	}
//	//for (size_t i = 0; i < vertices.size(); i++)
//	//{
//	//	_mesh->vertices[i * 3] = vertices[i].x;
//	//	_mesh->vertices[(i * 3)+1] = vertices[i].y;
//	//	_mesh->vertices[(i * 3)+2] = vertices[i].z;
//	//}
//
//	//memcpy(_mesh->vertices, vertices.data(), sizeof(float) * _mesh->vertices_count * 3);
//
//	_mesh->indices_count = xSize* zSize * 6;
//	_mesh->indices = new uint[_mesh->indices_count];
//
//	int vert = 0;
//	int y = 0;
//	for (size_t z = 0; z < zSize; z++)
//	{
//		for (size_t x = 0; x < xSize; x++)
//		{
//			_mesh->indices[y] = (vert + 0);
//			_mesh->indices[y+1] = (vert + xSize + 1);
//			_mesh->indices[y+2] = (vert + 1);
//
//			float3 A(_mesh->vertices[_mesh->indices[y]], _mesh->vertices[_mesh->indices[y] + 1], _mesh->vertices[_mesh->indices[y] + 2]);
//			float3 B(_mesh->vertices[_mesh->indices[y + 1]], _mesh->vertices[_mesh->indices[y + 1] + 1], _mesh->vertices[_mesh->indices[y + 1] + 2]);
//			float3 C(_mesh->vertices[_mesh->indices[y + 2]], _mesh->vertices[_mesh->indices[y + 2] + 1], _mesh->vertices[_mesh->indices[y + 2] + 2]);
//
//			float3 n = (B-A).Cross(C-A).Normalized();
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y]) + 7] = n.z;
//
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y+1]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y+1]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y+1]) + 7] = n.z;
//
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y+2]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y+2]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y+2]) + 7] = n.z;
//			
//			//-------------------
//
//			_mesh->indices[y+3] = (vert + 1);
//			_mesh->indices[y+4] = (vert + xSize + 1);
//			_mesh->indices[y+5] = (vert + xSize + 2);
//
//			A = float3(_mesh->vertices[_mesh->indices[y + 3]], _mesh->vertices[_mesh->indices[y + 3] + 1], _mesh->vertices[_mesh->indices[y + 3] + 2]);
//			B = float3(_mesh->vertices[_mesh->indices[y + 4]], _mesh->vertices[_mesh->indices[y + 4] + 1], _mesh->vertices[_mesh->indices[y + 4] + 2]);
//			C = float3(_mesh->vertices[_mesh->indices[y + 5]], _mesh->vertices[_mesh->indices[y + 5] + 1], _mesh->vertices[_mesh->indices[y + 5] + 2]);
//
//			n = (B - A).Cross(C - A).Normalized();
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y+3]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y+3]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y+3]) + 7] = n.z;
//
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 4]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 4]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 4]) + 7] = n.z;
//
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 5]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 5]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 5]) + 7] = n.z;
//
//			vert++;
//			y += 6;
//		}
//		vert++;
//	}
//	//memcpy(_mesh->indices, indices.data(), indices.size() * sizeof(uint));
//
//	glGenVertexArrays(1, &_mesh->VAO);
//	glGenBuffers(1, (GLuint*)&(_mesh->VBO));
//	glGenBuffers(1, (GLuint*)&(_mesh->EBO));
//
//	glBindVertexArray(_mesh->VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, _mesh->VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * EBO_ATTR * _mesh->vertices_count, _mesh->vertices, GL_STATIC_DRAW);
//
//	//indices
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * _mesh->indices_count, _mesh->indices, GL_STATIC_DRAW);
//
//	//position attribute
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, EBO_ATTR * sizeof(float), (GLvoid*)0);
//	glEnableVertexAttribArray(0);
//
//	//texcoords attribute
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, EBO_ATTR * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
//	glEnableVertexAttribArray(1);
//
//	//normals attribute
//	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, EBO_ATTR * sizeof(float), (GLvoid*)(5 * sizeof(GLfloat)));
//	glEnableVertexAttribArray(2);
//
//	glBindVertexArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//
//	//_mesh->LoadToMemory();
//
//}
//
//void MMP_Plane::GenerateDataTo(ResourceMesh* _mesh)
//{
//	int xSize = 100, zSize = 100;
//	int EBO_ATTR = 11;
//	LCG randomizer;
//
//
//	_mesh->vertices_count = (xSize + 1) * (zSize + 1);
//	_mesh->vertices = new float[_mesh->vertices_count * EBO_ATTR];
//
//	int i = 0;
//	float uvIncrement = 1.0 / 101.0;
//
//	float2 currentUV = float2::zero;
//	for (size_t z = 0; z <= zSize; ++z)
//	{
//		for (size_t x = 0; x <= xSize; ++x)
//		{
//
//			//vERTICES
//			_mesh->vertices[i * EBO_ATTR] = x;
//			_mesh->vertices[(i * EBO_ATTR) + 1] = 0.0;
//			_mesh->vertices[(i * EBO_ATTR) + 2] = z;
//
//			//TexCoords
//			_mesh->vertices[i * EBO_ATTR + 3] = currentUV.y;
//			_mesh->vertices[(i * EBO_ATTR) + 4] = currentUV.x;
//
//			//Normals
//			_mesh->vertices[i * EBO_ATTR + 5] = 0.0;
//			_mesh->vertices[(i * EBO_ATTR) + 6] = 0.0;
//			_mesh->vertices[(i * EBO_ATTR) + 7] = 0.0;
//
//			//Tangents
//			_mesh->vertices[i * EBO_ATTR + 8] = 1.0;
//			_mesh->vertices[(i * EBO_ATTR) + 9] = 0.0;
//			_mesh->vertices[(i * EBO_ATTR) + 10] = 0.0;
//
//			i++;
//			currentUV.x += uvIncrement;
//		}
//		currentUV.y += uvIncrement;
//		currentUV.x = 0;
//	}
//
//	_mesh->indices_count = xSize * zSize * 6;
//	_mesh->indices = new uint[_mesh->indices_count];
//
//	int vert = 0;
//	int y = 0;
//	for (size_t z = 0; z < zSize; z++)
//	{
//		for (size_t x = 0; x < xSize; x++)
//		{
//			_mesh->indices[y] = (vert + 0);
//			_mesh->indices[y + 1] = (vert + xSize + 1);
//			_mesh->indices[y + 2] = (vert + 1);
//
//			float3 A(_mesh->vertices[(EBO_ATTR * _mesh->indices[y])], _mesh->vertices[(EBO_ATTR * _mesh->indices[y]) + 1], _mesh->vertices[(EBO_ATTR * _mesh->indices[y]) + 2]);
//			float3 B(_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 1])], _mesh->vertices[(EBO_ATTR * _mesh->indices[y + 1]) + 1], _mesh->vertices[(EBO_ATTR * _mesh->indices[y + 1]) + 2]);
//			float3 C(_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 2])], _mesh->vertices[(EBO_ATTR * _mesh->indices[y + 2]) + 1], _mesh->vertices[(EBO_ATTR * _mesh->indices[y + 2]) + 2]);
//
//			float3 n = (B - A).Cross(C - A).Normalized();
//			n.y = abs(n.y);
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y]) + 7] = n.z;
//
//			n = (A - B).Cross(C - B).Normalized();
//			n.y = abs(n.y);
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 1]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 1]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 1]) + 7] = n.z;
//
//			n = (A - C).Cross(B - C).Normalized();
//			n.y = abs(n.y);
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 2]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 2]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 2]) + 7] = n.z;
//
//			//-------------------
//
//			_mesh->indices[y + 3] = (vert + 1);
//			_mesh->indices[y + 4] = (vert + xSize + 1);
//			_mesh->indices[y + 5] = (vert + xSize + 2);
//
//			A = float3(_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 3])], _mesh->vertices[(EBO_ATTR * _mesh->indices[y + 3]) + 1], _mesh->vertices[(EBO_ATTR * _mesh->indices[y + 3]) + 2]);
//			B = float3(_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 4])], _mesh->vertices[(EBO_ATTR * _mesh->indices[y + 4]) + 1], _mesh->vertices[(EBO_ATTR * _mesh->indices[y + 4]) + 2]);
//			C = float3(_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 5])], _mesh->vertices[(EBO_ATTR * _mesh->indices[y + 5]) + 1], _mesh->vertices[(EBO_ATTR * _mesh->indices[y + 5]) + 2]);
//
//			n = (B - A).Cross(C - A).Normalized();
//			n.y = abs(n.y);
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 3]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 3]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 3]) + 7] = n.z;
//
//			n = (A - B).Cross(C - B).Normalized();
//			n.y = abs(n.y);
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 4]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 4]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 4]) + 7] = n.z;
//
//			n = (A - C).Cross(B - C).Normalized();
//			n.y = abs(n.y);
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 5]) + 5] = n.x;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 5]) + 6] = n.y;
//			_mesh->vertices[(EBO_ATTR * _mesh->indices[y + 5]) + 7] = n.z;
//
//			vert++;
//			y += 6;
//		}
//		vert++;
//	}
//
//	glGenVertexArrays(1, &_mesh->VAO);
//	glGenBuffers(1, (GLuint*)&(_mesh->VBO));
//	glGenBuffers(1, (GLuint*)&(_mesh->EBO));
//
//	glBindVertexArray(_mesh->VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, _mesh->VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * EBO_ATTR * _mesh->vertices_count, _mesh->vertices, GL_STATIC_DRAW);
//
//	//indices
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * _mesh->indices_count, _mesh->indices, GL_STATIC_DRAW);
//
//	//position attribute
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, EBO_ATTR * sizeof(float), (GLvoid*)0);
//	glEnableVertexAttribArray(0);
//
//	//texcoords attribute
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, EBO_ATTR * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
//	glEnableVertexAttribArray(1);
//
//	//normals attribute
//	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, EBO_ATTR * sizeof(float), (GLvoid*)(5 * sizeof(GLfloat)));
//	glEnableVertexAttribArray(2);
//
//	//tangent attribute
//	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, EBO_ATTR * sizeof(float), (GLvoid*)(8 * sizeof(GLfloat)));
//	glEnableVertexAttribArray(3);
//
//	glBindVertexArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//}
