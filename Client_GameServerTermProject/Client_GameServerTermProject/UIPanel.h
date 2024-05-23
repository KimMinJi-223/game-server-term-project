#pragma once
#include "Panel.h"

class UIPanel : public Panel
{
	using Super = Panel;
public:
	UIPanel();
	virtual ~UIPanel() override;

	virtual void BeginPlay() override;
	virtual void Tick() override;
	virtual void Render(HDC hdc) override;

	void OnClickStartButton();
	void OnClickEditButton();
	void OnClickExitButton();

	int32 _count = 0;
};