#pragma once
#include "ResourceBase.h"

class Texture : public ResourceBase
{
public:
	Texture();
	virtual ~Texture();

public :
	Texture* LoadBmp(HWND hwnd, const wstring& path);
	HDC GetDC();
	void SetSize(VectorInt size) { _size = size; }
	VectorInt GetSize() { return _size; }

	void SetTransparent(uint32 transparent) { _transparent = transparent; }
	uint32 GetTransparent() { return _transparent; }

private:
	HDC _hdc = {};
	HBITMAP _bitmap = {};
	VectorInt _size = {};
	// 제거할 배경색이다. (파일마다 다르다)
	uint32 _transparent = RGB(255, 0, 255);
};

