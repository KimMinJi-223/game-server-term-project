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
}

void Scene::Init()
{
	for (const vector<Actor*>& actors : _actors)
	{
		for (Actor* actor : actors)
			actor->BeginPlay();;
	}

	for (UI* ui : _uis)
	{
		ui->BeginPlay();
	}
}

void Scene::Update()
{
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
}

void Scene::Render(HDC hdc)
{
	for (const vector<Actor*>& actors : _actors)
	{
		for (Actor* actor : actors)
			actor->Render(hdc);
	}

	for (UI* ui : _uis)
	{
		ui->Render(hdc);
	}
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
