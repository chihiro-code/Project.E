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
	// �p����ŏ������e������
}

void ComponentBase::Update()
{
	// �p����ŏ������e������
}

void ComponentBase::Draw()
{
	// �p����ŏ������e������
}

void ComponentBase::Release()
{
	// �p����ŏ������e������
}
