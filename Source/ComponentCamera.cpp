#pragma once

#include "Application.h"
#include "GameObject.h"

#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleScene.h"

#include "ComponentCamera.h"
#include "ComponentTransform.h"

#include "glmath.h"
#include "Dependencies/MathGeoLib/include/Math/float3.h"
#include "Dependencies/MathGeoLib/include/Math/float3x3.h"
#include "Dependencies/MathGeoLib/include/Math/Quat.h"

#include "Dependencies/ImGUI/imgui.h"

ComponentCamera::ComponentCamera(GameObject* owner) : Component(owner),
active_camera(false),
looking_at(float3::zero)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPos(float3(0, 0, 0));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	if(owner != nullptr)
	{
		ComponentTransform* transform = (ComponentTransform*)owner->GetComponent(ComponentType::Transform);
		//transform->UpdateTransform(frustum.Pos(), frustum.());
		transform->UpdateLocalTransform();
	}

	frustum.SetViewPlaneDistances(1.0f, 1000.0f);
	frustum.SetPerspective(1.0f, 1.0f);
	SetAspectRatio((float)App->window->Width() / (float)App->window->Height());
	SetFOV(50.0f);

	//Look(float3(0.0f, 0.0f, 0.0f));

	type = ComponentType::Camera;

	draw_boundingboxes = true;
	frustum_culling = false;
	active_camera = false;
}

void ComponentCamera::Enable() {}

void ComponentCamera::Disable() {}

void ComponentCamera::Update() {}

void ComponentCamera::CleanUp() {}

float ComponentCamera::GetFOV() const
{
	return frustum.VerticalFov() * RADTODEG;
}

float ComponentCamera::GetAspectRatio() const
{
	return frustum.AspectRatio();
}

void ComponentCamera::SetFOV(float fov) 
{
	frustum.SetVerticalFovAndAspectRatio(fov * DEGTORAD, ((float)App->window->Width() / (float)App->window->Height()));
}

void ComponentCamera::SetAspectRatio(float aspectratio) {
	
	frustum.SetHorizontalFovAndAspectRatio(frustum.HorizontalFov(), aspectratio);
}

float ComponentCamera::GetNearPlane() const
{
	return frustum.NearPlaneDistance();
}

void ComponentCamera::SetNearPlane(float distance)
{
	if(distance > 0.0f && distance < frustum.FarPlaneDistance())

	frustum.SetViewPlaneDistances(distance, frustum.FarPlaneDistance());
}

float ComponentCamera::GetFarPlane() const
{
	return frustum.FarPlaneDistance();
}

void ComponentCamera::SetFarPlane(float distance)
{
	if (distance > 0.0f && distance > frustum.NearPlaneDistance())

	frustum.SetViewPlaneDistances(frustum.NearPlaneDistance(), distance);
}

vec* ComponentCamera::GetFrustumPoints() const
{
	static vec frustum_corners[8];
	frustum.GetCornerPoints(frustum_corners);
	
	return frustum_corners;
}

void ComponentCamera::OnUpdateTransform(const float4x4& global, const float4x4& parent_global)
{
	frustum.SetFront(global.WorldZ());
	frustum.SetUp(global.WorldY());

	//Init just in case
	float3 position = float3::zero;
	float3 scale = float3::one;
	Quat   rotation = Quat::identity;

	global.Decompose(position, rotation, scale);

	frustum.SetPos(position);
}

float* ComponentCamera::GetRawViewMatrix()
{
	static float4x4 m;

	m = frustum.ViewMatrix();
	m.Transpose();

	return (float*)m.v;
}

math::float4x4 ComponentCamera::GetAlternativeViewMatrix() const
{
	math::float4x4 matrix = frustum.ViewMatrix();
	return matrix.Transposed();
}
math::float4x4 ComponentCamera::GetAlternativeProjectionMatrix() const
{
	math::float4x4 matrix = frustum.ProjectionMatrix();
	matrix.Transpose();
	return matrix;
}

mat4x4 ComponentCamera::GetViewMatrix()
{
	static float4x4 m;
	//static mat4x4 m;

	m = frustum.ViewMatrix();
	m.Transpose();


	float3 position = float3::zero;
	float3 scale = float3::one;
	Quat   rotation = Quat::identity;

	m.Decompose(position, rotation, scale);

	float3 rot_axis;
	float rot_angle;

	rotation.ToAxisAngle(rot_axis, rot_angle);

	vec3 rot_axis_asvec;
	rot_axis_asvec.x = rot_axis.x;
	rot_axis_asvec.y = rot_axis.y;
	rot_axis_asvec.z = rot_axis.z;

	mat4x4 a;
	a.translate(position.x, position.y, position.z);
	a.rotate(rot_angle, rot_axis_asvec);
	a.scale(scale.x, scale.y, scale.z);

	return a;
}

float* ComponentCamera::GetProjectionMatrix()
{
	static float4x4 m;

	m = frustum.ProjectionMatrix();
	m.Transpose();

	return (float*)m.v;
}

void ComponentCamera::Look(float3& point_to_look)
{
	float3 direction = point_to_look - frustum.Pos();

	float3x3 mat = float3x3::LookAt(frustum.Front(), direction.Normalized(), frustum.Up(), float3::unitY);

	frustum.SetFront(mat.MulDir(frustum.Front()).Normalized());
	frustum.SetUp(mat.MulDir(frustum.Up()).Normalized());

	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::Transform);
	transform->SetLocalTransform(frustum.WorldMatrix());

	//float3 fwd = direction.Normalized();
	//float3 right = float3::unitY.Cross(fwd).Normalized();
	//float3 up = fwd.Cross(right).Normalized();


	//transform->GetLocalTransform().SetCol3(0, right);
	//transform->GetLocalTransform().SetCol3(1, up);
	//transform->GetLocalTransform().SetCol3(2, fwd);
	//transform->SetLocalTransform(transform->GetLocalTransform());

}

void ComponentCamera::Match(ComponentCamera* reference)
{
	frustum.SetPos(reference->frustum.Pos());
	frustum.SetFront(reference->frustum.Front());
	frustum.SetUp(reference->frustum.Up());
}

void ComponentCamera::Pan(float motion_x, float motion_y)
{
	float distance = App->camera->reference.Distance(frustum.Pos());

	float3 Y_add = frustum.Up() * motion_y * (distance / 1000);
	float3 X_add = frustum.WorldRight() * -motion_x * (distance / 1000);

	App->camera->reference += X_add;
	App->camera->reference += Y_add;

	frustum.SetPos(frustum.Pos() + X_add + Y_add);


	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::Transform);
	transform->SetPosition(transform->GetPosition() + X_add + Y_add);
}

void ComponentCamera::Orbit(float motion_x, float motion_y)
{

	if(App->scene->selected_object != nullptr)
	{
		looking_at = App->scene->selected_object->transform->GetPosition();
	}

	float3 focus = frustum.Pos() - looking_at;


	if (motion_x != 0.0f)
	{
		Quat newX = Quat(frustum.Up(), -motion_x );
		focus = newX.Transform(focus);
	}

	if (motion_y != 0.0f)
	{
		Quat newY = Quat(frustum.WorldRight(), -motion_y );
		focus = newY.Transform(focus);
	}


	//Quat y = Quat(frustum.Up(), motion_x );
	//Quat x = Quat(frustum.WorldRight(), motion_y );

	//focus = x.Transform(focus);
	//focus = y.Transform(focus);


	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::Transform);
	transform->SetPosition(focus + looking_at);



	//frustum.SetPos(focus+ looking_at);



	Look(looking_at);


}

void ComponentCamera::Zoom(float motion_z)
{
	float dist = 0;
	if (!looking) 
	{
		dist = looking_at.Distance(frustum.Pos());
	}

	float3 vec = frustum.Front() * motion_z;
	frustum.SetPos(frustum.Pos()+vec);


	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::Transform);
	transform->SetPosition(transform->GetPosition() + transform->local_transform.WorldZ() * motion_z );
}

void ComponentCamera::LookAt(float motion_x, float motion_y)
{
	looking = false;


	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::Transform);

	// x motion rotates in Y absolute axis
	if(motion_x != 0.f)
	{
		Quat q = Quat::RotateY(motion_x);

		float3 new_Axis = q.Mul(frustum.Up().Normalized());

		frustum.SetFront(q.Mul(frustum.Front()).Normalized());
		frustum.SetUp(q.Mul(frustum.Up()).Normalized());

		
	}
	
	// y motion rotates in X local axis, with tops
	if (motion_y != 0.f)
	{
		Quat q = Quat::RotateAxisAngle(frustum.WorldRight(), motion_y);

		float3 new_yAxis = q.Mul(frustum.Up().Normalized());
		

		frustum.SetUp(new_yAxis);
		frustum.SetFront(q.Mul(frustum.Front().Normalized()));


		//Avoid upside down camera movement 
	/*	if(new_yAxis.y > 0.0f)
		{
			frustum.SetUp(new_yAxis);
			frustum.SetFront(q.Mul(frustum.Front().Normalized()));
		}*/
	}

	transform->SetLocalTransform(frustum.WorldMatrix());
}

void ComponentCamera::OnClick(float2 mousePOs)
{

	float normalPos_x = mousePOs.x / (float)App->window->Width();
	float normalPos_y = mousePOs.y / (float)App->window->Height();

	normalPos_x = (normalPos_x - 0.5) / 0.5;
	normalPos_y = (normalPos_y - 0.5) / 0.5;

	latest_ray = frustum.UnProjectLineSegment(normalPos_x, normalPos_y);
	
	App->scene->TestGameObjectSelection(latest_ray);
}
