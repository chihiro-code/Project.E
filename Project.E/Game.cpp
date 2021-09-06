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

	// UI
	UIInit();

	// ゲームオブジェクト
	GameObjectInit();

	// グリッドマップ
	MapInit();

	// プレイヤー
	PlayerInit();

	// シェーダー
	ShaderInit();

	// XAudio
	InitSound();


	return S_OK;
}


//*************************************
// ゲーム更新
//*************************************
void Game_Update(void)
{
	// XAudio更新
	UpdateSound();

	// カメラ
	CameraUpdate();

	// プレイヤー
	PlayerUpdate();

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
	

	//===========================================
	// ↓↓↓　描画処理をここに書く　↓↓↓


	//---------------------------------
	// シャドウマップ作成
	//---------------------------------
	manager->RenderTargetsSetShadow();
	//manager->RenderTargetsSetShadowNoMotion();

	// ゲームオブジェクト
	GameObjectDrawShadow();

	// グリッドマップ
	MapDrawShadow();


	//---------------------------------
	// モデル描画
	//---------------------------------
	manager->RenderTargetsSet();

	// ゲームオブジェクト
	GameObjectDraw();

	// グリッドマップ
	MapDraw();

	// UI
	UIDraw();


	// ↑↑↑　描画処理をここに書く　↑↑↑
	//===========================================

	// ダブル・バッファのディスプレイ領域へのコピー命令
	manager->UpdateScreen();

}


//*************************************
// ゲーム解放
//*************************************
void Game_Release(void)
{
	// XAudio解放
	UninitSound();

	// シェーダー
	ShaderRelease();

	// プレイヤー
	PlayerRelease();

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