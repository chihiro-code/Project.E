#pragma once


//WindowsApplicationを使用するので必須
#include <windows.h>
//DirectX11のAPIが入っている
#include <d3d11.h>
//シェーダーのコンパイラ
#include <d3dcompiler.h>
//ベクトルやFLOAT3など便利な算術ライブラリ
#include <DirectXMath.h>
//デバイスの管理をしやすくするためのDirectX版のスマートポインタのようなもの
#include <wrl/client.h>

//DirectXAPI関係のリンク
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// unique_ptrスマートポインタ使用のため
#include <memory>  

//DirectXMathがDirectXのネームスペースにある
using namespace DirectX;
//ComPtr(DirectX版のスマートポインタ)
using Microsoft::WRL::ComPtr;
// unique_ptrスマートポインタ
using std::unique_ptr;

//書きやすいようにtypedef色々
typedef ComPtr<ID3D11Buffer> /*ConstantBuffer,*/ VertexBuffer, IndexBuffer, StructuredBuffer;
typedef ComPtr<ID3D11InputLayout>         InputLayout;
typedef ComPtr<ID3D11VertexShader>        VertexShader;
typedef ComPtr<ID3D11PixelShader>         PixelShader;
typedef ComPtr<ID3D11GeometryShader>      GeometryShader;
typedef ComPtr<ID3D11ComputeShader>       ComputeShader;
typedef ComPtr<ID3D11Texture2D>           Texture2D;
typedef ComPtr<ID3D11ShaderResourceView>  ShaderTexture;
typedef ComPtr<ID3D11UnorderedAccessView> ComputeOutputView;
typedef ComPtr<ID3D11RasterizerState>     RasterizerState;


//*************************************
// DirectX11Manager class
//*************************************
class DirectX11Manager
{
public: // 変数

    //Windowsのハンドル
    HWND mhWnd = NULL;
    //DX11のデバイス
    ComPtr<ID3D11Device>            mpDevice = nullptr;
    //DX11の描画命令等を送るためのもの
    ComPtr<ID3D11DeviceContext>     mpImContext = nullptr;
    //ハードウェアの情報が詰まっているもの
    ComPtr<IDXGISwapChain>          mpSwapChain = nullptr;
    //ディスプレイのバッグバッファのテクスチャ
    Texture2D                       mpRTTex = nullptr;
    //ディスプレイのバッグバッファのテクスチャを描画先として指定できるようにしたもの
    ComPtr<ID3D11RenderTargetView>  mpRTView = nullptr;
    //ウィンドウのサイズの指定
    D3D11_VIEWPORT                  mViewport = { 0,0,0,0,0,0 };
    //デプスステンシルビューのテクスチャ
    ComPtr<ID3D11Texture2D>         mpDSTex  = nullptr;
    //デプスステンシルビュー
    ComPtr<ID3D11DepthStencilView>  mpDSView = nullptr;
    // サンプラー
    ComPtr<ID3D11SamplerState>      mpSampler = nullptr;

    


private: // 変数

    // 画面サイズ
    UINT mScrWidth  = 0;
    UINT mScrHeight = 0;


public: // 関数

    // Windows & DirectX11 初期化
    HRESULT Init(HINSTANCE hInstance, int cCmdShow);
    // 画面クリア
    void ClearScreen();
    // RenderTargets設定
    void RenderTargetsSet();
    // RenderTargets設定 Zバッファ無効
    void RenderTargetsSetDisableZbuffer();
    // RenderTargets設定（影　モーションあり）
    void RenderTargetsSetShadow();
    // RenderTargets設定（影　モーションなし）
    //void RenderTargetsSetShadowNoMotion();
    // ダブル・バッファのディスプレイ領域へのコピー命令
    void UpdateScreen();
    // 画面サイズ取得（幅）
    UINT GetScreenWidth();
    // 画面サイズ取得（高さ）
    UINT GetScreenHeight();

};


//*************************************
// 関数
//*************************************

// ゲット関数
DirectX11Manager* GetDirectX11Manager(void);
// 初期化
HRESULT DirectX11Init(HINSTANCE hInstance, int nCmdShow);
// 解放
void DirectX11Release(void);



#define SAFE_RELEASE(p){if(p){(p)->Release();(p)=NULL;}}