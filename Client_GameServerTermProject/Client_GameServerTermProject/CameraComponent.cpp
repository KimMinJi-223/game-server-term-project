#include "pch.h"
#include "CameraComponent.h"
#include "Actor.h"
#include "SceneManager.h"

CameraComponent::CameraComponent()
{
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::BeginPlay()
{

}

void CameraComponent::Tick()
{
	if (_owner == nullptr)
		return;

	Vector pos = _owner->GetPos();

	pos.x = ::clamp(pos.x, GWinSizeX / 2.f, GMapSizeX * GTilesSize - GWinSizeX / 2.f);
	pos.y = ::clamp(pos.y, GWinSizeY / 2.f, GMapSizeY * GTilesSize - GWinSizeY / 2.f);

	GET_SINGLE(SceneManager)->SetCameraPos(pos);
}

void CameraComponent::Render(HDC hdc)
{

}
