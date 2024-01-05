#pragma once
#include <optional>

#include "../Geometry/ObjModel.h"

class Player : public ObjModel
{
private:
	enum class Behavior{
		kRoot,
		kAttack,
		Jump,
	};

public:
	Player(std::string filePath, Transform transform = {{0,0,0}, {0,0,0}, {0.8f,0.8f,0.8f}});

public:
	void Update()override;
	void Draw(Camera* camera) override;
	void OnCollision() override;

	//Getter/Setter
	Vector3 GetColliderCenterPos()	const override{
		return translate;
	};

private:
	void BehaviorRootInitialize();
	void BehaviorRootUpdate();

	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();

	void BehaviorJumpInitialize();
	void BehaviorJumpUpdate();


	void Input();
	void Move();
	void Attack();
	void Jump();

private:
	const float kGravityAcceleration = 0.05f;

	const float kMoveVelocity = 0.1f;
	const float kJumpVelocity = 1.0f;

private:
	//状態
	Behavior behavior_ = Behavior::kRoot;
	//次の状態
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//武器
	ObjModel* weapon_ = nullptr;
	float weaponAngle_ = 0.0f;
	float weaponAnimFrame_ = 0.f;

	//速度
	Vector3 velocity_ = {};
};

