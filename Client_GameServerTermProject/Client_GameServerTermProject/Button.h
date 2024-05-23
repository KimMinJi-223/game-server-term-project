#pragma once
#include "UI.h"

class Sprite;

enum ButtonState
{
	BS_Default,
	//BS_Hovered, // 버튼에 마우스를 올린 상태
	BS_Pressed,
	BS_Clicked,

	BS_MaxCount
};

class Button : public UI
{
	using Super = UI;
public:
	Button();
	virtual ~Button() override;

	virtual void BeginPlay() override;
	virtual void Tick() override;
	virtual void Render(HDC hdc) override;

	void	SetSize(VectorInt size) { _size = size; }
	VectorInt	GetSize() { return _size; }
	Sprite* GetSprite(ButtonState state) { return _sprites[_state]; }
	
	void SetCurrentSprite(Sprite* sprite) { _currentSprite = sprite; }
	void SetSprite(Sprite* sprite, ButtonState state) { _sprites[state] = sprite; }
	void SetButtonState(ButtonState state);

	void OnClickButton() {}

protected:
	Sprite* _currentSprite = nullptr;
	Sprite* _sprites[BS_MaxCount] = {};
	ButtonState _state = BS_Default;

	float _sumTime = 0.f;
	
public:
	static void Test() { }
	//using OnClickButtonFuncType = void(*)();
	//OnClickButtonFuncType _onClickFunc = nullptr;

	void TestMem(){}
	//using OnClickButtonMenFuncType = void(Button::*)();
	//Button* _owner = nullptr;
	//OnClickButtonMenFuncType _onClickMemFunc = nullptr;

	template<typename T>
	void AddOnClickDelegate(T* owner, void(T::* func)())
	{
		_onClick = [owner, func]()
			{
				(owner->*func)();
			};

	}

	std::function<void(void)> _onClick = nullptr;

};

