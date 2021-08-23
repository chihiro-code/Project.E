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
// グローバル変数
//*************************************


//*************************************
// ゲーム初期化
//*************************************
HRESULT Game_Initialize(void)
{
	HRESULT hr;

	// カメラ
	CameraInit();

	// ToonMap
	ToonMap_Init();

	//ライト
	LightInit();

	// ゲームオブジェクト
	GameObjectInit();

	// グリッドマップ
	MapInit();

	// シェーダー
	ShaderInit();

	return S_OK;
}


//*************************************
// ゲーム更新
//*************************************
void Game_Update(void)
{
	// カメラ
	CameraUpdate();

	// ゲームオブジェクト
	GameObjectUpdate();

	// ライト
	LightUpdate();

}


//*************************************
// ゲーム描画
//*************************************
void Game_Render(void)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	
	// 画面クリア
	manager->ClearScreen();
	manager->RenderTargetsSet();

	//------------------------------------
	// ↓↓↓　描画処理をここに書く　↓↓↓


	// ゲームオブジェクト
	GameObjectDraw();

	// グリッドマップ
	MapDraw();


	// ↑↑↑　描画処理をここに書く　↑↑↑
	//------------------------------------

	// ダブル・バッファのディスプレイ領域へのコピー命令
	manager->UpdateScreen();

}


//*************************************
// ゲーム解放
//*************************************
void Game_Release(void)
{
	// シェーダー
	ShaderRelease();

	// ゲームオブジェクト
	GameObjectRelease();

	// グリッドマップ
	MapRelease();

	// ライト
	LightRelease();

	// ToonMap
	ToonMap_Release();

	// カメラ解放
	CameraRelease();

}