#pragma once
#include "Component.h"

class CameraComponent : public Component
{
public:
	CameraComponent();
	virtual ~CameraComponent() override;

	virtual void BeginPlay() override;
	virtual void Tick() override;
	virtual void Render(HDC hdc) override;
};

