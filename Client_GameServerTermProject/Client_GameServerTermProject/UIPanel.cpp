#include "pch.h"
#include "UIPanel.h"
#include "Button.h"
#include "ResourceManager.h"

UIPanel::UIPanel()
{
	//{
	//	Button* ui = new Button();
	//	//ui->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"Start"), BS_Default);
	//	ui->SetPos({ 400, 200 });
	//	ui->SetSize({ 650, 300 });
	//	AddChild(ui);
	//}
	{
		// ÇÁ·ÎÇÊ
		Button* ui = new Button();
		ui->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"Pro"), BS_Default);
		ui->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"Edit_Off"), BS_Clicked);
		VectorInt size = ui->GetSize();
		ui->SetPos({ static_cast<float>(size.x / 2), static_cast<float>(size.y / 2) });

		//ui->_onClickMemFunc = &UIPanel::OnClickStartButton;
		ui->AddOnClickDelegate(this, &UIPanel::OnClickStartButton);
		AddChild(ui);
	}
	{
		Button* ui = new Button();
		ui->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"Edit_Off"), BS_Default);
		ui->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"Edit_On"), BS_Clicked);
		ui->SetPos({ 400, 200 });
		ui->AddOnClickDelegate(this, &UIPanel::OnClickEditButton);
		AddChild(ui);
	}
	{
		Button* ui = new Button();
		ui->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"Exit_Off"), BS_Default);
		ui->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"Exit_On"), BS_Clicked);
		ui->SetPos({ 600, 200 });
		ui->AddOnClickDelegate(this, &UIPanel::OnClickExitButton);
		AddChild(ui);
	}
}

UIPanel::~UIPanel()
{

}

void UIPanel::BeginPlay()
{
	Super::BeginPlay();
}

void UIPanel::Tick()
{
	Super::Tick();


}

void UIPanel::Render(HDC hdc)
{
	Super::Render(hdc);

	wstring str = std::format(L"Count : {0}", _count);
	Utils::DrawTextW(hdc, { 400, 150 }, str);
}

void UIPanel::OnClickStartButton()
{
	_count++;
}

void UIPanel::OnClickEditButton()
{
	_count--;
}

void UIPanel::OnClickExitButton()
{
	_count = 0;
}
