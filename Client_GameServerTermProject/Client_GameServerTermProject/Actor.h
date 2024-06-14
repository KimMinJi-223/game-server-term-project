#pragma once
#include "pch.h"

class Component;
class Collider;
class Actor
{
public:
	Actor();
	virtual ~Actor();

	virtual void BeginPlay();
	virtual void Tick();
	virtual void Render(HDC hdc);

	void SetPos(Vector pos) { _pos = pos; }
	Vector GetPos() { return _pos; }

	void SetLayer(LAYER_TYPE layer) { _layer = layer; }
	LAYER_TYPE GetLayer() { return _layer; }

	Vector GetDestPos() { return _destPos; }

	void SetName(const char* name) { 
		memcpy_s(_name, NAME_SIZE, name, NAME_SIZE); 
		int a = 0;
	}
	char* GetName() { return _name; }

	void SetHp(int hp) { _hp = hp; }
	int GetHp() { return _hp; }

	void SetMaxHp(int hp) { _maxHp = hp; }
	int GetMaxHp() { return _maxHp; }

	void SetLevel(int level) { _level = level; }
	int GetLevel() { return _level; }

	void AddComponent(Component* component);
	void RemoveComponent(Component* component);

	virtual void OnComponentBeginOverlap(Collider* collider, Collider* other);
	virtual void OnComponentEndOverlap(Collider* collider, Collider* other);

protected:
	// 액터는 씬에 배치할 수 있다. 즉 좌표를 가진다. 
	Vector _pos = { 0,0 };
	Vector _destPos = { 0, 0 };
	LAYER_TYPE _layer = LAYER_OBJECT;
	vector<Component*> _components;
	int _level;
	int _hp = 100;
	int _maxHp = 100;
	char _name[NAME_SIZE];
};