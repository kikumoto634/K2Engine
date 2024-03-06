#pragma once

class BaseScene;
class SceneManager
{
public:
	static SceneManager* GetInstance();
	static void Finalize();

	void Initialize(BaseScene* scene);
	void Update();
	void Draw();

	void SetNext(BaseScene* scene)	{next_ = scene;}

private:
	SceneManager() = default;
	~SceneManager() = default;

public:
	SceneManager(const SceneManager& obj) = delete;
	SceneManager* operator=(const SceneManager& obj) = delete;

private:
	static SceneManager* instance_;

public:
	BaseScene* current_ = nullptr;
	BaseScene* next_ = nullptr;
};

