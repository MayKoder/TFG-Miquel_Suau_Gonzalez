#include "Globals.h"
#include "Application.h"
#include "MO_Camera3D.h"
#include "ImGui/imgui.h"
#include "MO_Input.h"
#include "MO_Window.h"
#include "MO_Editor.h"
#include "GameObject.h"
#include "MO_Renderer3D.h"

#include"CO_Transform.h"
#include"MathGeoLib/include/Math/float4.h"
#include"GO_Bridge.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled), mouseSensitivity(0.50f), cameraSpeed(40.f), cameraMovement(0.f, 0.f, 0.f)
{
	editorCamera.camFrustrum.farPlaneDistance = 5000;
	editorCamera.camFrustrum.pos = float3(0.0f, 9.0f, 7.0f);
	App->moduleRenderer3D->activeRenderCamera = &editorCamera;
	Reference = float3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG(  "Setting up the camera");
	bool ret = true;

	editorCamera.LookAt(dynamic_cast<GO_Bridge*>(App->moduleRenderer3D->renderQueue[0])->localAABB.CenterPoint());

	//LookAt(float3(0.f, 1.f, 0.f));

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG(  "Cleaning camera");

	return true;
}

#ifndef STANDALONE
void ModuleCamera3D::OnGUI()
{
	if (ImGui::CollapsingHeader("3D Input Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TextWrapped("Camera Rotation Speed"); ImGui::SameLine(); ImGui::PushItemWidth(100.f); ImGui::DragFloat("##crs", &mouseSensitivity, 0.01f, 0.f, 999.f);
		ImGui::TextWrapped("Camera Movement Speed"); ImGui::SameLine(); ImGui::PushItemWidth(100.f); ImGui::DragFloat("##cms", &cameraSpeed, 0.01f, 0.f, 999.f);
	}
}
#endif // !STANDALONE

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{

	if (App->moduleInput->GetMouseLayer() == MOUSE_LAYER::MOVE_CAMERA) {
		ProcessSceneKeyboard();
	}

	//ASK: This should be here to move camera with code but idk its expensive
	Reference += cameraMovement;
	editorCamera.Move(cameraMovement);
	cameraMovement = float3::zero;

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::ProcessSceneKeyboard()
{
	cameraMovement.Set(0.f, 0.f, 0.f);
	const float dt = App->GetDT();

	float speed = cameraSpeed * dt;
	if (App->moduleInput->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed *= 2.f;

	//if (App->moduleInput->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) cameraMovement.y += speed;
	//if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

	if (App->moduleInput->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) Zoom(speed);
	if (App->moduleInput->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) Zoom(-speed);


	//if (App->moduleInput->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) cameraMovement -= editorCamera.camFrustrum.WorldRight() * speed;
	//if (App->moduleInput->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) cameraMovement += editorCamera.camFrustrum.WorldRight() * speed;

	if (App->moduleInput->GetMouseZ() != 0)
	{
		Zoom(App->moduleInput->GetMouseZ());
	}

	// Mouse motion ----------------
	
	//BUG: Will lock the mouse forever if the user docks two camera windows together lol
	//if (App->moduleInput->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN /*|| App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN*/)
	//{
	//	SDL_SetRelativeMouseMode(SDL_TRUE);
	//	SDL_WarpMouseInWindow(App->moduleWindow->window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	//}
	//else if (App->moduleInput->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_UP /*|| App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP*/)
	//{
	//	SDL_SetRelativeMouseMode(SDL_FALSE);
	//	SDL_WarpMouseInWindow(App->moduleWindow->window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	//}

	//ASK: Is this really the best way to rotate the camera? Maybe i should use a matrix
	//TODO: Camera rotation should not be affected by the program framerate
	if (App->moduleInput->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		//FreeRotation(dt);
		OrbitalRotation(dynamic_cast<GO_Bridge*>(App->moduleRenderer3D->renderQueue[0])->localAABB.CenterPoint(), dt);
	}

#ifndef STANDALONE
	//Rotate around 0,0,0
	//ASK: Should i also include Right alt?
	//Maybe we could use quaternions?
	//if (/*App->moduleInput->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && */App->moduleInput->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) 
	//{
	//	/*float3 target(0.f, 0.f, 0.f);*/
	//	//if (App->moduleEditor->GetSelectedGO() != nullptr)
	//	//{
	//	//	float3 maTogl = App->moduleEditor->GetSelectedGO()->transform->globalTransform.TranslatePart();
	//	//	target.Set(maTogl.x, maTogl.y, maTogl.z);
	//	//}
	//	OrbitalRotation(Reference, dt);
	//}


	if (App->moduleInput->GetKey(SDL_SCANCODE_F) == KEY_DOWN) 
	{
		FocusCamera(dynamic_cast<GO_Bridge*>(App->moduleRenderer3D->renderQueue[0])->localAABB.CenterPoint(), 10.f);
	}
#endif // !STANDALONE


	//if (App->moduleInput->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	//	PanCamera(dt);
}

////Could be a good idea to use quaternions? Would it be faster?
////BUG: Camera will bug when the the horizontal rotation is almost 
////equal as the target X and Z coords
void ModuleCamera3D::OrbitalRotation(float3 center, float dt)
{
	int dx = -App->moduleInput->GetMouseXMotion();
	int dy = -App->moduleInput->GetMouseYMotion();

	float distance = editorCamera.camFrustrum.pos.Distance(center);

	Quat direction;
	editorCamera.camFrustrum.WorldMatrix().Decompose(float3(), direction, float3());

	//editorCamera.camFrustrum.pos = center;
	//ERROR, BUG, WARNING: CAMERA DOES A WEIRD FLIP WHEN ORBITATING PAST Y -1, Y 1, SHOULD INVERT WORLD X
	if (dy != 0)
	{
		float DeltaY = (float)dy * mouseSensitivity;

		Quat Y = Quat::identity;
		Y.SetFromAxisAngle(float3(1, 0, 0), DeltaY * DEGTORAD);
		//LOG("%f", Quat(0.0, 0.0, 0.707, 0.707).Dot(direction * Y));

		direction = direction * Y;
	}

	if (dx != 0)
	{
		float DeltaX = (float)dx * mouseSensitivity;

		Quat X = Quat::identity;
		X.SetFromAxisAngle(float3(0, 1, 0), DeltaX * DEGTORAD);

		direction = X * direction;
	}

	//if ((center + (editorCamera.camFrustrum.front * -distance)).y >= 0.25) 
	//{

		float4x4 mat = editorCamera.camFrustrum.WorldMatrix();
		mat.SetRotatePart(direction.Normalized());
		editorCamera.camFrustrum.SetWorldMatrix(mat.Float3x4Part());

		editorCamera.camFrustrum.pos = center + (editorCamera.camFrustrum.front * -distance);
		CLAMP(editorCamera.camFrustrum.pos.y, 0.5, 300);
		editorCamera.LookAt(center);
	//}
	//else 
	//{
	//	float3 ret = center + (editorCamera.camFrustrum.front * -distance);
	//	ret.y = 0.25f;
	//	editorCamera.camFrustrum.pos = ret;
	//	editorCamera.LookAt(center);
	//}
}

void ModuleCamera3D::FreeRotation(float dt)
{
	int dx = -App->moduleInput->GetMouseXMotion();
	int dy = -App->moduleInput->GetMouseYMotion();

	Quat direction;
	editorCamera.camFrustrum.WorldMatrix().Decompose(float3(), direction, float3());

	if (dy != 0)
	{
		float DeltaY = (float)dy * mouseSensitivity;

		Quat Y = Quat::identity;
		Y.SetFromAxisAngle(float3(1, 0, 0), DeltaY * DEGTORAD);

		direction = direction * Y;
	}

	if (dx != 0)
	{
		float DeltaX = (float)dx * mouseSensitivity;

		Quat X = Quat::identity;
		X.SetFromAxisAngle(float3(0, 1, 0), DeltaX * DEGTORAD);

		direction = X * direction;
	}

	float4x4 mat = editorCamera.camFrustrum.WorldMatrix();
	mat.SetRotatePart(direction.Normalized());
	editorCamera.camFrustrum.SetWorldMatrix(mat.Float3x4Part());

	//Reference = editorCamera.GetGO()->transform->GetForward() * editorCamera.GetPosition() - 
}

void ModuleCamera3D::FocusCamera(float3 center, float offset)
{
	editorCamera.LookAt(center);
	editorCamera.camFrustrum.pos = center + (((editorCamera.camFrustrum.pos - center).Normalized()) * offset);
}

void ModuleCamera3D::PanCamera(float dt)
{

	//WARNING: Need to normalize movement
	int dx = -App->moduleInput->GetMouseXMotion();
	int dy = -App->moduleInput->GetMouseYMotion();

	if (dx != 0 || dy != 0) 
	{
		float3 movVector((editorCamera.camFrustrum.WorldRight() * dx) + (-editorCamera.camFrustrum.up * dy));
		cameraMovement += movVector * dt;
	}
}

void ModuleCamera3D::Zoom(float increment)
{
	float3 centerPoint = dynamic_cast<GO_Bridge*>(App->moduleRenderer3D->renderQueue[0])->localAABB.CenterPoint();

	float distance = (editorCamera.camFrustrum.pos - centerPoint).Length() - increment;
	CLAMP(distance, 0.5, 20.0);
	editorCamera.camFrustrum.pos = centerPoint + (-editorCamera.camFrustrum.front * distance);
}
