#include "MO_Renderer3D.h"
#include "MaykMath.h"
#include "MMGui.h"

#include "MO_Window.h"
#include "MO_Camera3D.h"
#include "MO_Editor.h"
#include "MO_Scene.h"
#include "MO_Input.h"
#include "MO_GUI.h"

#include "RE_Mesh.h"
#include "RE_Texture.h"
#include "mmgr/mmgr.h"
#include "DE_FrameBuffer.h"

#include"GameObject.h"

#include"CO_MeshRenderer.h"
#include"CO_Camera.h"
#include"CO_Transform.h"
#include"CO_DirectionalLight.h"

#include"MathGeoLib/include/Geometry/Triangle.h"
#include"RE_Shader.h"

#include"IM_TextureImporter.h"			//Delete this

#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/MGDebug/MathGeoLib.lib")
#else
#pragma comment (lib, "MathGeoLib/libx86/MGRelease/MathGeoLib.lib")
#endif

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */


ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled), str_CAPS(""),
vsync(true), wireframe(false), directLight(nullptr), activeRenderCamera(nullptr)
{
	GetCAPS(str_CAPS);
	/*depth =*/ cull = lightng = color_material = texture_2d = true;

	//framebuffer = texColorBuffer = rbo = 0;
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{
}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG(  "Init: 3D Renderer context");
	bool ret = true;
	
	//ASK: Can i do this inside the MM namespace?
	MaykMath::Init();
	LOG(  "Init: MaykMath");

	//Create context
	context = SDL_GL_CreateContext(App->moduleWindow->window);
	SDL_GL_MakeCurrent(App->moduleWindow->window, context);
	if(context == NULL)
	{
		LOG( "OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum error = glewInit();
	if (error != GL_NO_ERROR)
	{
		LOG( "Error initializing glew library! %s", SDL_GetError());
		ret = false;
	}
	else
	{
		LOG(  "Init: Glew %s", glewGetString(GLEW_VERSION));
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(SDL_GL_SetSwapInterval(static_cast<int>(vsync)) < 0)
			LOG( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//gluErrorString
			LOG( "Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//ASK: Maybe glewGetErrorString is not the same as glutGerErrorString, so errors could be wrong
			LOG( "Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//ASK: Maybe glewGetErrorString is not the same as glutGerErrorString, so errors could be wrong
			LOG( "Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}

		// Blend for transparency
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendEquation(GL_FUNC_ADD);
		
		//GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
		//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		//glEnable(GL_LIGHT0);
		//glLightf(GL_LIGHT0, GL_AMBIENT, 0.75f);
		//glLightf(GL_LIGHT0, GL_DIFFUSE, 0.05f);

		//float pos[] = { 0, 1, 0, 1.0f };
		//glLightfv(GL_LIGHT0, GL_POSITION, pos);

		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		//glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	//Generate texture
	for (int i = 0; i < SQUARE_TEXTURE_W; i++) {
		for (int j = 0; j < SQUARE_TEXTURE_H; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkersTexture);
	glBindTexture(GL_TEXTURE_2D, checkersTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SQUARE_TEXTURE_W, SQUARE_TEXTURE_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);	
	glBindTexture(GL_TEXTURE_2D, 0);


	// Projection matrix for
	OnResize(App->moduleWindow->s_width, App->moduleWindow->s_height);

	std::vector<std::string> faces = {
		"EngineIcons/Skybox/right.dds",
		"EngineIcons/Skybox/left.dds",
		"EngineIcons/Skybox/top.dds",
		"EngineIcons/Skybox/bottom.dds",
		"EngineIcons/Skybox/front.dds",
		"EngineIcons/Skybox/back.dds"
	};

	TextureImporter::LoadCubeMap(faces, skybox);
	skybox.CreateGLData();
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	//plane.GenerateData();

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	gridInstance.UpdateInput(App->moduleGUI->selectedTool);
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{

	//DirectionalShadowPass();

	//-------- CAMERA CULLING PROCESS -----------//
	if (GetGameRenderTarget() != nullptr && GetGameRenderTarget()->cullingState == true)
	{
		std::vector<C_MeshRenderer*> copy = renderQueue;
		renderQueue.clear();
		for (size_t i = 0; i < copy.size(); i++)
		{
			if (GetGameRenderTarget()->IsInsideFrustum(copy[i]->globalAABB))
				renderQueue.push_back(copy[i]);
		}
		copy.clear();
	}
	
	//
	App->moduleCamera->editorCamera.StartDraw();
	//

//#pragma region IcoTest
//
//	glPointSize(10.0f);
//	glBegin(GL_TRIANGLES);
//	glColor3f(1.f, 0.f, 0.f);
//
//	// create 12 vertices of a icosahedron
//	float t = (1.0 + sqrtf(5.0)) / 2.0;
//
//	float3 vertices[] = 
//	{
//		float3(-1, t, 0),
//		float3(1, t, 0),
//		float3(-1, -t, 0),
//		float3(1, -t, 0),
//
//		float3(0, -1, t),
//		float3(0, 1, t),
//		float3(0, -1, -t),
//		float3(0, 1, -t),
//
//		float3(t, 0, -1),
//		float3(t, 0, 1),
//		float3(-t, 0, -1),
//		float3(-t, 0, 1),
//	};
//
//	// 5 faces around point 0
//	glVertex3fv(&vertices[0].x);
//	glVertex3fv(&vertices[11].x);
//	glVertex3fv(&vertices[5].x);
//
//	glVertex3fv(&vertices[0].x);
//	glVertex3fv(&vertices[5].x);
//	glVertex3fv(&vertices[1].x);
//
//	glVertex3fv(&vertices[0].x);
//	glVertex3fv(&vertices[1].x);
//	glVertex3fv(&vertices[7].x);
//
//	glVertex3fv(&vertices[0].x);
//	glVertex3fv(&vertices[7].x);
//	glVertex3fv(&vertices[10].x);
//
//	glVertex3fv(&vertices[0].x);
//	glVertex3fv(&vertices[10].x);
//	glVertex3fv(&vertices[11].x);
//
//	//// 5 adjacent faces
//	glVertex3fv(&vertices[1].x);
//	glVertex3fv(&vertices[5].x);
//	glVertex3fv(&vertices[9].x);
//
//	glVertex3fv(&vertices[5].x);
//	glVertex3fv(&vertices[11].x);
//	glVertex3fv(&vertices[4].x);
//
//	glVertex3fv(&vertices[11].x);
//	glVertex3fv(&vertices[10].x);
//	glVertex3fv(&vertices[2].x);
//
//	glVertex3fv(&vertices[10].x);
//	glVertex3fv(&vertices[7].x);
//	glVertex3fv(&vertices[6].x);
//
//	glVertex3fv(&vertices[7].x);
//	glVertex3fv(&vertices[1].x);
//	glVertex3fv(&vertices[8].x);
//
//	//// 5 faces around point 3
//	glVertex3fv(&vertices[3].x);
//	glVertex3fv(&vertices[9].x);
//	glVertex3fv(&vertices[4].x);
//
//	glVertex3fv(&vertices[3].x);
//	glVertex3fv(&vertices[4].x);
//	glVertex3fv(&vertices[2].x);
//
//	glVertex3fv(&vertices[3].x);
//	glVertex3fv(&vertices[2].x);
//	glVertex3fv(&vertices[6].x);
//
//	glVertex3fv(&vertices[3].x);
//	glVertex3fv(&vertices[6].x);
//	glVertex3fv(&vertices[8].x);
//
//	glVertex3fv(&vertices[3].x);
//	glVertex3fv(&vertices[8].x);
//	glVertex3fv(&vertices[9].x);
//
//	//// 5 adjacent faces
//	glVertex3fv(&vertices[4].x);
//	glVertex3fv(&vertices[9].x);
//	glVertex3fv(&vertices[5].x);
//
//	glVertex3fv(&vertices[2].x);
//	glVertex3fv(&vertices[4].x);
//	glVertex3fv(&vertices[11].x);
//
//	glVertex3fv(&vertices[6].x);
//	glVertex3fv(&vertices[2].x);
//	glVertex3fv(&vertices[10].x);
//
//	glVertex3fv(&vertices[8].x);
//	glVertex3fv(&vertices[6].x);
//	glVertex3fv(&vertices[7].x);
//
//	glVertex3fv(&vertices[9].x);
//	glVertex3fv(&vertices[8].x);
//	glVertex3fv(&vertices[1].x);
//
//
//	glEnd();
//	glPointSize(1.0f);
//	glColor3f(1.f, 1.f, 1.f);
//
//#pragma endregion

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	gridInstance.RenderGridTemporal();

	glPointSize(5);
	glBegin(GL_POINTS);

	int subDivisions = 10;
	float maxH = 1;
	float hIncrement = maxH / (subDivisions - 1);

	std::vector<vec3> vertices;

	int angle = 45;
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
			vertices.push_back(ret);
			glVertex3fv(&ret.x);

			k += hIncrement;
		}
		angle += 90;
	}
	


	glEnd();
	glPointSize(1);

	glBegin(GL_TRIANGLES);
	for (size_t h = 0; h < subDivisions - 1; h++)
	{
		for (size_t s = 0; s < 4; s++)
		{
			int safe = s;
			if (safe >= 3) {
				safe = -1;
			}
			glVertex3fv(&vertices[h+0 + (subDivisions * s)].x);
			glVertex3fv(&vertices[subDivisions + h + (subDivisions * safe)].x);
			glVertex3fv(&vertices[subDivisions+1 + h + (subDivisions * safe)].x);

			glVertex3fv(&vertices[0 + h + (subDivisions * s)].x);
			glVertex3fv(&vertices[subDivisions+1 + h + (subDivisions * safe)].x);
			glVertex3fv(&vertices[1 + h + (subDivisions * s)].x);
		}
	}

	int top = subDivisions - 1;
	glVertex3fv(&vertices[top].x);
	glVertex3fv(&vertices[top*2+1].x);
	glVertex3fv(&vertices[top*3+2].x);

	glVertex3fv(&vertices[top].x);
	glVertex3fv(&vertices[top * 3 + 2].x);
	glVertex3fv(&vertices[top*4+3].x);

	glEnd();
	vertices.clear();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//TODO: This should not be here
	if (!renderQueue.empty()) 
	{
		for (size_t i = 0; i < renderQueue.size(); i++)
		{
			float distance = App->moduleCamera->editorCamera.camFrustrum.pos.DistanceSq(renderQueue[i]->globalOBB.pos);
			renderQueueMap.emplace(distance, renderQueue[i]);
		}

		//TODO: Make wireframe only affect scene window
		(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		RenderWithOrdering();
		(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	skybox.DrawAsSkybox(&App->moduleCamera->editorCamera);
	
	//DebugLine(pickingDebug);
	//DrawDebugLines();

	//App->moduleCamera->editorCamera.msaaFBO.BindFrameBuffer();
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);

	//TODO: This is bad
	App->moduleInput->SetMouseLayer(MOUSE_LAYER::MOVE_CAMERA);
	glDisable(GL_DEPTH_TEST);  //Rendering by call order now
	App->moduleGUI->RenderUIElements();
	glEnable(GL_DEPTH_TEST);  


	//glClear(GL_DEPTH_BUFFER_BIT);
	//App->moduleCamera->editorCamera.msaaFBO.UnbindFrameBuffer();
	//App->moduleCamera->editorCamera.msaaFBO.ResolveToScreen();
	App->moduleCamera->editorCamera.EndDraw();
	
	//TEMPORAL: Delete here so you can call mouse picking from scene window, should not be here in the future
	ClearAllRenderData();

	SDL_GL_SwapWindow(App->moduleWindow->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG(  "Destroying 3D Renderer");
	skybox.ClearMemory();
	gridInstance.ClearMemory();

	SDL_GL_DeleteContext(context);
	ClearAllRenderData();

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);
	//ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);

	App->moduleWindow->s_width = width;
	App->moduleWindow->s_height = height;

	App->moduleCamera->editorCamera.ReGenerateBuffer(width, height);


	//if (gameCamera != nullptr) 
	//	gameCamera->ReGenerateBuffer(width, height);

}

#ifndef STANDALONE
void ModuleRenderer3D::OnGUI()
{
	if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
	{

		//TODO: Store all this info as const char* and dont call the functions every frame
		SDL_version ver;
		SDL_GetVersion(&ver);
		ImGui::Text("SDL Version: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%d.%d.%d", ver.major, ver.minor, ver.patch);
		ImGui::Text("OpenGL Version: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", glGetString(GL_VERSION));
		ImGui::TextWrapped("All external library versions can be found in the 'About' window with links to their pages.");

		ImGui::GreySeparator();
		ImGui::Text("CPUs: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%d (Cache: %dkb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
		ImGui::Text("System RAM: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.1fGb", SDL_GetSystemRAM() / 1000.f);

#pragma region Caps
		ImGui::Text("Caps:");
		ImGui::SameLine();

		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), str_CAPS.c_str());

#pragma endregion


		ImGui::GreySeparator();
		ImGui::Text("GPU: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), (const char*)glGetString(GL_VENDOR));
		ImGui::Text("Brand: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), (const char*)glGetString(GL_RENDERER));

		// Memory --------------------
		sMStats stats = m_getMemoryStatistics();

		ImGui::Text("Total Reported Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%u bytes", stats.totalReportedMemory);
		ImGui::Text("Total Actual Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%u bytes", stats.totalActualMemory);
		ImGui::Text("Peak Reported Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%u bytes", stats.peakReportedMemory);
		ImGui::Text("Peak Actual Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%u bytes", stats.peakActualMemory);

		ImGui::Checkbox("Enable V-Sync", &vsync);
			////Use Vsync
			//if (SDL_GL_SetSwapInterval(static_cast<int>(vsync)) < 0)
			//	LOG( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		ImGui::SameLine();
		ImGui::Checkbox("Wireframe Mode", &wireframe);

		//if (ImGui::Checkbox("Depth Test", &depth))
		//	(depth) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
		ImGui::SameLine();
		if (ImGui::Checkbox("Cull face", &cull))
			(cull) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

		if (ImGui::Checkbox("Lighting", &lightng))
			(lightng) ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
		ImGui::SameLine();
		if (ImGui::Checkbox("Color material", &color_material))
			(color_material) ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);

		if (ImGui::Checkbox("Texture 2D", &texture_2d))
			(texture_2d) ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);

		ImGui::Separator();

		//if (ImGui::DragFloat4("Ambient: ", &lights[0].ambient, 0.01f, 0.f, 1.f))
		//	lights[0].Init();
		//if(ImGui::DragFloat4("Diffuse: ", &lights[0].diffuse, 0.01f, 0.f, 1.f)) 
		//	lights[0].Init();

	}
}
void ModuleRenderer3D::DrawDebugLines()
{
	//glBegin(GL_LINES);
	//for (size_t i = 0; i < lines.size(); i++)
	//{
	//	glColor3fv(lines[i].color.ptr());
	//	glVertex3fv(lines[i].a.ptr());
	//	glVertex3fv(lines[i].b.ptr());

	//	glColor3f(255.f, 255.f, 255.f);
	//}
	//glEnd();

	//lines.clear();
}
void ModuleRenderer3D::AddDebugLines(float3& a, float3& b, float3& color)
{
	//lines.push_back(LineRender(a, b, color));
}
#endif // !STANDALONE

void ModuleRenderer3D::DrawBox(float3* points, float3 color)
{
	//glColor3fv(&color.x);
	//glLineWidth(2.f);
	EngineExternal->moduleRenderer3D->AddDebugLines(points[0], points[2], color);
	EngineExternal->moduleRenderer3D->AddDebugLines(points[2], points[6], color);
	EngineExternal->moduleRenderer3D->AddDebugLines(points[6], points[4], color);
	EngineExternal->moduleRenderer3D->AddDebugLines(points[4], points[0], color);

	EngineExternal->moduleRenderer3D->AddDebugLines(points[0], points[1], color);
	EngineExternal->moduleRenderer3D->AddDebugLines(points[1], points[3], color);
	EngineExternal->moduleRenderer3D->AddDebugLines(points[3], points[2], color);
	EngineExternal->moduleRenderer3D->AddDebugLines(points[4], points[5], color);

	EngineExternal->moduleRenderer3D->AddDebugLines(points[6], points[7], color);
	EngineExternal->moduleRenderer3D->AddDebugLines(points[5], points[7], color);
	EngineExternal->moduleRenderer3D->AddDebugLines(points[3], points[7], color);
	EngineExternal->moduleRenderer3D->AddDebugLines(points[1], points[5], color);

	//glLineWidth(1.f);
	//glColor3f(1.f, 1.f, 1.f);
}

void ModuleRenderer3D::RayToMeshQueueIntersection(LineSegment& ray)
{
	pickingDebug = ray;

	std::map<float, C_MeshRenderer*> canSelect;
	float nHit = 0;
	float fHit = 0;

	bool selected = false;
	for (std::vector<C_MeshRenderer*>::iterator i = renderQueue.begin(); i != renderQueue.end(); ++i)
	{
		if (ray.Intersects((*i)->globalAABB, nHit, fHit))
			canSelect[nHit] = (*i);
	}


	//Add all meshes with a triangle hit and store the distance from the ray to the triangle, then pick the closest one
	std::map<float, C_MeshRenderer*> distMap;
	for(auto i = canSelect.begin(); i != canSelect.end(); ++i)
	{
		const ResourceMesh* _mesh = (*i).second->GetRenderMesh();
		if (_mesh)
		{
			LineSegment local = ray;
			local.Transform((*i).second->GetGO()->transform->globalTransform.Inverted());

			if (_mesh->vertices_count >= 9) //TODO: Had to do this to avoid squared meshes crash
			{
				for (uint index = 0; index < _mesh->indices_count; index += 3)
				{
					float3 pA(&_mesh->vertices[_mesh->indices[index] * VERTEX_ATTRIBUTES]);
					float3 pB(&_mesh->vertices[_mesh->indices[index + 1] * VERTEX_ATTRIBUTES]);
					float3 pC(&_mesh->vertices[_mesh->indices[index + 2] * VERTEX_ATTRIBUTES]);

					Triangle triangle(pA, pB, pC);

					float dist = 0;
					if (local.Intersects(triangle, &dist, nullptr))
						distMap[dist] = (*i).second;

				}
			}
		}
	}
	canSelect.clear();

//#ifndef STANDALONE
//	if (distMap.begin() != distMap.end())
//	{
//		App->moduleEditor->SetSelectedGO((*distMap.begin()).second->GetGO());
//		selected = true;
//	}
//
//
//	//If nothing is selected, set selected GO to null
//	if(!selected)
//		App->moduleEditor->SetSelectedGO(nullptr);
//#endif // !STANDALONE
	distMap.clear();
}

void ModuleRenderer3D::DirectionalShadowPass()
{
	//Render light depth pass
	if (directLight)
	{
		directLight->StartPass();
		if (!renderQueue.empty())
		{
			for (size_t i = 0; i < renderQueue.size(); i++)
			{
				float distance = directLight->orthoFrustum.pos.DistanceSq(renderQueue[i]->globalOBB.pos);
				renderQueueMap.emplace(distance, renderQueue[i]);
			}


			if (!renderQueueMap.empty())
			{
				for (auto i = renderQueueMap.rbegin(); i != renderQueueMap.rend(); ++i)
				{
					// Get the range of the current key
					auto range = renderQueueMap.equal_range(i->first);

					// Now render out that whole range
					for (auto d = range.first; d != range.second; ++d)
					{
						GLint modelLoc = glGetUniformLocation(directLight->depthShader->shaderProgramID, "model");
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, d->second->GetGO()->transform->GetGlobalTransposed());

						modelLoc = glGetUniformLocation(directLight->depthShader->shaderProgramID, "lightSpaceMatrix");
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, directLight->spaceMatrixOpenGL.ptr());

						d->second->GetRenderMesh()->OGL_GPU_Render();
					}
				}

				renderQueueMap.clear();
			}
		}
		directLight->EndPass();
	}
}

void ModuleRenderer3D::RenderWithOrdering(bool rTex)
{
	if (renderQueueMap.empty())
		return;

	for (auto i = renderQueueMap.rbegin(); i != renderQueueMap.rend(); ++i)
	{
		// Get the range of the current key
		auto range = renderQueueMap.equal_range(i->first);

		// Now render out that whole range
		for (auto d = range.first; d != range.second; ++d)
			d->second->RenderMesh(rTex);
	}

	renderQueueMap.clear();
}

void ModuleRenderer3D::DebugLine(LineSegment& line)
{
	glColor3f(1.f, 0.f, 0.f);
	glLineWidth(2.f);
	glBegin(GL_LINES);
	glVertex3fv(&pickingDebug.a.x);
	glVertex3fv(&pickingDebug.b.x);
	glEnd();
	glLineWidth(1.f);
	glColor3f(1.f, 1.f, 1.f);
}

/*Get SDL caps*/
void ModuleRenderer3D::GetCAPS(std::string& caps)
{
	caps += (SDL_HasRDTSC()) ? "RDTSC," : "";
	caps += (SDL_HasMMX()) ? "MMX, " : "";
	caps += (SDL_HasSSE()) ? "SSE, " : "";
	caps += (SDL_HasSSE2()) ? "SSE2, " : "";
	caps += (SDL_HasSSE3()) ? "SSE3, " : "";
	caps += "\n";
	caps += (SDL_HasSSE41()) ? "SSE41, " : "";
	caps += (SDL_HasSSE42()) ? "SSE42, " : "";
	caps += (SDL_HasAVX()) ? "AVX, " : "";
	caps += (SDL_HasAltiVec()) ? "AltiVec, " : "";
	caps += (SDL_Has3DNow()) ? "3DNow, " : "";
}

C_Camera* ModuleRenderer3D::GetGameRenderTarget() const
{
	//return gameCamera;
	return nullptr;
}

void ModuleRenderer3D::SetGameRenderTarget(C_Camera* cam)
{
	//gameCamera = cam;

#ifndef STANDALONE
	//W_Game* gWindow = dynamic_cast<W_Game*>(App->moduleEditor->GetEditorWindow(EditorWindow::GAME));
	//if (gWindow != nullptr && gameCamera != nullptr)
	//	gWindow->SetTargetCamera(gameCamera);
#endif // !STANDALONE

	//if (gameCamera != nullptr)
	//	gameCamera->ReGenerateBuffer(App->moduleWindow->s_width, App->moduleWindow->s_height);
}

void ModuleRenderer3D::ClearAllRenderData()
{
	renderQueueMap.clear();
	renderQueue.clear();
	
	//lines.clear();
}
