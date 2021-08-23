#include <dwrite.h>
#include <stdio.h>
#include "DirectX11Manager.h"
#include "Game.h"
#include "input.h"
#include "camera.h"
#include "ToonMap.h"
#include "Light.h"
#include "GameObject.h"
#include "ShaderClass.h"
#include "Map.h"


//*************************************
// �O���[�o���ϐ�
//*************************************


//*************************************
// �Q�[��������
//*************************************
HRESULT Game_Initialize(void)
{
	HRESULT hr;

	// �J����
	CameraInit();

	// ToonMap
	ToonMap_Init();

	//���C�g
	LightInit();

	// �Q�[���I�u�W�F�N�g
	GameObjectInit();

	// �O���b�h�}�b�v
	MapInit();

	// �V�F�[�_�[
	ShaderInit();

	return S_OK;
}


//*************************************
// �Q�[���X�V
//*************************************
void Game_Update(void)
{
	// �J����
	CameraUpdate();

	// �Q�[���I�u�W�F�N�g
	GameObjectUpdate();

	// ���C�g
	LightUpdate();

}


//*************************************
// �Q�[���`��
//*************************************
void Game_Render(void)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	
	// ��ʃN���A
	manager->ClearScreen();
	manager->RenderTargetsSet();

	//------------------------------------
	// �������@�`�揈���������ɏ����@������


	// �Q�[���I�u�W�F�N�g
	GameObjectDraw();

	// �O���b�h�}�b�v
	MapDraw();


	// �������@�`�揈���������ɏ����@������
	//------------------------------------

	// �_�u���E�o�b�t�@�̃f�B�X�v���C�̈�ւ̃R�s�[����
	manager->UpdateScreen();

}


//*************************************
// �Q�[�����
//*************************************
void Game_Release(void)
{
	// �V�F�[�_�[
	ShaderRelease();

	// �Q�[���I�u�W�F�N�g
	GameObjectRelease();

	// �O���b�h�}�b�v
	MapRelease();

	// ���C�g
	LightRelease();

	// ToonMap
	ToonMap_Release();

	// �J�������
	CameraRelease();

}