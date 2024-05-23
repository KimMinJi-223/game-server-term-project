#pragma once

enum class SceneType
{
	None,
	DevScene,
	EditScene,
};

enum class PlayerType
{
	CanonTank,
	MissileTank
};

enum LAYER_TYPE
{
	LAYER_BACKGROUND,
	LAYER_OBJECT,
	// 
	LAYER_UI,

	LAYER_MAXCOUNT
};

enum class ColliderType
{
	// 충돌 모양은 대표적으로 박스와 구가 있다. 
	Box,
	Sphere,
};

enum COLLISION_LAYER_TYPE
{
	CLT_OBJECT,
	CLT_GROUND,
	CLT_WALL,
	// ..
};

enum Dir
{
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
};