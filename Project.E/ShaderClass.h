#pragma once
#include "DirectX11Manager.h"
#include "GameObject.h"


//*************************************
// Shader class
//*************************************
class Shader : public GameObject
{
    //*******************************************
    // �ϐ�
    //*******************************************
public:
    //---------------------------------
    // �ʏ�`��p
    //---------------------------------

    InputLayout  mpInputLayout = nullptr;
    VertexShader mpVertexShader = nullptr;
    PixelShader  mpPixelShader = nullptr;
    
    //�`��p
    // ���[�V��������
    ComPtr<ID3D11Buffer> mpConstantBufferWorld = nullptr;
    ComPtr<ID3D11Buffer> mpConstantBufferAnime = nullptr;
    ComPtr<ID3D11Buffer> mpConstantBufferLight = nullptr;
    ComPtr<ID3D11RasterizerState> mpRasterState = nullptr;
   

    //---------------------------------
    // �e�`��p
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
    // �֐�
    //*******************************************
public:
    //---------------------------------
    // �ʏ�`��p
    //---------------------------------
    
    // �V�F�[�_�[�������i���[�V��������j
    void ShaderInit();

    // �V�F�[�_�[�������i���[�V�����Ȃ��j
    void ShaderInitNoMotion();


    //---------------------------------
    // �e�`��p
    //---------------------------------

    // �V�F�[�_�[�������i�e�j
    void ShaderInitNoMotionShadow();

};


//*************************************
// �֐�
//*************************************

// �Q�b�g�֐�
Shader* GetShader();
Shader* GetShaderNoMotion();

// �V�F�[�_�[������
void ShaderInit();

// �V�F�[�_�[���
void ShaderRelease();
