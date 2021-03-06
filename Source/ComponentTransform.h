#pragma once
#include "Component.h"
#include "Dependencies/MathGeoLib/include/Math/float3.h"
#include "Dependencies/MathGeoLib/include/Math/float4x4.h"
#include "Dependencies/MathGeoLib/include/Math/Quat.h"
#include "Globals.h"


class ComponentTransform : public Component {
	//Properties
public:
	bool updatedtransform = false;

	math::float3		local_position;
	math::float3		local_scale;
	Quat		local_rotation;

	math::float3		local_eulerRotation = math::float3(0.f, 0.f, 0.f);

	math::float3		global_position;
	math::float3		global_scale;
	Quat		global_rotation;


	math::float3		global_eulerRotation = math::float3(0.f, 0.f, 0.f);

	float4x4	local_transform;
	float4x4	global_transform;

private:
	
	bool updateGlobalTrans = false;


	//Methods
public:
	void Enable() override;
	void Update() override;
	void Disable() override;
	void CleanUp() override;
	Component* GetComponent(ComponentType type);

	void UpdateTransform(math::float3 position, math::float3 scale, Quat rotation);

	void inline SetPosition(math::float3 position) { this->local_position = position;  UpdateLocalTransform();};
	void inline SetRotation(Quat rotation) { this->local_rotation = rotation; UpdateLocalTransform();};
	void inline SetScale(math::float3 scale) { this->local_scale = scale; UpdateLocalTransform(); };

	void inline SetGlobalPosition(math::float3 position) { this->global_position = position;  UpdateGlobalTransform();};
	void inline SetGlobalRotation(Quat rotation) { this->global_rotation = rotation; UpdateGlobalTransform(); };
	void inline SetGlobalScale(math::float3 scale) { this->global_scale = scale; UpdateGlobalTransform(); };

	void inline SetGlobalTransform(math::float3 position, math::float3 scale, Quat rotation) { global_transform = float4x4::FromTRS(position, rotation, scale); UpdateGlobalTransform();}

	void SetLocalEulerRotation(math::float3 euler_angles);
	void SetGlobalEulerRotation(math::float3 euler_angles);


	void SetGlobalTransform(float4x4 global_matrix);
	void SetLocalTransform(float4x4 global_matrix);

	math::float3 inline GetPosition() const { return this->local_position ; };
	Quat inline GetRotation() const { return this->local_rotation; };
	math::float3 inline GetScale() const { return this->local_scale; };

	math::float3 inline GetGlobalPosition() const { return this->global_position; };
	Quat inline GetGlobalRotation() const { return this->global_rotation; };
	math::float3 inline GetGlobalScale() const { return this->global_scale; };

	float4x4 inline GetLocalTransform() const { return this->local_transform; };
	float4x4 inline GetGlobalTransform() const { return this->global_transform; };

	ComponentTransform(GameObject* parent); //DEFAULTS TO 0,0
	ComponentTransform(GameObject* parent, math::float3 position, math::float3 scale, Quat rotation);
	ComponentTransform(GameObject* owner, math::float3 position, math::float3 scale, Quat rotation, math::float3 globalPosition, math::float3 globalScale, Quat globalRotation);
	~ComponentTransform();

	void UpdateLocalTransform();
	void UpdateGlobalTransform();
	void UpdateTRS();
	void UpdateEulerAngles();

};