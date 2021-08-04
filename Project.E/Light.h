#pragma once
#include <cstdio>
#include "DirectXMath.h"


using namespace DirectX;


//*************************************
// Light class
//*************************************
class Light 
{
	//*******************************************
	// �ϐ�
	//*******************************************
private:

	// �v���W�F�N�V����
	struct Projection {
		float fov;
		float aspect;
		float nearZ;
		float farZ;
	};

	XMMATRIX mViewMatrix;
	XMMATRIX mProjectionMatrix;
	XMFLOAT3 mPos;
	XMFLOAT3 mFocus;
	XMFLOAT4 mColor;
	Projection mProjection;
	float mMoveRot;


	//*******************************************
	// �֐�
	//*******************************************
public:

	Light();
	~Light();

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();
	XMFLOAT3 GetPosition();
	XMFLOAT4 GetColor();
	void SetPosition(float, float, float);
	void SetFocus(float, float, float);
	void SetColor(float, float, float, float);
	void SetProjection(float, float, float, float);
	void Init();
	void Update();
	
private:

	void ViewSet();

};


//*************************************
// �֐�
//*************************************

// �Q�b�g�֐�
Light* GetLight();

// ���C�g������
void LightInit();

// ���C�g�X�V
void LightUpdate();

// ���C�g���
void LightRelease();

