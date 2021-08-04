//=============================================================================
//
// 入力処理 [input.cpp]
//
//=============================================================================
#undef UNICODE
#include "input.h"

//*****************************************************************************
// 定数定義
//*****************************************************************************
const int NUM_KEY_MAX = 256;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd);
void UninitKeyboard(void);
HRESULT UpdateKeyboard(void);
HRESULT InitMouse(HINSTANCE hInst, HWND hWnd);
void UninitMouse(void);
HRESULT UpdateMouse(void);

bool InitGamepad(void);
void UninitGamepad(void);   // ゲームパッドデバイスの作成
bool CreateGamePadDevice(); // ゲームパッドの更新
void UpdateGamePad();


//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECTINPUT8			g_pDInput = NULL;					// IDirectInput8インターフェースへのポインタ
LPDIRECTINPUTDEVICE8	g_pDIDevKeyboard = NULL;			// IDirectInputDevice8インターフェースへのポインタ(キーボード)
LPDIRECTINPUTDEVICE8	g_pDIDevMouse = NULL;				// IDirectInputDevice8インターフェースへのポインタ(マウス)
LPDIRECTINPUTDEVICE8	g_pDIDevGamepad = NULL;             // IDirectInputDevice8インターフェースへのポインタ(ゲームパッド)

BYTE					g_keyState[NUM_KEY_MAX];			// キーボードの状態を受け取るワーク
BYTE					g_keyStateTrigger[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
BYTE					g_keyStateRepeat[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
BYTE					g_keyStateRelease[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
BYTE                    g_ButtonStates[NUM_KEY_MAX];        // ゲームパッドの状態を受け取るワーク

int						g_keyStateRepeatCnt[NUM_KEY_MAX];	// キーボードのリピートカウンタ
DIMOUSESTATE2			g_mouseState;						// マウスの状態を受け取るワーク
DIMOUSESTATE2			g_mouseStateTrigger;				// マウスの状態を受け取るワーク
POINT					g_MousePoint;						// マウスの現在座標
HWND					g_hwnd;

//=============================================================================
// 入力処理の初期化
//=============================================================================
HRESULT InitInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	if(!g_pDInput)
	{
		// DirectInputオブジェクトの作成
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&g_pDInput, NULL);
	}

	// キーボードの初期化
	InitKeyboard(hInst, hWnd);

	// マウスの初期化
	InitMouse(hInst, hWnd);

	g_hwnd=hWnd;

	return hr;
}

//=============================================================================
// 入力処理の終了処理
//=============================================================================
void UninitInput(void)
{
	// キーボードの終了処理
	UninitKeyboard();

	// マウスの終了処理
	UninitMouse();

	if(g_pDInput)
	{
		g_pDInput->Release();
		g_pDInput = NULL;
	}
}

//=============================================================================
// 入力処理の更新処理
//=============================================================================
void UpdateInput(void)
{
	// キーボードの更新
	UpdateKeyboard();

	// マウスの更新
	UpdateMouse();
}

//=============================================================================
// キーボードの初期化
//=============================================================================
HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// デバイスオブジェクトを作成
	hr = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDIDevKeyboard, NULL);
	if(FAILED(hr) || g_pDIDevKeyboard == NULL)
	{
		MessageBox(hWnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = g_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = g_pDIDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	g_pDIDevKeyboard->Acquire();

	return S_OK;
}

//=============================================================================
// キーボードの終了処理
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
// キーボードの更新処理
//=============================================================================
HRESULT UpdateKeyboard(void)
{
	HRESULT hr;
	BYTE keyStateOld[256];

	// 前回のデータを保存
	memcpy(keyStateOld, g_keyState, NUM_KEY_MAX);

	// デバイスからデータを取得
	hr = g_pDIDevKeyboard->GetDeviceState(sizeof(g_keyState), g_keyState);
	if(SUCCEEDED(hr))
	{
		for(int cnt = 0; cnt < NUM_KEY_MAX; cnt++)
		{
			// トリガーとリリースを取得
			g_keyStateTrigger[cnt] = ( g_keyState[cnt] ^ keyStateOld[cnt] )& g_keyState[cnt];
			g_keyStateRelease[cnt] = ( g_keyState[cnt] ^ keyStateOld[cnt] )& keyStateOld[cnt];
			
			// キーが押されているならリピートの判定処理
			if(g_keyState[cnt])
			{
				if(g_keyStateRepeatCnt[cnt] < 20)
				{
					g_keyStateRepeatCnt[cnt]++;
					// 「初回入力」もしくは「ボタンを押してから20フレーム経過」
					// した場合、リピート用配列のデータを有効にする
					//if(  ||  )
					// 上記条件以外はリピート用配列のデータを無効にする
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
		// キーボードへのアクセス権を取得
		hr = g_pDIDevKeyboard->Acquire();
	}

	return hr;
}

//=============================================================================
// キーボードのプレス状態を取得
//=============================================================================
bool GetKeyboardPress(int key)
{
	return ((g_keyState[key] & 0x80) != 0);
}

//=============================================================================
// キーボードのトリガー状態を取得
//=============================================================================
bool GetKeyboardTrigger(int key)
{
	return ((g_keyStateTrigger[key] & 0x80) != 0);
}

//=============================================================================
// キーボードのリピート状態を取得
//=============================================================================
bool GetKeyboardRepeat(int key)
{
	return ((g_keyStateRepeat[key] & 0x80) != 0);
}

//=============================================================================
// キーボードのリリ－ス状態を取得
//=============================================================================
bool GetKeyboardRelease(int key)
{
	return ((g_keyStateRelease[key] & 0x80) != 0);
}

//=============================================================================
// マウスの初期化
//=============================================================================
HRESULT InitMouse(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// デバイスオブジェクトを作成
	hr = g_pDInput->CreateDevice(GUID_SysMouse, &g_pDIDevMouse, NULL);
	if(FAILED(hr) || g_pDIDevMouse == NULL)
	{
		MessageBox(hWnd, "マウスがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = g_pDIDevMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "マウスのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = g_pDIDevMouse->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "マウスの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// デバイスの設定
	{
		DIPROPDWORD dipdw;

		dipdw.diph.dwSize = sizeof(dipdw);
		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = DIPROPAXISMODE_REL;

		hr = g_pDIDevMouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	g_pDIDevMouse->Acquire();

	return S_OK;
}

//=============================================================================
// マウスの終了処理
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
// マウスの更新処理
//=============================================================================
HRESULT UpdateMouse(void)
{
	HRESULT hr;
	DIMOUSESTATE2 mouseStateOld;

	GetCursorPos(&g_MousePoint);
	ScreenToClient(g_hwnd, &g_MousePoint);

	// 前回のデータを保存
	mouseStateOld = g_mouseState;

	// デバイスからデータを取得
	hr = g_pDIDevMouse->GetDeviceState(sizeof(g_mouseState), &g_mouseState);
	if(SUCCEEDED(hr))
	{
		// トリガーの取得
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
// マウスデータ取得(左プレス)
//=============================================================================
bool GetMouseLeftPress(void)
{
	return ((g_mouseState.rgbButtons[0] & 0x80) != 0);
}

//=============================================================================
// マウスデータ取得(左トリガー)
//=============================================================================
bool GetMouseLeftTrigger(void)
{
	return ((g_mouseStateTrigger.rgbButtons[0] & 0x80) != 0);
}

//=============================================================================
// マウスデータ取得(右プレス)
//=============================================================================
bool GetMouseRightPress(void)
{
	return ((g_mouseState.rgbButtons[1] & 0x8) != 00);
}

//=============================================================================
// マウスデータ取得(右トリガー)
//=============================================================================
bool GetMouseRightTrigger(void)
{
	return ((g_mouseStateTrigger.rgbButtons[1] & 0x80) != 0);
}

//=============================================================================
// マウスデータ取得(中央プレス)
//=============================================================================
bool GetMouseCenterPress(void)
{
	return ((g_mouseState.rgbButtons[2] & 0x80) != 0);
}

//=============================================================================
// マウスデータ取得(中央トリガー)
//=============================================================================
bool GetMouseCenterTrigger(void)
{
	return ((g_mouseState.rgbButtons[2] & 0x80) != 0);
}

//=============================================================================
// マウスデータ取得(Ｘ軸移動)
//=============================================================================
long GetMouseAxisX(void)
{
	return g_mouseState.lX;
}

//=============================================================================
// マウスデータ取得(Ｙ軸移動)
//=============================================================================
long GetMouseAxisY(void)
{
	return g_mouseState.lY;
}

//=============================================================================
// マウスデータ取得(Ｚ軸移動)
//=============================================================================
long GetMouseAxisZ(void)
{
	return g_mouseState.lZ;
}

//=============================================================================
// マウス座標取得(X)
//=============================================================================
long GetMouseX(void)
{
	return g_MousePoint.x;
}

//=============================================================================
// マウス座標取得(Y)
//=============================================================================
long GetMouseY(void)
{
	return g_MousePoint.y;
}


//============================================================================
// ゲームパッドの初期化
//=============================================================================
BOOL SetUpCooperativeLevel(LPDIRECTINPUTDEVICE8 device)
{
	//協調モードの設定
	if (FAILED(device->SetCooperativeLevel(g_hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
	{
		return false;
	}

	return true;
}

BOOL StartGamePadControl()
{
	// デバイスが生成されてない
	if (g_pDIDevGamepad == nullptr)
	{
		return false;
	}

	// 制御開始
	if (FAILED(g_pDIDevGamepad->Acquire()))
	{
		return false;
	}

	DIDEVCAPS cap;
	g_pDIDevGamepad->GetCapabilities(&cap);
	// ポーリング判定
	if (cap.dwFlags & DIDC_POLLEDDATAFORMAT)
	{
		DWORD error = GetLastError();
		// ポーリング開始
		/*
			PollはAcquireの前に行うとされていたが、
			Acquireの前で実行すると失敗したので
			後で実行するようにした
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
	// 軸モードを絶対値モードとして設定
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

	// X軸の値の範囲設定
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

	// Y軸の値の範囲設定
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

	// 既に発見しているなら終了
	if (parameter->FindCount >= 1)
	{
		return DIENUM_STOP;
	}

	// デバイス生成
	hr = g_pDInput->CreateDevice(
		lpddi->guidInstance,
		parameter->GamePadDevice,
		NULL);

	if (FAILED(hr))
	{
		return DIENUM_STOP;
	}

	// 入力フォーマットの指定
	device = *parameter->GamePadDevice;
	hr = device->SetDataFormat(&c_dfDIJoystick);

	if (FAILED(hr))
	{
		return DIENUM_STOP;
	}

	// プロパティの設定
	if (SetUpGamePadProperty(device) == false)
	{
		return DIENUM_STOP;
	}

	// 協調レベルの設定
	if (SetUpCooperativeLevel(device) == false)
	{
		return DIENUM_STOP;
	}

	// 発見数をカウント
	parameter->FindCount++;

	return DIENUM_CONTINUE;
}

bool InitGamepad()
{

	// デバイス作成
	if (CreateGamePadDevice() == false)
	{
		UninitGamepad();
		return false;
	}

	// 入力情報の初期化
	for (int i = 0; i < Button_MAX::Buttan_Kind; i++)
	{
		g_ButtonStates[i] = ButtonState::ButtonState_NO;
	}


	return true;
}

//=============================================================================
// ゲームパッドの終了処理
//=============================================================================
void UninitGamepad()
{
	// デバイスの解放
	if (g_pDIDevGamepad != nullptr)
	{
		// 制御を停止
		g_pDIDevGamepad->Unacquire();
		g_pDIDevGamepad->Release();
		g_pDIDevGamepad = nullptr;
	}

	// インターフェースの解放
	if (g_pDInput != nullptr)
	{
		g_pDInput->Release();
		g_pDInput = nullptr;
	}
}

//=============================================================================
// ゲームパッドのボタン長押し
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
// ゲームパッドのボタンのタップ押し
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

	// GAMEPADを調べる
	g_pDInput->EnumDevices(
		DI8DEVTYPE_GAMEPAD,			// 検索するデバイスの種類
		DeviceFindCallBack,			// 発見時に実行する関数
		&parameter,					// 関数に渡す値
		DIEDFL_ATTACHEDONLY			// 検索方法
	);

	// JOYSTICKを調べる
	/*g_pDInput->EnumDevices(
		DI8DEVTYPE_JOYSTICK,
		DeviceFindCallBack,
		&parameter,
		DIEDFL_ATTACHEDONLY
	);*/

	// どちらも見つけることが出来なかったら失敗
	if (parameter.FindCount == 0)
	{
		return false;
	}

	int count = 0;
	// 制御開始
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
// ゲームパッドのの更新処理
//=============================================================================


void UpdateGamePad()
{
	if (g_pDIDevGamepad == nullptr)
	{
		return;
	}

	DIJOYSTATE pad_data;

	// デバイス取得
	HRESULT hr = g_pDIDevGamepad->GetDeviceState(sizeof(DIJOYSTATE), &pad_data);
	if (FAILED(hr))
	{
		// 再度制御開始
		if (FAILED(g_pDIDevGamepad->Acquire()))
		{
			for (int i = 0; i < Button_MAX::Buttan_Kind; i++)
			{
				g_ButtonStates[i] = ButtonState::ButtonState_NO; // コントローラーの状態を初期化
			}
			g_pDIDevGamepad->Poll();
		}
		return;
	}


	bool is_push[Button_MAX::Buttan_Kind];
	// スティック判定
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


	// 左方向に倒されていた時
	if (pad_data.lRx < unresponsive_range && pad_data.lRx < RIGHTSTICK_DEFVAL) {
		is_push[Button_MAX::MAX_SR_LEFT] = true;
		// 処理を描く//　//更新の時は処理はいらない
	}

	// 右方向に倒されていた時
	else if (pad_data.lRx > unresponsive_range && pad_data.lRx > RIGHTSTICK_DEFVAL) {
		is_push[Button_MAX::MAX_SR_RIGHT] = true;
		// 処理を描く//　//更新の時は処理はいらない
	}


	// 上方向に倒されていた時
	if (pad_data.lRy < -unresponsive_range && pad_data.lRy < RIGHTSTICK_DEFVAL) {
		is_push[Button_MAX::MAX_SR_UP] = true;
		// 処理を描く//　//更新の時は処理はいらない
	}

	// 下方向に倒されていた時
	else if (pad_data.lRy > unresponsive_range && pad_data.lRy > RIGHTSTICK_DEFVAL) {
		is_push[Button_MAX::MAX_SR_DOWN] = true;
		// 処理を描く//　//更新の時は処理はいらない
	}

	// 十字キー判定
	if (pad_data.rgdwPOV[0] != 0xFFFFFFFF)
	{
		float rad = XMConvertToRadians((pad_data.rgdwPOV[0] / 100.0f));
		// 本来はxがcos、yがsinだけど、rgdwPOVは0が上から始まるので、
		// cosとsinを逆にした方が都合がいい
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

	// ボタン判定
	for (int i = 0; i < 10; i++)
	{
		if (!(pad_data.rgbButtons[i] & 0x80))
		{
			continue;
		}

		switch (i)
		{

		case Bbatu: // [A]ボタン 「１番ボタン」
			is_push[Button_MAX::MAX_Bbatu] = true;
			break;

		case Bmaru: // [B]ボタン 「２番ボタン」
			is_push[Button_MAX::MAX_Bmaru] = true;
			break;

		case Bshikaku: // [X]ボタン 「３番ボタン」
			is_push[Button_MAX::MAX_Bshikaku] = true;
			break;

		case Bsankaku: // [Y]ボタン 「４番ボタン」
			is_push[Button_MAX::MAX_Bsankaku] = true;
			break;

		case BL1: // [ＬＢ(Ｌ1)]ボタン 「５番ボタン」
			is_push[Button_MAX::MAX_BL1] = true;
			break;

		case BR1: // [ＲＢ(Ｒ1)]ボタン 「６番ボタン」
			is_push[Button_MAX::MAX_BR1] = true;
			break;

		case Bshare: // [SHARA(SHARA)]ボタン 「７番ボタン」
			is_push[Button_MAX::MAX_Bshare] = true;
			break;

		case Boptions: // [OPTIONS(OPTIONS)]ボタン 「８番ボタン」
			is_push[Button_MAX::MAX_Boptions] = true;
			break;

		case BL3: // 左スティック押し込み 「９番ボタン」
			is_push[Button_MAX::MAX_BL3] = true;
			break;

		case BR3: // 右スティック押し込み 「１０番ボタン」
			is_push[Button_MAX::MAX_BR3] = true;
			break;


		}

	}
	// 入力情報からボタンの状態を更新する
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