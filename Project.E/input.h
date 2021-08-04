//=============================================================================
//
// 入力処理 [input.h]
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
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitInput(HINSTANCE hInstance, HWND hWnd);
void UninitInput(void);
void UpdateInput(void);

bool GetKeyboardPress(int key);  // GetAsyncKeystateとほぼ同じ、プレス状態を取得する
bool GetKeyboardTrigger(int key);  // 現在のフレームでキーが押されたか、その瞬間が分かる
bool GetKeyboardRepeat(int key);  // リピート＝カーソルの動き
bool GetKeyboardRelease(int key);  // リリース＝キーが離された瞬間を取れる

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



//ゲームパッドデバイスの作成-デバイス列挙の結果を受け取る構造体
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


	ButtonState_NO,     //押されてない
	ButtonState_PUSH,   //押された瞬間
	ButtonState_PRESS,  //押されている間
	ButtonState_OUT,    //離された瞬間
	ButtonState_MAX,

};


//=============================================================================
// ゲームパッドのボタン長押し
//=============================================================================
bool IsButtonPress(Button_MAX button);


//=============================================================================
// ゲームパッドのボタンのタップ押し
//=============================================================================
bool IsButtonPush(Button_MAX button);


#endif