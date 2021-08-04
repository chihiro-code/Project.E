//=============================================================================
//
// ���͏��� [input.h]
//
//=============================================================================
#ifndef _INPUT_H_
#define _INPUT_H_

#define DIRECTINPUT_VERSION 0x0800
#define RIGHTSTICK_DEFVAL 32767

#include <dinput.h>
#include<DirectXMath.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

using namespace DirectX;
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitInput(HINSTANCE hInstance, HWND hWnd);
void UninitInput(void);
void UpdateInput(void);

bool GetKeyboardPress(int key);  // GetAsyncKeystate�Ƃقړ����A�v���X��Ԃ��擾����
bool GetKeyboardTrigger(int key);  // ���݂̃t���[���ŃL�[�������ꂽ���A���̏u�Ԃ�������
bool GetKeyboardRepeat(int key);  // ���s�[�g���J�[�\���̓���
bool GetKeyboardRelease(int key);  // �����[�X���L�[�������ꂽ�u�Ԃ�����

bool GetMouseLeftPress(void);
bool GetMouseLeftTrigger(void);
bool GetMouseRightPress(void);
bool GetMouseRightTrigger(void);
bool GetMouseCenterPress(void);
bool GetMouseCenterTrigger(void);
long GetMouseAxisX(void);
long GetMouseAxisY(void);
long GetMouseAxisZ(void);
long GetMouseX(void);
long GetMouseY(void);



//�Q�[���p�b�h�f�o�C�X�̍쐬-�f�o�C�X�񋓂̌��ʂ��󂯎��\����
struct DeviceEnumParameter
{
	LPDIRECTINPUTDEVICE8* GamePadDevice;
	int FindCount;
};


enum Button_MAX {

	MAX_Bbatu,
	MAX_Bmaru,
	MAX_Bshikaku,
	MAX_Bsankaku,
	MAX_BL1,
	MAX_BR1,
	MAX_Bshare,
	MAX_Boptions,
	MAX_BL3,
	MAX_BR3,

	MAX_BC_UP,
	MAX_BC_LEFT,
	MAX_BC_RIGHT,
	MAX_BC_DOWN,

	MAX_SL_UP,
	MAX_SL_LEFT,
	MAX_SL_RIGHT,
	MAX_SL_DOWN,

	MAX_SR_UP,
	MAX_SR_LEFT,
	MAX_SR_RIGHT,
	MAX_SR_DOWN,

	Buttan_Kind

};


enum Button {

	Bbatu,
	Bmaru,
	Bshikaku,
	Bsankaku,
	BL1,
	BR1,
	Bshare,
	Boptions,
	BL3,
	BR3

};

enum BCrosskey {

	BC_UP,
	BC_LEFT,
	BC_RIGHT,
	BC_DOWN

};

enum Stick {

	SL_UP,
	SL_LEFT,
	SL_RIGHT,
	SL_DOWN,

	SR_UP,
	SR_LEFT,
	SR_RIGHT,
	SR_DOWN


};



enum ButtonState {


	ButtonState_NO,     //������ĂȂ�
	ButtonState_PUSH,   //�����ꂽ�u��
	ButtonState_PRESS,  //������Ă����
	ButtonState_OUT,    //�����ꂽ�u��
	ButtonState_MAX,

};


//=============================================================================
// �Q�[���p�b�h�̃{�^��������
//=============================================================================
bool IsButtonPress(Button_MAX button);


//=============================================================================
// �Q�[���p�b�h�̃{�^���̃^�b�v����
//=============================================================================
bool IsButtonPush(Button_MAX button);


#endif