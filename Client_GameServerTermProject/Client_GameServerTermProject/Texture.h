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
	// ������ �����̴�. (���ϸ��� �ٸ���)
	uint32 _transparent = RGB(255, 0, 255);
};

