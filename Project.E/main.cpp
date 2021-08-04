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
#include <crtdbg.h> // メモリリーク検出用

#pragma comment (lib, "Winmm.lib")


//*****************************************************************************
// グローバル変数
//*****************************************************************************
HWND ghWnd;
int gFps;  // FPS表示用の変数


//=============================================================================
// メイン関数
//  (WINAPI : Win32API関数を呼び出す時の規約)
//  hInstance     : このアプリケーションのインスタンスハンドル(識別子)
//  hPrevInstance : 常にNULL(16bit時代の名残り)
//  lpCmdLine     : コマンドラインから受け取った引数へのポインタ
//  nCmdShow      : アプリケーションの初期ウインドウ表示方法の指定
//=============================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//*****************************************************************************
	// Windows関係の初期化
	// DirectXの初期化
	//*****************************************************************************

	HRESULT hr;
	hr = DirectX11Init(hInstance, nCmdShow);

	DirectX11Manager* manager = GetDirectX11Manager();

	if (FAILED(hr)) {
		goto MAIN_LOOP_END;	
	}


	//*****************************************************************************
	// ゲーム初期化
	//*****************************************************************************

	hr = Game_Initialize();
	if (FAILED(hr)) {
		MessageBox(manager->mhWnd, "[Game_Initialize]に失敗しました。", "エラー発生", MB_OK | MB_ICONERROR);
		goto MAIN_LOOP_END;
	}
	
	// インプット系関数を使えるようにする初期化
	InitInput(hInstance, manager->mhWnd);

	// カーソルを消す
	ShowCursor(FALSE);


	//*****************************************************************************
	// ゲームループ
	//*****************************************************************************
	// メッセージループ
	// 第１引数：メッセージ情報
	// 第２引数：ウィンドウのハンドル
	// 第３引数：最初のメッセージ
	// 第４引数：最後のメッセージ
	MSG msg;
	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			if (msg.message == WM_QUIT) {
				break;
			}
			else {
				// 呼び出し側スレッドのメッセージキューからメッセージを取得
				TranslateMessage(&msg);	// 仮想キーメッセージを文字メッセージへ変換
				DispatchMessage(&msg);	// ウィンドウプロシージャへメッセージを送出
			}
		}
		else {  // メッセージが何も発生していない時
			// ↓　ゲームに関するプログラムを書く場所 ////////////////////////////////////////////////////////
			// 時間を取得する
			static DWORD lastTime = 0;  // 前回フレームが実行された時のミリ秒を入れる変数

			timeBeginPeriod(1);
			DWORD currentTime = timeGetTime(); // Windowsが起動してから現在までの経過時間がミリ秒単位で取れる
			timeEndPeriod(1);

			static DWORD everyLastTime; // 前回のループ時の時間を保存する変数
			static int fpsCnt;          // FPSカウント用変数

			// ミリ秒の1000の位が変わったら１秒経ったということ
			if (currentTime / 1000 > everyLastTime / 1000) {
				gFps = fpsCnt; // 表示用の変数にカウントしたFPSの値を保存
				fpsCnt = 0;    // カウント用変数を０に戻す
			}
			everyLastTime = currentTime;

			if (currentTime >= lastTime + 16) { // 前回フレームが実行されてから1000/60ミリ秒経過したか？ 経過してればフレーム実行
				UpdateInput();                  // フレームの初めに、現在のキーの押されてる状態を全部チェックする
				Game_Update();                  // ゲーム更新
				Game_Render();                  // ゲーム描画
				lastTime = currentTime;         // 前回フレーム実行時間として保存
				fpsCnt++;
			}
			// ↑　ゲームに関するプログラムを書く場所 ////////////////////////////////////////////////////////
		}
	}

MAIN_LOOP_END:

	UninitInput();  // インプット系関数の終了処理
	Game_Release(); // ゲームの終了処理（解放処理）
	DirectX11Release(); // DirectX11解放処理

	// メモリリーク
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	return 0; // (int)msg.wParam;
}
