#pragma once
struct EmitterData{
	Transform transform; // エミッタのTransform
	Vector2 velocity;	//射出速度
	int count;	//発生数
	float frequency; //発生頻度
	float frequencyTime; //頻度用時刻
};