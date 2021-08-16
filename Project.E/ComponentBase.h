#pragma once
#include "GameObject.h"


//---------------------------------------------------------
// GameObject�Ɏ�������R���|�[�l���g��\�����N���X
// �S�ẴR���|�[�l���g�͂�����p�����č��
//---------------------------------------------------------


//*************************************
// ComponentBase class
//*************************************
class ComponentBase //: public GameObject
{
	//*******************************************
	// �ϐ�
	//*******************************************
protected:

	GameObject* mpOwner; // GameObject�C���X�^���X�̃A�h���X�ۑ�


	//*******************************************
	// �֐�
	//*******************************************
public:
	ComponentBase();
	~ComponentBase();

	// GameObject�C���X�^���X�̃A�h���X��ۑ�����
	void SetOwner(GameObject*);

	// �I�[�o�[���C�h�œ��e���㏑������
	virtual void Init();
	virtual void Update();
	virtual void Draw();
	virtual void Release();

};

