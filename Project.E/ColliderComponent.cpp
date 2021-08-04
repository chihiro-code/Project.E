#include "ColliderComponent.h"
#include "GameObject.h"


//*************************************
// グローバル変数
//*************************************


//*************************************
// ColliderComponent関数
//*************************************
ColliderComponent::ColliderComponent()
{
	// 球
	mSphere.centerPos = { 0,0,0 };
	mSphere.radius = 0;

	// 直方体
	mAabb.centerPos = { 0,0,0 };
	mAabb.length = { 0,0,0 };
	mAabb.rot = { 0,0,0 };
}

ColliderComponent::~ColliderComponent()
{

}

void ColliderComponent::Init()
{
	GameObject* pColliderObject;
	pColliderObject = new GameObject;

}

void ColliderComponent::Update()
{

}

void ColliderComponent::Release()
{

}

void ColliderComponent::SetSphere(XMFLOAT3 centerPos, float radius)
{
	mSphere.centerPos = centerPos;
	mSphere.radius = radius;
}

void ColliderComponent::SetAabb(XMFLOAT3 centerPos, XMFLOAT3 length, XMFLOAT3 rot)
{
	mAabb.centerPos = centerPos;
	mAabb.length = length;
	mAabb.rot = rot;
}

bool ColliderComponent::GetSphere()
{
	return false;
}

bool ColliderComponent::GetAabb()
{
	return false;
}
