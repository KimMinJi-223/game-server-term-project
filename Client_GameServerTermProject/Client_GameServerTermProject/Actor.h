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
	int _hp = 100;
};