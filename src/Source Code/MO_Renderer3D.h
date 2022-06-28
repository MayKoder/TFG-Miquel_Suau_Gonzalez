#pragma once
#include "Application.h"
#include "Module.h"

#include <queue>

#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Geometry/LineSegment.h"

#include"DE_Cubemap.h"
#include"GridManager.h"

#include<map>

class ResourceMesh;
class ResourceTexture;
class C_Camera;
class C_DirectionalLight;
class GameObject;

typedef unsigned int	GLuint;		/* 4-byte unsigned */
typedef unsigned char	GLubyte;	/* 1-byte unsigned */

#define MAX_LIGHTS 1

#define SQUARE_TEXTURE_W 256
#define SQUARE_TEXTURE_H 256

struct LineRender
{
	LineRender(float3& _a, float3& _b, float3& _color) : a(_a), b(_b), color(_color) {}
	float3 a, b, color;
};

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	virtual ~ModuleRenderer3D();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void OnResize(int width, int height) override;

	void OnGUI() override;

	void DrawDebugLines();
	void AddDebugLines(float3& a, float3& b, float3& color);


	static void DrawBox(float3* points, float3 color = float3::one);
	
	void RayToMeshQueueIntersection(LineSegment& ray);

	C_Camera* GetGameRenderTarget()const;
	void SetGameRenderTarget(C_Camera* cam);

	void ClearAllRenderData();
private:

	void DirectionalShadowPass();

	void RenderWithOrdering(bool rTex = false);
	void DebugLine(LineSegment& line);
	void GetCAPS(std::string& caps);
	std::string str_CAPS;

public:
	bool vsync, wireframe, cull, lightng, color_material, texture_2d;
	bool displayDebugVertices, displayDebugBoxes, renderSkybox;

	GLuint checkersTexture;
	GLubyte checkerImage[SQUARE_TEXTURE_W][SQUARE_TEXTURE_H][4];

	std::vector<GameObject*> renderQueue;

	SDL_GLContext context;

	C_Camera* activeRenderCamera = nullptr; //TODO: This is temporal
	DE_Cubemap skybox;
	C_DirectionalLight* directLight;

	//GridManager gridInstance;

private:
	std::vector<LineRender> lines;
	LineSegment pickingDebug;
	std::vector<float3> debugPoints;

	//MMP_Plane plane;
};