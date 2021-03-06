#pragma once
#include "Module.h"
#include "Timer.h"
#include "Dependencies/MathGeoLib/include/Geometry/LineSegment.h"
#include "Dependencies/ImGuizmo/ImGuizmo.h"
#include <unordered_map>
#include <map>

class GameObject;
class Primitive;
class ResourceMesh;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

	std::map<uint32, GameObject*> FillGameObjectsMap();
	void FillGameObjectsVector(std::unordered_map<uint32, GameObject*> gameObjectsMap);

	void AddGameObject(GameObject* object);
	GameObject* CreateGameObject(std::string name, GameObject* parent = nullptr);
	void SelectObject(GameObject* object);
	void DeleteGameObject(GameObject* object);
	void DeleteAllGameObjects();

	GameObject* CreateGameCamera();

	GameObject* CreateEditorCamera();

	void TestGameObjectSelection(const LineSegment& ray);
	//void ImGuizmoHandling();

private:

	

private:
	std::vector<Primitive*> primitives;

public:

	std::vector<GameObject*> game_objects;

	GameObject* root_object = nullptr;
	GameObject* selected_object = nullptr;

	Timer GameTime;
	std::string sceneLibraryPath;
	uint32 sceneUID;
};
