#pragma once

class Scene;

class SceneManager
{
	DECLARE_SINGLE(SceneManager);

public:
	void Init();
	void Update();
	void Render(HDC hdc);

	void Clear();

public:
	void ChangeScene(SceneType sceneType);

	Scene* GetCurrentScene() { return _scene; }

private:
	Scene* _scene;
	SceneType _sceneType = SceneType::None;


public:
	Vector GetCameraPos() { return _cameraPos; }
	void SetCameraPos(Vector pos) { _cameraPos = pos; }

private:
	Vector _cameraPos = { GWinSizeX, GWinSizeY };

};

