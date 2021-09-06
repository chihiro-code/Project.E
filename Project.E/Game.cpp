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
#include "XAudio2.h"
#include "Player.h"
#include "UI.h"


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

	// UI
	UIInit();

	// �Q�[���I�u�W�F�N�g
	GameObjectInit();

	// �O���b�h�}�b�v
	MapInit();

	// �v���C���[
	PlayerInit();

	// �V�F�[�_�[
	ShaderInit();

	// XAudio
	InitSound();


	return S_OK;
}


//*************************************
// �Q�[���X�V
//*************************************
void Game_Update(void)
{
	// XAudio�X�V
	UpdateSound();

	// �J����
	CameraUpdate();

	// �v���C���[
	PlayerUpdate();

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
	

	//===========================================
	// �������@�`�揈���������ɏ����@������


	//---------------------------------
	// �V���h�E�}�b�v�쐬
	//---------------------------------
	manager->RenderTargetsSetShadow();
	//manager->RenderTargetsSetShadowNoMotion();

	// �Q�[���I�u�W�F�N�g
	GameObjectDrawShadow();

	// �O���b�h�}�b�v
	MapDrawShadow();


	//---------------------------------
	// ���f���`��
	//---------------------------------
	manager->RenderTargetsSet();

	// �Q�[���I�u�W�F�N�g
	GameObjectDraw();

	// �O���b�h�}�b�v
	MapDraw();

	// UI
	UIDraw();


	// �������@�`�揈���������ɏ����@������
	//===========================================

	// �_�u���E�o�b�t�@�̃f�B�X�v���C�̈�ւ̃R�s�[����
	manager->UpdateScreen();

}


//*************************************
// �Q�[�����
//*************************************
void Game_Release(void)
{
	// XAudio���
	UninitSound();

	// �V�F�[�_�[
	ShaderRelease();

	// �v���C���[
	PlayerRelease();

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