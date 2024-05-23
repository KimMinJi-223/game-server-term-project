#pragma once
#include "ResourceBase.h"

class Texture;

struct FlipbookInfo
{
	Texture* texture = nullptr;
	wstring name;
	VectorInt size = {};
	int32 start;
	int32 end;
	int32 line;
	float duration = 1.f;
	bool loop = true;
};

class Flipbook : public ResourceBase
{
public:
	Flipbook();
	virtual ~Flipbook();

	void SetInfo(const FlipbookInfo& info) { _info = info; }
	const FlipbookInfo& GetInfo() { return _info; }

private:
	FlipbookInfo _info;
};

