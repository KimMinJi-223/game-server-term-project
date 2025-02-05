#pragma once
#include "Player.h"
#include "Monster.h"
#include "PlayerController.h"

class Actor;
class UI;

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void Init();
	virtual void Update();
	virtual void Render(HDC hdc);

public:
	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);

	PlayerController* GetPlayerController() { return controller; }

public:
	vector<Actor*> _actors[LAYER_MAXCOUNT];
	vector<UI*> _uis;

	Player* avatar;
	unordered_map <int, Player*> players;
	unordered_map <int, Monster*> monsters;

private:
	PlayerController* controller;
};

