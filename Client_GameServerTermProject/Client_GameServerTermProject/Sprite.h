#pragma once
#include "ResourceBase.h"

class Texture;

class Sprite : public ResourceBase
{
public:
	Sprite(Texture* texture, int32 x, int32 y, int32 cx, int32 cy);
	virtual ~Sprite();

	HDC GetDC();
	int32 GetTransparent();
	VectorInt GetPos() { return VectorInt{ _x, _y }; }
	VectorInt GetSize() { return VectorInt{ _cx, _cy }; }

private:
	Texture* _texture = nullptr;
	int32 _x;
	int32 _y;
	int32 _cx;
	int32 _cy;
};

