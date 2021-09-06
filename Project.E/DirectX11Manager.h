#pragma once


//WindowsApplication���g�p����̂ŕK�{
#include <windows.h>
//DirectX11��API�������Ă���
#include <d3d11.h>
//�V�F�[�_�[�̃R���p�C��
#include <d3dcompiler.h>
//�x�N�g����FLOAT3�ȂǕ֗��ȎZ�p���C�u����
#include <DirectXMath.h>
//�f�o�C�X�̊Ǘ������₷�����邽�߂�DirectX�ł̃X�}�[�g�|�C���^�̂悤�Ȃ���
#include <wrl/client.h>

//DirectXAPI�֌W�̃����N
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// unique_ptr�X�}�[�g�|�C���^�g�p�̂���
#include <memory>  

//DirectXMath��DirectX�̃l�[���X�y�[�X�ɂ���
using namespace DirectX;
//ComPtr(DirectX�ł̃X�}�[�g�|�C���^)
using Microsoft::WRL::ComPtr;
// unique_ptr�X�}�[�g�|�C���^
using std::unique_ptr;

//�����₷���悤��typedef�F�X
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
public: // �ϐ�

    //Windows�̃n���h��
    HWND mhWnd = NULL;
    //DX11�̃f�o�C�X
    ComPtr<ID3D11Device>            mpDevice = nullptr;
    //DX11�̕`�施�ߓ��𑗂邽�߂̂���
    ComPtr<ID3D11DeviceContext>     mpImContext = nullptr;
    //�n�[�h�E�F�A�̏�񂪋l�܂��Ă������
    ComPtr<IDXGISwapChain>          mpSwapChain = nullptr;
    //�f�B�X�v���C�̃o�b�O�o�b�t�@�̃e�N�X�`��
    Texture2D                       mpRTTex = nullptr;
    //�f�B�X�v���C�̃o�b�O�o�b�t�@�̃e�N�X�`����`���Ƃ��Ďw��ł���悤�ɂ�������
    ComPtr<ID3D11RenderTargetView>  mpRTView = nullptr;
    //�E�B���h�E�̃T�C�Y�̎w��
    D3D11_VIEWPORT                  mViewport = { 0,0,0,0,0,0 };
    //�f�v�X�X�e���V���r���[�̃e�N�X�`��
    ComPtr<ID3D11Texture2D>         mpDSTex  = nullptr;
    //�f�v�X�X�e���V���r���[
    ComPtr<ID3D11DepthStencilView>  mpDSView = nullptr;
    // �T���v���[
    ComPtr<ID3D11SamplerState>      mpSampler = nullptr;

    


private: // �ϐ�

    // ��ʃT�C�Y
    UINT mScrWidth  = 0;
    UINT mScrHeight = 0;


public: // �֐�

    // Windows & DirectX11 ������
    HRESULT Init(HINSTANCE hInstance, int cCmdShow);
    // ��ʃN���A
    void ClearScreen();
    // RenderTargets�ݒ�
    void RenderTargetsSet();
    // RenderTargets�ݒ� Z�o�b�t�@����
    void RenderTargetsSetDisableZbuffer();
    // RenderTargets�ݒ�i�e�@���[�V��������j
    void RenderTargetsSetShadow();
    // RenderTargets�ݒ�i�e�@���[�V�����Ȃ��j
    //void RenderTargetsSetShadowNoMotion();
    // �_�u���E�o�b�t�@�̃f�B�X�v���C�̈�ւ̃R�s�[����
    void UpdateScreen();
    // ��ʃT�C�Y�擾�i���j
    UINT GetScreenWidth();
    // ��ʃT�C�Y�擾�i�����j
    UINT GetScreenHeight();

};


//*************************************
// �֐�
//*************************************

// �Q�b�g�֐�
DirectX11Manager* GetDirectX11Manager(void);
// ������
HRESULT DirectX11Init(HINSTANCE hInstance, int nCmdShow);
// ���
void DirectX11Release(void);



#define SAFE_RELEASE(p){if(p){(p)->Release();(p)=NULL;}}