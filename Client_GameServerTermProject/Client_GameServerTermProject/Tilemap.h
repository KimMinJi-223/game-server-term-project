#pragma once
#include "ResourceBase.h"

struct Tile
{
	int32 value = 0;
};

enum TILE_SIZE
{
	TILE_WIDTH = 63,
	TILE_HEIGHT = 43,
	TILE_SIZEX = 30,
	TILE_SIZEY = 30
};

class Tilemap : public ResourceBase
{
public:
	Tilemap();
	virtual ~Tilemap() override;

	virtual void LoadFile(const wstring& path) override;
	virtual void SaveFile(const wstring& path) override;

	VectorInt GetMapSize() { return _mapSize; }
	int32 GetTileSize() { return _tileSize; }
	Tile* GetTileAt(VectorInt pos);
	vector<vector<Tile>>& GetTiles() { return _tiles; }

	void SetMapSize(VectorInt size);
	void SetTileSize(int32 size);

private:
	VectorInt _mapSize = {};
	int32 _tileSize = {};
	vector<vector<Tile>> _tiles;
};

