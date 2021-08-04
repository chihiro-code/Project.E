//=============================================================================
//
// DirectX Project-E [main.cpp]
//
//=============================================================================
#undef UNICODE

#include "DirectX11Manager.h"
#include "Game.h"
#include "input.h"
#include <timeapi.h>
#include <crtdbg.h> // ���������[�N���o�p

#pragma comment (lib, "Winmm.lib")


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
HWND ghWnd;
int gFps;  // FPS�\���p�̕ϐ�


//=============================================================================
// ���C���֐�
//  (WINAPI : Win32API�֐����Ăяo�����̋K��)
//  hInstance     : ���̃A�v���P�[�V�����̃C���X�^���X�n���h��(���ʎq)
//  hPrevInstance : ���NULL(16bit����̖��c��)
//  lpCmdLine     : �R�}���h���C������󂯎���������ւ̃|�C���^
//  nCmdShow      : �A�v���P�[�V�����̏����E�C���h�E�\�����@�̎w��
//=============================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//*****************************************************************************
	// Windows�֌W�̏�����
	// DirectX�̏�����
	//*****************************************************************************

	HRESULT hr;
	hr = DirectX11Init(hInstance, nCmdShow);

	DirectX11Manager* manager = GetDirectX11Manager();

	if (FAILED(hr)) {
		goto MAIN_LOOP_END;	
	}


	//*****************************************************************************
	// �Q�[��������
	//*****************************************************************************

	hr = Game_Initialize();
	if (FAILED(hr)) {
		MessageBox(manager->mhWnd, "[Game_Initialize]�Ɏ��s���܂����B", "�G���[����", MB_OK | MB_ICONERROR);
		goto MAIN_LOOP_END;
	}
	
	// �C���v�b�g�n�֐����g����悤�ɂ��鏉����
	InitInput(hInstance, manager->mhWnd);

	// �J�[�\��������
	ShowCursor(FALSE);


	//*****************************************************************************
	// �Q�[�����[�v
	//*****************************************************************************
	// ���b�Z�[�W���[�v
	// ��P�����F���b�Z�[�W���
	// ��Q�����F�E�B���h�E�̃n���h��
	// ��R�����F�ŏ��̃��b�Z�[�W
	// ��S�����F�Ō�̃��b�Z�[�W
	MSG msg;
	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			if (msg.message == WM_QUIT) {
				break;
			}
			else {
				// �Ăяo�����X���b�h�̃��b�Z�[�W�L���[���烁�b�Z�[�W���擾
				TranslateMessage(&msg);	// ���z�L�[���b�Z�[�W�𕶎����b�Z�[�W�֕ϊ�
				DispatchMessage(&msg);	// �E�B���h�E�v���V�[�W���փ��b�Z�[�W�𑗏o
			}
		}
		else {  // ���b�Z�[�W�������������Ă��Ȃ���
			// ���@�Q�[���Ɋւ���v���O�����������ꏊ ////////////////////////////////////////////////////////
			// ���Ԃ��擾����
			static DWORD lastTime = 0;  // �O��t���[�������s���ꂽ���̃~���b������ϐ�

			timeBeginPeriod(1);
			DWORD currentTime = timeGetTime(); // Windows���N�����Ă��猻�݂܂ł̌o�ߎ��Ԃ��~���b�P�ʂŎ���
			timeEndPeriod(1);

			static DWORD everyLastTime; // �O��̃��[�v���̎��Ԃ�ۑ�����ϐ�
			static int fpsCnt;          // FPS�J�E���g�p�ϐ�

			// �~���b��1000�̈ʂ��ς������P�b�o�����Ƃ�������
			if (currentTime / 1000 > everyLastTime / 1000) {
				gFps = fpsCnt; // �\���p�̕ϐ��ɃJ�E���g����FPS�̒l��ۑ�
				fpsCnt = 0;    // �J�E���g�p�ϐ����O�ɖ߂�
			}
			everyLastTime = currentTime;

			if (currentTime >= lastTime + 16) { // �O��t���[�������s����Ă���1000/60�~���b�o�߂������H �o�߂��Ă�΃t���[�����s
				UpdateInput();                  // �t���[���̏��߂ɁA���݂̃L�[�̉�����Ă��Ԃ�S���`�F�b�N����
				Game_Update();                  // �Q�[���X�V
				Game_Render();                  // �Q�[���`��
				lastTime = currentTime;         // �O��t���[�����s���ԂƂ��ĕۑ�
				fpsCnt++;
			}
			// ���@�Q�[���Ɋւ���v���O�����������ꏊ ////////////////////////////////////////////////////////
		}
	}

MAIN_LOOP_END:

	UninitInput();  // �C���v�b�g�n�֐��̏I������
	Game_Release(); // �Q�[���̏I�������i��������j
	DirectX11Release(); // DirectX11�������

	// ���������[�N
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	return 0; // (int)msg.wParam;
}
