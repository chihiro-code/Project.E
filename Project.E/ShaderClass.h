#pragma once
#include "DirectX11Manager.h"
#include "GameObject.h"


//*************************************
// Shader class
//*************************************
class Shader : public GameObject
{
    //*******************************************
    // 変数
    //*******************************************
public:
    //---------------------------------
    // 通常描画用
    //---------------------------------

    InputLayout  mpInputLayout = nullptr;
    VertexShader mpVertexShader = nullptr;
    PixelShader  mpPixelShader = nullptr;
    
    //描画用
    // モーションあり
    ComPtr<ID3D11Buffer> mpConstantBufferWorld = nullptr;
    ComPtr<ID3D11Buffer> mpConstantBufferAnime = nullptr;
    ComPtr<ID3D11Buffer> mpConstantBufferLight = nullptr;
    ComPtr<ID3D11RasterizerState> mpRasterState = nullptr;
   

    //---------------------------------
    // 影描画用
    //---------------------------------
    InputLayout                      mpShadowInputLayout = nullptr;
    VertexShader                     mpShadowVertexShader = nullptr;
    PixelShader                      mpShadowPixelShader = nullptr;
  
    ComPtr<ID3D11Buffer>             mpShadowConstantBuffer = nullptr;
    ComPtr<ID3D11RasterizerState>    mpShadowRasterState = nullptr;
    ComPtr<ID3D11Texture2D>          mpShadowDepthStencilTexture = nullptr;
    ComPtr<ID3D11SamplerState>       mpShadowSampler = nullptr;
    ComPtr<ID3D11DepthStencilView>   mpShadowDepthStencilView = nullptr;
    ComPtr<ID3D11ShaderResourceView> mpShadowShaderResourceView = nullptr;
    D3D11_VIEWPORT                   mShadowViewport;


    //*******************************************
    // 関数
    //*******************************************
public:
    //---------------------------------
    // 通常描画用
    //---------------------------------
    
    // シェーダー初期化（モーションあり）
    void ShaderInit();

    // シェーダー初期化（モーションなし）
    void ShaderInitNoMotion();


    //---------------------------------
    // 影描画用
    //---------------------------------

    // シェーダー初期化（影）
    void ShaderInitNoMotionShadow();

};


//*************************************
// 関数
//*************************************

// ゲット関数
Shader* GetShader();
Shader* GetShaderNoMotion();

// シェーダー初期化
void ShaderInit();

// シェーダー解放
void ShaderRelease();
