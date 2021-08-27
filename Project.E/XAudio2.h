#pragma once


//=============================================================================
//
// �T�E���h���� [XAudio2.h]
//
//=============================================================================
#ifndef _XAUDIO2_H_
#define _XAUDIO2_H_

#include <xaudio2.h>

// �T�E���h�t�@�C���i�����ɃT�E���h����ǉ����Acpp�̕��Ńt�@�C���p�X�����Ԓʂ�ɏ������I�j
typedef enum
{
	//-------------------------------------------
	// BGM
	//-------------------------------------------
	BGM_BATTLE_01,		 // �퓬01 BGM


	//-------------------------------------------
	// SE
	//-------------------------------------------
	SE_HIT_01,           // �q�b�g��


	SOUND_LABEL_MAX
} SOUND_LABEL;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSound(void);
void UninitSound(void);
void PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void PauseSound(SOUND_LABEL label);
void UpdateSound(void);
#endif