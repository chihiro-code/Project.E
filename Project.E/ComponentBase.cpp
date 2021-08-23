#include "ComponentBase.h"


ComponentBase::ComponentBase()
{

}

ComponentBase::~ComponentBase()
{
	
}

void ComponentBase::SetOwner(GameObject* obj)
{
	mpOwner = obj;
}

void ComponentBase::Init()
{
	// 継承先で処理内容を書く
}

void ComponentBase::Update()
{
	// 継承先で処理内容を書く
}

void ComponentBase::Draw()
{
	// 継承先で処理内容を書く
}

void ComponentBase::Release()
{
	// 継承先で処理内容を書く
}
