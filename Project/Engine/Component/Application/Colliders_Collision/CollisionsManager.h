#pragma once
#include <forward_list>

#include "Geometry/Common/BaseCollider.h"

class CollisionsManager
{
public:
	static CollisionsManager* GetInstance();

public:
	CollisionsManager() = default;
	CollisionsManager(const CollisionsManager&) = delete;
	~CollisionsManager(){
		Reset();
	}
	CollisionsManager& operator=(const CollisionsManager&) = delete;

	void CheckAllCollisions();

	void AddCollider(BaseCollider* col)		{colliders_.push_front(col);}
	void RemoveCollider(BaseCollider* col)	{colliders_.remove(col);}
	void Reset()							{colliders_.clear();}

private:
	static CollisionsManager* instance;

private:
	std::forward_list<BaseCollider*> colliders_;
};

