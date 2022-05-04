#pragma once

#include<vector>
#include<string>

#include "Component.h"
#include"parson/parson.h"
#include"Primitive.h"

class C_Transform;

class GameObject
{
public:

	GameObject(const char*, GameObject* parent, int _uid = -1);
	virtual ~GameObject();

	void Update();

	Component* AddComponent(Component::Type _type, const char* params = nullptr);

	void RecursiveUIDRegeneration();

	bool isActive() const;

	virtual void Draw() {

	}
	virtual void DrawOptionsMenu() {

	}

	void Enable();
	void Disable();
	bool IsRoot();

	void Destroy();

	void SaveToJson(JSON_Array*);
	void LoadFromJson(JSON_Object*);

	void LoadComponents(JSON_Array* componentArray);
	void RemoveComponent(Component* ptr);

	void ChangeParent(GameObject*);
	bool IsChild(GameObject*);

	void RemoveChild(GameObject*);

	//void CompilePrimitivesToMesh();

	template<typename A>
	A* GetComponent()
	{
		return (A*)GetComponent(A::GetType());
	}



	GameObject* parent;
	C_Transform* transform;

	std::vector<GameObject*> children;
	std::vector<Component*> components;


	std::vector<Primitive> objPrimitives;
	ResourceShader* generalShader;
	

public:
	std::string name;
	bool active, isStatic;

	bool toDelete;
	int UID;

private:
	Component* GetComponent(Component::Type _type);
	Component* dumpComponent;

};