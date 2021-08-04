//=============================================================================
//
// ���͏��� [input.cpp]
//
//=============================================================================
#undef UNICODE
#include "input.h"

//*****************************************************************************
// �萔��`
//*****************************************************************************
const int NUM_KEY_MAX = 256;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd);
void UninitKeyboard(void);
HRESULT UpdateKeyboard(void);
HRESULT InitMouse(HINSTANCE hInst, HWND hWnd);
void UninitMouse(void);
HRESULT UpdateMouse(void);

bool InitGamepad(void);
void UninitGamepad(void);   // �Q�[���p�b�h�f�o�C�X�̍쐬
bool CreateGamePadDevice(); // �Q�[���p�b�h�̍X�V
void UpdateGamePad();


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECTINPUT8			g_pDInput = NULL;					// IDirectInput8�C���^�[�t�F�[�X�ւ̃|�C���^
LPDIRECTINPUTDEVICE8	g_pDIDevKeyboard = NULL;			// IDirectInputDevice8�C���^�[�t�F�[�X�ւ̃|�C���^(�L�[�{�[�h)
LPDIRECTINPUTDEVICE8	g_pDIDevMouse = NULL;				// IDirectInputDevice8�C���^�[�t�F�[�X�ւ̃|�C���^(�}�E�X)
LPDIRECTINPUTDEVICE8	g_pDIDevGamepad = NULL;             // IDirectInputDevice8�C���^�[�t�F�[�X�ւ̃|�C���^(�Q�[���p�b�h)

BYTE					g_keyState[NUM_KEY_MAX];			// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
BYTE					g_keyStateTrigger[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
BYTE					g_keyStateRepeat[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
BYTE					g_keyStateRelease[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
BYTE                    g_ButtonStates[NUM_KEY_MAX];        // �Q�[���p�b�h�̏�Ԃ��󂯎�郏�[�N

int						g_keyStateRepeatCnt[NUM_KEY_MAX];	// �L�[�{�[�h�̃��s�[�g�J�E���^
DIMOUSESTATE2			g_mouseState;						// �}�E�X�̏�Ԃ��󂯎�郏�[�N
DIMOUSESTATE2			g_mouseStateTrigger;				// �}�E�X�̏�Ԃ��󂯎�郏�[�N
POINT					g_MousePoint;						// �}�E�X�̌��ݍ��W
HWND					g_hwnd;

//=============================================================================
// ���͏����̏�����
//=============================================================================
HRESULT InitInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	if(!g_pDInput)
	{
		// DirectInput�I�u�W�F�N�g�̍쐬
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&g_pDInput, NULL);
	}

	// �L�[�{�[�h�̏�����
	InitKeyboard(hInst, hWnd);

	// �}�E�X�̏�����
	InitMouse(hInst, hWnd);

	g_hwnd=hWnd;

	return hr;
}

//=============================================================================
// ���͏����̏I������
//=============================================================================
void UninitInput(void)
{
	// �L�[�{�[�h�̏I������
	UninitKeyboard();

	// �}�E�X�̏I������
	UninitMouse();

	if(g_pDInput)
	{
		g_pDInput->Release();
		g_pDInput = NULL;
	}
}

//=============================================================================
// ���͏����̍X�V����
//=============================================================================
void UpdateInput(void)
{
	// �L�[�{�[�h�̍X�V
	UpdateKeyboard();

	// �}�E�X�̍X�V
	UpdateMouse();
}

//=============================================================================
// �L�[�{�[�h�̏�����
//=============================================================================
HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// �f�o�C�X�I�u�W�F�N�g���쐬
	hr = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDIDevKeyboard, NULL);
	if(FAILED(hr) || g_pDIDevKeyboard == NULL)
	{
		MessageBox(hWnd, "�L�[�{�[�h���˂��I", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr = g_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = g_pDIDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �L�[�{�[�h�ւ̃A�N�Z�X�����l��(���͐���J�n)
	g_pDIDevKeyboard->Acquire();

	return S_OK;
}

//=============================================================================
// �L�[�{�[�h�̏I������
//=============================================================================
void UninitKeyboard(void)
{
	if(g_pDIDevKeyboard)
	{
		g_pDIDevKeyboard->Unacquire();

		g_pDIDevKeyboard->Release();
		g_pDIDevKeyboard = NULL;
	}
}

//=============================================================================
// �L�[�{�[�h�̍X�V����
//=============================================================================
HRESULT UpdateKeyboard(void)
{
	HRESULT hr;
	BYTE keyStateOld[256];

	// �O��̃f�[�^��ۑ�
	memcpy(keyStateOld, g_keyState, NUM_KEY_MAX);

	// �f�o�C�X����f�[�^���擾
	hr = g_pDIDevKeyboard->GetDeviceState(sizeof(g_keyState), g_keyState);
	if(SUCCEEDED(hr))
	{
		for(int cnt = 0; cnt < NUM_KEY_MAX; cnt++)
		{
			// �g���K�[�ƃ����[�X���擾
			g_keyStateTrigger[cnt] = ( g_keyState[cnt] ^ keyStateOld[cnt] )& g_keyState[cnt];
			g_keyStateRelease[cnt] = ( g_keyState[cnt] ^ keyStateOld[cnt] )& keyStateOld[cnt];
			
			// �L�[��������Ă���Ȃ烊�s�[�g�̔��菈��
			if(g_keyState[cnt])
			{
				if(g_keyStateRepeatCnt[cnt] < 20)
				{
					g_keyStateRepeatCnt[cnt]++;
					// �u������́v�������́u�{�^���������Ă���20�t���[���o�߁v
					// �����ꍇ�A���s�[�g�p�z��̃f�[�^��L���ɂ���
					//if(  ||  )
					// ��L�����ȊO�̓��s�[�g�p�z��̃f�[�^�𖳌��ɂ���
					//else
					if((g_keyStateRepeatCnt[cnt]==1)  || g_keyStateRepeatCnt[cnt] >=20){
						g_keyStateRepeat[cnt]=g_keyState[cnt];
					}
					else{
						g_keyStateRepeat[cnt]=0;
					}
				}
			}
			else
			{
				g_keyStateRepeatCnt[cnt] = 0;
				g_keyStateRepeat[cnt] = 0;
			}
		}
	}
	else
	{
		// �L�[�{�[�h�ւ̃A�N�Z�X�����擾
		hr = g_pDIDevKeyboard->Acquire();
	}

	return hr;
}

//=============================================================================
// �L�[�{�[�h�̃v���X��Ԃ��擾
//=============================================================================
bool GetKeyboardPress(int key)
{
	return ((g_keyState[key] & 0x80) != 0);
}

//=============================================================================
// �L�[�{�[�h�̃g���K�[��Ԃ��擾
//=============================================================================
bool GetKeyboardTrigger(int key)
{
	return ((g_keyStateTrigger[key] & 0x80) != 0);
}

//=============================================================================
// �L�[�{�[�h�̃��s�[�g��Ԃ��擾
//=============================================================================
bool GetKeyboardRepeat(int key)
{
	return ((g_keyStateRepeat[key] & 0x80) != 0);
}

//=============================================================================
// �L�[�{�[�h�̃����|�X��Ԃ��擾
//=============================================================================
bool GetKeyboardRelease(int key)
{
	return ((g_keyStateRelease[key] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�̏�����
//=============================================================================
HRESULT InitMouse(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// �f�o�C�X�I�u�W�F�N�g���쐬
	hr = g_pDInput->CreateDevice(GUID_SysMouse, &g_pDIDevMouse, NULL);
	if(FAILED(hr) || g_pDIDevMouse == NULL)
	{
		MessageBox(hWnd, "�}�E�X���˂��I", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr = g_pDIDevMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�}�E�X�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = g_pDIDevMouse->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�}�E�X�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �f�o�C�X�̐ݒ�
	{
		DIPROPDWORD dipdw;

		dipdw.diph.dwSize = sizeof(dipdw);
		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = DIPROPAXISMODE_REL;

		hr = g_pDIDevMouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
	}

	// �L�[�{�[�h�ւ̃A�N�Z�X�����l��(���͐���J�n)
	g_pDIDevMouse->Acquire();

	return S_OK;
}

//=============================================================================
// �}�E�X�̏I������
//=============================================================================
void UninitMouse(void)
{
	if(g_pDIDevMouse)
	{
		g_pDIDevMouse->Unacquire();

		g_pDIDevMouse->Release();
		g_pDIDevMouse = NULL;
	}
}

//=============================================================================
// �}�E�X�̍X�V����
//=============================================================================
HRESULT UpdateMouse(void)
{
	HRESULT hr;
	DIMOUSESTATE2 mouseStateOld;

	GetCursorPos(&g_MousePoint);
	ScreenToClient(g_hwnd, &g_MousePoint);

	// �O��̃f�[�^��ۑ�
	mouseStateOld = g_mouseState;

	// �f�o�C�X����f�[�^���擾
	hr = g_pDIDevMouse->GetDeviceState(sizeof(g_mouseState), &g_mouseState);
	if(SUCCEEDED(hr))
	{
		// �g���K�[�̎擾
		//g_mouseStateTrigger.lX = 
		//g_mouseStateTrigger.lY = 
		//g_mouseStateTrigger.lZ = 
		for(int cnt = 0; cnt < 8; cnt++)
		{
			g_mouseStateTrigger.rgbButtons[cnt] = ((mouseStateOld.rgbButtons[cnt] ^ g_mouseState.rgbButtons[cnt]) & g_mouseState.rgbButtons[cnt]);
		}
	}
	else
	{
		g_pDIDevMouse->Acquire();
	}

	return S_OK;
}

//=============================================================================
// �}�E�X�f�[�^�擾(���v���X)
//=============================================================================
bool GetMouseLeftPress(void)
{
	return ((g_mouseState.rgbButtons[0] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�f�[�^�擾(���g���K�[)
//=============================================================================
bool GetMouseLeftTrigger(void)
{
	return ((g_mouseStateTrigger.rgbButtons[0] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�f�[�^�擾(�E�v���X)
//=============================================================================
bool GetMouseRightPress(void)
{
	return ((g_mouseState.rgbButtons[1] & 0x8) != 00);
}

//=============================================================================
// �}�E�X�f�[�^�擾(�E�g���K�[)
//=============================================================================
bool GetMouseRightTrigger(void)
{
	return ((g_mouseStateTrigger.rgbButtons[1] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�f�[�^�擾(�����v���X)
//=============================================================================
bool GetMouseCenterPress(void)
{
	return ((g_mouseState.rgbButtons[2] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�f�[�^�擾(�����g���K�[)
//=============================================================================
bool GetMouseCenterTrigger(void)
{
	return ((g_mouseState.rgbButtons[2] & 0x80) != 0);
}

//=============================================================================
// �}�E�X�f�[�^�擾(�w���ړ�)
//=============================================================================
long GetMouseAxisX(void)
{
	return g_mouseState.lX;
}

//=============================================================================
// �}�E�X�f�[�^�擾(�x���ړ�)
//=============================================================================
long GetMouseAxisY(void)
{
	return g_mouseState.lY;
}

//=============================================================================
// �}�E�X�f�[�^�擾(�y���ړ�)
//=============================================================================
long GetMouseAxisZ(void)
{
	return g_mouseState.lZ;
}

//=============================================================================
// �}�E�X���W�擾(X)
//=============================================================================
long GetMouseX(void)
{
	return g_MousePoint.x;
}

//=============================================================================
// �}�E�X���W�擾(Y)
//=============================================================================
long GetMouseY(void)
{
	return g_MousePoint.y;
}


//============================================================================
// �Q�[���p�b�h�̏�����
//=============================================================================
BOOL SetUpCooperativeLevel(LPDIRECTINPUTDEVICE8 device)
{
	//�������[�h�̐ݒ�
	if (FAILED(device->SetCooperativeLevel(g_hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
	{
		return false;
	}

	return true;
}

BOOL StartGamePadControl()
{
	// �f�o�C�X����������ĂȂ�
	if (g_pDIDevGamepad == nullptr)
	{
		return false;
	}

	// ����J�n
	if (FAILED(g_pDIDevGamepad->Acquire()))
	{
		return false;
	}

	DIDEVCAPS cap;
	g_pDIDevGamepad->GetCapabilities(&cap);
	// �|�[�����O����
	if (cap.dwFlags & DIDC_POLLEDDATAFORMAT)
	{
		DWORD error = GetLastError();
		// �|�[�����O�J�n
		/*
			Poll��Acquire�̑O�ɍs���Ƃ���Ă������A
			Acquire�̑O�Ŏ��s����Ǝ��s�����̂�
			��Ŏ��s����悤�ɂ���
		*/
		if (FAILED(g_pDIDevGamepad->Poll()))
		{
			return false;
		}
	}

	return true;
}

BOOL SetUpGamePadProperty(LPDIRECTINPUTDEVICE8 device)
{
	// �����[�h���Βl���[�h�Ƃ��Đݒ�
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;
	if (FAILED(device->SetProperty(DIPROP_AXISMODE, &diprop.diph)))
	{
		return false;
	}

	// X���̒l�͈̔͐ݒ�
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = -1000;
	diprg.lMax = 1000;
	if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph)))
	{
		return false;
	}

	// Y���̒l�͈̔͐ݒ�
	diprg.diph.dwObj = DIJOFS_Y;
	if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph)))
	{
		return false;
	}

	return true;
}

BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{

	HRESULT hr;
	DeviceEnumParameter* parameter = (DeviceEnumParameter*)pvRef;
	LPDIRECTINPUTDEVICE8 device = nullptr;

	// ���ɔ������Ă���Ȃ�I��
	if (parameter->FindCount >= 1)
	{
		return DIENUM_STOP;
	}

	// �f�o�C�X����
	hr = g_pDInput->CreateDevice(
		lpddi->guidInstance,
		parameter->GamePadDevice,
		NULL);

	if (FAILED(hr))
	{
		return DIENUM_STOP;
	}

	// ���̓t�H�[�}�b�g�̎w��
	device = *parameter->GamePadDevice;
	hr = device->SetDataFormat(&c_dfDIJoystick);

	if (FAILED(hr))
	{
		return DIENUM_STOP;
	}

	// �v���p�e�B�̐ݒ�
	if (SetUpGamePadProperty(device) == false)
	{
		return DIENUM_STOP;
	}

	// �������x���̐ݒ�
	if (SetUpCooperativeLevel(device) == false)
	{
		return DIENUM_STOP;
	}

	// ���������J�E���g
	parameter->FindCount++;

	return DIENUM_CONTINUE;
}

bool InitGamepad()
{

	// �f�o�C�X�쐬
	if (CreateGamePadDevice() == false)
	{
		UninitGamepad();
		return false;
	}

	// ���͏��̏�����
	for (int i = 0; i < Button_MAX::Buttan_Kind; i++)
	{
		g_ButtonStates[i] = ButtonState::ButtonState_NO;
	}


	return true;
}

//=============================================================================
// �Q�[���p�b�h�̏I������
//=============================================================================
void UninitGamepad()
{
	// �f�o�C�X�̉��
	if (g_pDIDevGamepad != nullptr)
	{
		// ������~
		g_pDIDevGamepad->Unacquire();
		g_pDIDevGamepad->Release();
		g_pDIDevGamepad = nullptr;
	}

	// �C���^�[�t�F�[�X�̉��
	if (g_pDInput != nullptr)
	{
		g_pDInput->Release();
		g_pDInput = nullptr;
	}
}

//=============================================================================
// �Q�[���p�b�h�̃{�^��������
//=============================================================================
bool IsButtonPress(Button_MAX button)
{
	if (g_ButtonStates[button] == ButtonState::ButtonState_PRESS)
	{
		return true;
	}

	return false;
}

//=============================================================================
// �Q�[���p�b�h�̃{�^���̃^�b�v����
//=============================================================================
bool IsButtonPush(Button_MAX button)
{
	if (g_ButtonStates[button] == ButtonState::ButtonState_PUSH)
	{
		return true;
	}

	return false;
}

bool CreateGamePadDevice()
{
	DeviceEnumParameter parameter;

	parameter.FindCount = 0;
	parameter.GamePadDevice = &g_pDIDevGamepad;

	// GAMEPAD�𒲂ׂ�
	g_pDInput->EnumDevices(
		DI8DEVTYPE_GAMEPAD,			// ��������f�o�C�X�̎��
		DeviceFindCallBack,			// �������Ɏ��s����֐�
		&parameter,					// �֐��ɓn���l
		DIEDFL_ATTACHEDONLY			// �������@
	);

	// JOYSTICK�𒲂ׂ�
	/*g_pDInput->EnumDevices(
		DI8DEVTYPE_JOYSTICK,
		DeviceFindCallBack,
		&parameter,
		DIEDFL_ATTACHEDONLY
	);*/

	// �ǂ���������邱�Ƃ��o���Ȃ������玸�s
	if (parameter.FindCount == 0)
	{
		return false;
	}

	int count = 0;
	// ����J�n
	while (StartGamePadControl() == false)
	{
		Sleep(100);
		count++;
		if (count >= 5)
		{
			break;
		}
	}

	return true;
}



//=============================================================================
// �Q�[���p�b�h�̂̍X�V����
//=============================================================================


void UpdateGamePad()
{
	if (g_pDIDevGamepad == nullptr)
	{
		return;
	}

	DIJOYSTATE pad_data;

	// �f�o�C�X�擾
	HRESULT hr = g_pDIDevGamepad->GetDeviceState(sizeof(DIJOYSTATE), &pad_data);
	if (FAILED(hr))
	{
		// �ēx����J�n
		if (FAILED(g_pDIDevGamepad->Acquire()))
		{
			for (int i = 0; i < Button_MAX::Buttan_Kind; i++)
			{
				g_ButtonStates[i] = ButtonState::ButtonState_NO; // �R���g���[���[�̏�Ԃ�������
			}
			g_pDIDevGamepad->Poll();
		}
		return;
	}


	bool is_push[Button_MAX::Buttan_Kind];
	// �X�e�B�b�N����
	int unresponsive_range = 200;
	if (pad_data.lX < -unresponsive_range)
	{
		is_push[Button_MAX::MAX_SL_LEFT] = true;
	}
	else if (pad_data.lX > unresponsive_range)
	{
		is_push[Button_MAX::MAX_SL_RIGHT] = true;
	}

	if (pad_data.lY < -unresponsive_range)
	{
		is_push[Button_MAX::MAX_SL_UP] = true;
	}
	else if (pad_data.lY > unresponsive_range)
	{
		is_push[Button_MAX::MAX_SL_DOWN] = true;
	}


	// �������ɓ|����Ă�����
	if (pad_data.lRx < unresponsive_range && pad_data.lRx < RIGHTSTICK_DEFVAL) {
		is_push[Button_MAX::MAX_SR_LEFT] = true;
		// ������`��//�@//�X�V�̎��͏����͂���Ȃ�
	}

	// �E�����ɓ|����Ă�����
	else if (pad_data.lRx > unresponsive_range && pad_data.lRx > RIGHTSTICK_DEFVAL) {
		is_push[Button_MAX::MAX_SR_RIGHT] = true;
		// ������`��//�@//�X�V�̎��͏����͂���Ȃ�
	}


	// ������ɓ|����Ă�����
	if (pad_data.lRy < -unresponsive_range && pad_data.lRy < RIGHTSTICK_DEFVAL) {
		is_push[Button_MAX::MAX_SR_UP] = true;
		// ������`��//�@//�X�V�̎��͏����͂���Ȃ�
	}

	// �������ɓ|����Ă�����
	else if (pad_data.lRy > unresponsive_range && pad_data.lRy > RIGHTSTICK_DEFVAL) {
		is_push[Button_MAX::MAX_SR_DOWN] = true;
		// ������`��//�@//�X�V�̎��͏����͂���Ȃ�
	}

	// �\���L�[����
	if (pad_data.rgdwPOV[0] != 0xFFFFFFFF)
	{
		float rad = XMConvertToRadians((pad_data.rgdwPOV[0] / 100.0f));
		// �{����x��cos�Ay��sin�����ǁArgdwPOV��0���ォ��n�܂�̂ŁA
		// cos��sin���t�ɂ��������s��������
		float x = sinf(rad);
		float y = cosf(rad);

		if (x < -0.01f)
		{
			is_push[Button_MAX::MAX_BC_LEFT] = true;
		}
		else if (x > 0.01f)
		{
			is_push[Button_MAX::MAX_BC_LEFT] = true;
		}

		if (y > 0.01f)
		{
			is_push[Button_MAX::MAX_BC_UP] = true;
		}
		else if (y < -0.01f)
		{
			is_push[Button_MAX::MAX_SL_DOWN] = true;
		}
	}

	// �{�^������
	for (int i = 0; i < 10; i++)
	{
		if (!(pad_data.rgbButtons[i] & 0x80))
		{
			continue;
		}

		switch (i)
		{

		case Bbatu: // [A]�{�^�� �u�P�ԃ{�^���v
			is_push[Button_MAX::MAX_Bbatu] = true;
			break;

		case Bmaru: // [B]�{�^�� �u�Q�ԃ{�^���v
			is_push[Button_MAX::MAX_Bmaru] = true;
			break;

		case Bshikaku: // [X]�{�^�� �u�R�ԃ{�^���v
			is_push[Button_MAX::MAX_Bshikaku] = true;
			break;

		case Bsankaku: // [Y]�{�^�� �u�S�ԃ{�^���v
			is_push[Button_MAX::MAX_Bsankaku] = true;
			break;

		case BL1: // [�k�a(�k1)]�{�^�� �u�T�ԃ{�^���v
			is_push[Button_MAX::MAX_BL1] = true;
			break;

		case BR1: // [�q�a(�q1)]�{�^�� �u�U�ԃ{�^���v
			is_push[Button_MAX::MAX_BR1] = true;
			break;

		case Bshare: // [SHARA(SHARA)]�{�^�� �u�V�ԃ{�^���v
			is_push[Button_MAX::MAX_Bshare] = true;
			break;

		case Boptions: // [OPTIONS(OPTIONS)]�{�^�� �u�W�ԃ{�^���v
			is_push[Button_MAX::MAX_Boptions] = true;
			break;

		case BL3: // ���X�e�B�b�N�������� �u�X�ԃ{�^���v
			is_push[Button_MAX::MAX_BL3] = true;
			break;

		case BR3: // �E�X�e�B�b�N�������� �u�P�O�ԃ{�^���v
			is_push[Button_MAX::MAX_BR3] = true;
			break;


		}

	}
	// ���͏�񂩂�{�^���̏�Ԃ��X�V����
	for (int i = 0; i < Button_MAX::Buttan_Kind; i++)
	{
		if (is_push[i] == true)
		{
			if (g_ButtonStates[i] == ButtonState::ButtonState_NO)
			{
				g_ButtonStates[i] = ButtonState::ButtonState_PUSH;
			}
			else
			{
				g_ButtonStates[i] = ButtonState::ButtonState_PRESS;
			}
		}
		else
		{
			if (g_ButtonStates[i] == ButtonState::ButtonState_PRESS)
			{
				g_ButtonStates[i] = ButtonState::ButtonState_OUT;
			}
			else
			{
				g_ButtonStates[i] = ButtonState::ButtonState_NO;
			}
		}
	}

}