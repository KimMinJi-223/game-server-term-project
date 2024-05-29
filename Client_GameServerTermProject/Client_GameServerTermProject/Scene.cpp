#include "pch.h"
#include "Scene.h"
#include "Actor.h"
#include "UI.h"
#include "TimeManager.h"
#include "SceneManager.h"
//#include "CollisionManager.h"

Scene::Scene()
{
}

Scene::~Scene()
{
	delete controller;
}

void Scene::Init()
{
	controller = new PlayerController();

	for (const vector<Actor*>& actors : _actors)
	{
		for (Actor* actor : actors)
			actor->BeginPlay();;
	}

	for (UI* ui : _uis)
	{
		ui->BeginPlay();
	}
	for (auto p : players)
		p.second->BeginPlay();

	avatar->BeginPlay();
}

void Scene::Update()
{
	controller->Update();
	// 이전 프레임에서 현재 프레임까지 경과된 시간 
	// 거리 = 시간 * 속도
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();

	//GET_SINGLE(CollisionManager)->Update();

	for (const vector<Actor*>& actors : _actors)
	{
		for (Actor* actor : actors)
			actor->Tick();
	}

	for (UI* ui : _uis)
	{
		ui->Tick();
	}

	for (auto p : players)
		p.second->Tick();

	avatar->Tick();
}

void Scene::Render(HDC hdc)
{
	// 여기 ui를 분리해야함
	for (const vector<Actor*>& actors : _actors)
	{
		for (Actor* actor : actors)
			actor->Render(hdc);
	}

	for (UI* ui : _uis)
	{
		ui->Render(hdc);
	}

	for (auto p : players)
		p.second->Render(hdc);

	avatar->Render(hdc);
}

void Scene::AddActor(Actor* actor)
{
	if (actor == nullptr)
		return;

	_actors[actor->GetLayer()].push_back(actor);
}

void Scene::RemoveActor(Actor* actor)
{
	if (actor == nullptr)
		return;

	vector<Actor*>& v = _actors[actor->GetLayer()];

	v.erase(std::remove(v.begin(), v.end(), actor), v.end());
}
