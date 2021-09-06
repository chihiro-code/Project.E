#include <iostream>
#include "DirectX11Manager.h"
#include "ShaderClass.h"

using namespace std;


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	CLASS_NAME		"�T���v��"				// �E�C���h�E�N���X�̖��O
#define WINDOW_NAME		"Project.E"	            // �E�B���h�E�̖��O

#define	SCREEN_WIDTH	(1920)					// �E�C���h�E�̕�
#define	SCREEN_HEIGHT	(1080)					// �E�C���h�E�̍���

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//*****************************************************************************
// �O���[�o���ϐ��錾
//*****************************************************************************
DirectX11Manager* manager;


//*************************************
// DirectX11Manager�֐�
//*************************************

HRESULT DirectX11Manager::Init(HINSTANCE hInstance, int nCmdShow)
{

    //*****************************************************************************
    // Windows�֌W�̏�����
    //*****************************************************************************
    TCHAR ClassName[]  = TEXT(CLASS_NAME);
    TCHAR WindowName[] = TEXT(WINDOW_NAME);
    mScrWidth  = SCREEN_WIDTH;
    mScrHeight = SCREEN_HEIGHT;

    //Windows�E�B���h�E�̍쐬
    WNDCLASSEX wcex =
    {
        wcex.cbSize        = sizeof(WNDCLASSEX),			// WNDCLASSEX�̃������T�C�Y���w��
        wcex.style         = CS_CLASSDC,					// �\������E�C���h�E�̃X�^�C����ݒ�
        wcex.lpfnWndProc   = WndProc,						// �E�B���h�E�v���V�[�W���̃A�h���X(�֐���)���w��
        wcex.cbClsExtra    = 0,								// �ʏ�͎g�p���Ȃ��̂�"0"���w��
        wcex.cbWndExtra    = 0,								// �ʏ�͎g�p���Ȃ��̂�"0"���w��
        wcex.hInstance     = hInstance,						// WinMain�̃p�����[�^�̃C���X�^���X�n���h����ݒ�
        wcex.hIcon         = NULL,							// �g�p����A�C�R�����w��(Windows�������Ă���A�C�R�����g���Ȃ�NULL)
        wcex.hCursor       = LoadCursor(NULL, IDC_ARROW),	// �}�E�X�J�[�\�����w��
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),	// �E�C���h�E�̃N���C�A���g�̈�̔w�i�F��ݒ�
        wcex.lpszMenuName  = NULL,							// Window�ɂ��郁�j���[��ݒ�
        wcex.lpszClassName = ClassName,					    // �E�C���h�E�N���X�̖��O
        wcex.hIconSm       = NULL							// �������A�C�R�����ݒ肳�ꂽ�ꍇ�̏����L�q
    };
    // �E�B���h�E�N���X�̓o�^
    if (!RegisterClassEx(&wcex)) {
        return E_FAIL;
    }

    // �E�C���h�E���쐬
    mhWnd = CreateWindowEx(0,         // �g���E�B���h�E�X�^�C��
        ClassName,				     // �E�B���h�E�N���X�̖��O
        WindowName,                  // �A�v���P�[�V�����̖��O
        WS_POPUP,	                 // �E�B���h�E�X�^�C��    �O��(WS_OVERLAPPEDWINDOW) �S���(POPUP)
        CW_USEDEFAULT,			     // �E�B���h�E�̍���w���W
        CW_USEDEFAULT,			     // �E�B���h�E�̍���x���W 
        SCREEN_WIDTH,			     // �E�B���h�E�̕�
        SCREEN_HEIGHT,			     // �E�B���h�E�̍���
        NULL,					     // �e�E�B���h�E�̃n���h��
        NULL,					     // ���j���[�n���h���܂��͎q�E�B���h�EID
        hInstance,				     // �C���X�^���X�n���h��
        NULL);					     // �E�B���h�E�쐬�f�[�^

    // �E�C���h�E�̕\��
    ShowWindow(mhWnd, nCmdShow);
    UpdateWindow(mhWnd);

    //Windows�E�B���h�E�̍쐬�����܂�


    //*****************************************************************************
    // �n�[�h�E�F�A�̃`�F�b�N
    //*****************************************************************************
#pragma region HardWare Check
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes = 0;
    size_t stringLength;
    DXGI_ADAPTER_DESC adapterDesc;

    //�O���t�B�b�N �C���^�t�F�[�X �t�@�N�g�����쐬
    auto hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(hr))
    {
        return hr;
    }

    int GPUNumber = 0;
    int GPUMaxMem = 0;
    //��ԋ���GPU�A�_�v�^������
    for (int i = 0; i < 100; i++)
    {
        IDXGIAdapter* add;
        hr = factory->EnumAdapters(i, &add);
        if (FAILED(hr))
            break;
        hr = add->GetDesc(&adapterDesc);

        char videoCardDescription[128];
        //�r�f�I�J�[�h�����擾
        int error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
        if (error != 0)
        {
            break;
        }
        cout << "�r�f�I�J�[�h�� : " << videoCardDescription << endl;

        //�r�f�I�J�[�h���������擾�iMB�P�ʁj
        int videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
        cout << "�r�f�I�������[ : " << videoCardMemory << endl;

        //�A�E�g�v�b�g�i���j�^�[�j�ɔԍ�ID��t����
        hr = add->EnumOutputs(0, &adapterOutput);
        if (FAILED(hr))
        {
            continue;
        }

        //DXGI_FORMAT_R8G8B8A8_UNORM �̕\���`�������擾����
        hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
        if (FAILED(hr))
        {
            continue;
        }
        cout << "RBGA8_UNORM Count : " << numModes << endl;

        if (videoCardMemory > GPUMaxMem)
        {
            GPUMaxMem = videoCardMemory;
            GPUNumber = i;
        }
        add->Release();
        //�A�_�v�^�A�E�g�v�b�g�����
        adapterOutput->Release();
        adapterOutput = 0;
    }

    //�O���t�B�b�N �C���^�t�F�[�X �A�_�v�^�[���쐬
    hr = factory->EnumAdapters(GPUNumber, &adapter);
    if (FAILED(hr))
    {
        return hr;
    }
#pragma endregion


    //*****************************************************************************
    // DirectX�̏�����
    //*****************************************************************************
#pragma region DirectX11Init
    UINT cdev_flags = 0;
#ifdef _DEBUG
    cdev_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    // �X���b�v�`�F�C���ݒ�
    DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
    ZeroMemory(&hDXGISwapChainDesc, sizeof(hDXGISwapChainDesc));
    hDXGISwapChainDesc.BufferDesc.Width  = SCREEN_WIDTH;                                   //�X���b�v�`�F�C���̕�
    hDXGISwapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;                                  //�X���b�v�`�F�C���̍���
    hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;                              //���t���b�V���i�ĕ`��j�Ɋւ��ݒ�@�ڍׂ͕s��
    hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;  //1/60 = 60fps             //���t���b�V���i�ĕ`��j�Ɋւ��ݒ�@�ڍׂ͕s��
    hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                     //�X���b�v�`�F�C����1�s�N�Z�����ێ�����f�[�^�^
    hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //��ʂ�`�悷��X�L�������C���̕����i���̐ݒ�Ȃ�DirectX���K���Ɍ��߂Ă����)
    hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                 //�摜�̈������΂��Ɋւ���t���O(���̐ݒ�Ȃ�DirectX���K���Ɍ��߂Ă����)
    hDXGISwapChainDesc.SampleDesc.Count = 1;                                               //�s�N�Z���V�F�[�_�̏����̍ہA�s�N�Z��1������T���v�����O���邩
    hDXGISwapChainDesc.SampleDesc.Quality = 0;                                             //�s�N�Z��1�Ɏg���f�[�^�� 0�`1�̊ԂŌ���
    hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                      //�ǂ�𗠉�ʂƂ��邩(���̐ݒ�Ȃ�RENDER_TARGET�Ɏw�肳�ꂽ�E�B���h�E)
    hDXGISwapChainDesc.BufferCount = 1;                                                    //�X���b�v�`�F�C���̐��A��{�͗���ʂ�1�ł����Ǝv��
    hDXGISwapChainDesc.OutputWindow = mhWnd;                                                //�ǂ̃E�B���h�E��\��ʂƂ��ĕ`�悷�邩
    hDXGISwapChainDesc.Windowed = true;                                                    //�N�����̃E�B���h�E���[�h(TRUE:�E�B���h�E���[�h�@FALSE:�t���X�N���[���ŋN��)
    hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                              //�`���Ƀo�b�N�o�b�t�@�̓��e��j������
    hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;                     //�E�B���h�E�̃T�C�Y�ύX��������

    D3D_FEATURE_LEVEL pLevels[] = { D3D_FEATURE_LEVEL_11_1 };     //DirectX�̃o�[�W�����𑗂邽�߂̕ϐ�
    D3D_FEATURE_LEVEL level;                                      //�ŏI�I�ɑI�����ꂽDirectX�̃o�[�W�������󂯎�邽�߂̕ϐ�

    //�ݒ肳�ꂽDXGI_SWAP_CHAIN_DESC����X���b�v�`�F�C���i����ʁj�Ƃ���ɃA�N�Z�X���邽�߂̃R���e�L�X�g���쐬
    hr = D3D11CreateDeviceAndSwapChain(
        adapter,                               //�f�B�X�v���C�A�_�v�^�̎w��@NULL�Ȃ�g������̂��g��
        D3D_DRIVER_TYPE_UNKNOWN,               //�`��h���C�o�̐ݒ�@��{�͂���ł���  D3D_DRIVER_TYPE_HARDWARE
        NULL,                                  //���Ń\�t�g�E�F�A�w�肵���ꍇ�A�����Ƀ\�t�g�E�F�A�̃|�C���^�������
        0,                                     //�����^�C�����C���w��@0��OK
        pLevels,                               //DirectX�̃o�[�W�����𑗂�
        1,                                     //�����^�C�����C���w�肠��ꍇ�A�����ɔz��̗v�f��������
        D3D11_SDK_VERSION,                     //DirectXSDK�o�[�W�����@11�̏ꍇ�����OK
        &hDXGISwapChainDesc,                   //�ݒ肳�ꂽDXGI_SWAP_CHAIN_DESC
        &mpSwapChain,                          //�����̍ۃX���b�v�`�F�C�����i�[����|�C���^
        &mpDevice,                             //�����̍ۑS�Ă̐e�ƂȂ�f�o�C�X���i�[����|�C���^
        &level,                                //�����̍ۍŏI�I�ɑI�����ꂽDirectX�̃o�[�W�������󂯎�邽�߂̃|�C���^
        &mpImContext);                         //�����̍ۃX���b�v�`�F�C�����Ǘ�����R���e�L�X�g���i�[����|�C���^

    if (FAILED(hr)) {
        //�쐬�Ɏ��s������x�����o���ăv���O�����������I��
        MessageBox(mhWnd, TEXT("D3D11CreateDeviceAndSwapChain"), TEXT("ERROR!!"), MB_ICONSTOP);
        PostQuitMessage(0);
    }

    //�A�_�v�^�����
    adapter->Release();
    adapter = 0;

    //�t�@�N�g�������
    factory->Release();
    factory = 0;

    //�쐬�����X���b�v�`�F�C���̃o�b�N�o�b�t�@�̃A�h���X�擾
    //(IDXGISwapChain*�^�ϐ�)->GetBuffer
    //(IDXGISwapChain*�^�ϐ�)�����o�b�t�@�̐ݒ�l���3�����̍\���̃|�C���^�Ɋi�[����
    //��1����:�o�b�t�@�C���f�b�N�X(��{��0)�@��2����:�i�[����\���̂̃T�C�Y�@��3����:�擾�����ݒ�l�̊i�[��
    if (FAILED(mpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&mpRTTex)))) {
        //�擾�Ɏ��s������x�����o���ăv���O�����������I��
        MessageBox(mhWnd, TEXT("SwpChain GetBuffer"), TEXT("ERROR!!"), MB_ICONSTOP);
        PostQuitMessage(0);
    }

    //�擾�����o�b�N�o�b�t�@�̐ݒ�l����ARenderTargetView(�����_�����O����쐬���ꂽ�s�N�Z�������󂯎��@�\������)�쐬
    //(ID3D11Device*�^�ϐ�)->CreateRenderTargetView
    //��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�o�b�t�@����RenderTargetView���쐬����
    //��1����:�ݒ�l���i�[���ꂽ�o�b�t�@�@��2����:�f�[�^�̏ڍ׎w��(NULL�ŕW���̒l�������ݒ�)�@��3����:RenderTargetView�̃A�h���X���󂯎��\����
    if (FAILED(mpDevice->CreateRenderTargetView(mpRTTex.Get(), NULL, &mpRTView))) {
        //�쐬�Ɏ��s������x�����o���ăv���O�����������I��
        MessageBox(mhWnd, TEXT("CreateRenderTargetView"), TEXT("ERROR!!"), MB_ICONSTOP);
        PostQuitMessage(0);
    }


    //�f�v�X�X�e���V���r���[�쐬
    //�f�v�X�X�e���V���r���[ : ���s���̕\�����s���f�v�X�@�\�ƁA�`���̃s�N�Z���F�Ƀ}�X�N���邩�ǂ������肷��X�e���V���@�\�����r���[
    D3D11_TEXTURE2D_DESC hTexture2dDesc;

    //�錾����D3D11_TEXTURE2D_DESC�ɐݒ�l�����Ă���
    hTexture2dDesc.Width  = hDXGISwapChainDesc.BufferDesc.Width;    //�e�N�X�`����:��ʃT�C�Y�Ɠ�����OK
    hTexture2dDesc.Height = hDXGISwapChainDesc.BufferDesc.Height;   //�e�N�X�`������:��ʃT�C�Y�Ɠ�����OK
    hTexture2dDesc.MipLevels = 1;                                   //�~�b�v�}�b�v�̐��@�l�𑝂₷�ƃe�N�X�`�������ꂢ�ɂȂ邪�d���Ȃ�
    hTexture2dDesc.ArraySize = 1;                                   //�e�N�X�`���̔z��T�C�Y
    hTexture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;          //�e�N�X�`��1�s�N�Z���̃f�[�^�T�C�Y�@���̐ݒ�͓���ŁA�f�v�X24bit�A�X�e���V��8bit���m�ۂ���
    hTexture2dDesc.SampleDesc = hDXGISwapChainDesc.SampleDesc;      //�}���`�T���v�����O�ݒ�@�f�v�X�X�e���V���ł͕s�v(�����ł͓K���ɒl�����Ă��邾��)
    hTexture2dDesc.Usage = D3D11_USAGE_DEFAULT;                     //�e�N�X�`���̎g�p�@(�����ł͕��ʂɃe�N�X�`���Ƃ��Ďg��)
    hTexture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;            //Bind�ݒ�(���̃e�N�X�`���͉��҂Ȃ̂�)���f�v�X�X�e���V���Ƃ��Ďg�����Ƃ𖾌�
    hTexture2dDesc.CPUAccessFlags = 0;                              //CPU�����ւ̃A�N�Z�X���@��{��0
    hTexture2dDesc.MiscFlags = 0;                                   //���\�[�X�I�v�V�����t���O�@��{��0

    //(ID3D11Device*�^�ϐ�)->CreateTexture2D
    //��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ���2�����e�N�X�`�����쐬����
    //��1����:�ݒ�l���i�[���ꂽ�o�b�t�@�@��2����:�f�[�^�̏ڍ׎w��(NULL�ŕW���̒l�������ݒ�)�@��3����:�쐬���ꂽ�e�N�X�`���̃A�h���X���󂯎��\����
    hr = mpDevice->CreateTexture2D(&hTexture2dDesc, NULL, &mpDSTex);
    if (FAILED(hr)) {
        //�쐬�Ɏ��s������x�����o���ăv���O�����������I��
        MessageBox(mhWnd, TEXT("CreateTexture2D"), TEXT("ERROR!!"), MB_ICONSTOP);
        PostQuitMessage(0);
    }

    if (SUCCEEDED(hr)) {
        //�����قǍ쐬�����f�v�X�X�e���V���p��2�����e�N�X�`���𗘗p���ăf�v�X�X�e���V���r���[�쐬
        //D3D11_DEPTH_STENCIL_VIEW_DESC : �f�v�X�X�e���V���r���[�̐ݒ���L�^���邽�߂̍\����
        D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;       

        //�錾����D3D11_DEPTH_STENCIL_VIEW_DESC�ɐݒ�l�����Ă���
        hDepthStencilViewDesc.Format = hTexture2dDesc.Format;                     //�r���[�̃T�C�Y�A�f�[�^�T�C�Y�Ȃǂ̐ݒ�A�������̃e�N�X�`���Ɠ���
        hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;    //�Ή������e�N�X�`���͂ǂ�Ȃ��̂Ȃ̂�(�����ł�2�����e�N�X�`�����w��)
        hDepthStencilViewDesc.Flags = 0;                                          //�e�N�X�`���͓ǂݎ���p���ǂ���(0�œǂݏ����\�A1�œǂݎ���p)

        //(ID3D11Device*�^�ϐ�)->CreateDepthStencilView
        //��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ���f�v�X�X�e���V���r���[���쐬����
        //��1����:���ƂȂ�e�N�X�`���@��2����:�ݒ�l���i�[���ꂽ�o�b�t�@�@��3����:�쐬���ꂽ�f�v�X�X�e���V���r���[���󂯎��\����
        hr = mpDevice->CreateDepthStencilView(mpDSTex.Get(), &hDepthStencilViewDesc, &mpDSView);
        if (FAILED(hr)) {
            //�쐬�Ɏ��s������x�����o���ăv���O�����������I��
            MessageBox(mhWnd, TEXT("CreateDepthStencilView"), TEXT("ERROR!!"), MB_ICONSTOP);
            PostQuitMessage(0);
        }
    }


    //�e�N�X�`���𒣂�ۂ̃T���v���[�̍쐬
    //�T���v���[ : �e�N�X�`����ǂݍ��ލۂ̐ݒ��o�^���邽�߂̂���
    D3D11_SAMPLER_DESC smpDesc; //D3D11_SAMPLER_DESC : �T���v���[�̐ݒ�l���i�[���邽�߂̍\����
    ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));
    //�錾����D3D11_SAMPLER_DESC�ɐݒ�l�����Ă���
    smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    smpDesc.BorderColor[0] = 1.0f;
    smpDesc.BorderColor[1] = 1.0f;
    smpDesc.BorderColor[2] = 1.0f;
    smpDesc.BorderColor[3] = 1.0f;
    smpDesc.MinLOD = -FLT_MAX;
    smpDesc.MaxLOD = +FLT_MAX;
    smpDesc.MipLODBias = 0.0f;
    smpDesc.MaxAnisotropy = 1; 
    smpDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    smpDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // or D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT

    //(ID3D11Device*�^�ϐ�)->CreateSamplerState
    //��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ���T���v���[���쐬����
    //��1����:�ݒ�l�̍\���́@��2����:�������ɃA�h���X��^����\����
    if (FAILED(mpDevice->CreateSamplerState(&smpDesc, &mpSampler))) {
        //�쐬�Ɏ��s������x�����o���ăv���O�����������I��
        MessageBox(NULL, TEXT("CreateSamplerState"), TEXT("ERROR!!"), MB_ICONSTOP);
        PostQuitMessage(0);
    }

    //�r���[�|�[�g�ݒ�
    mViewport.Width  = static_cast<FLOAT>SCREEN_WIDTH;
    mViewport.Height = static_cast<FLOAT>SCREEN_HEIGHT;
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;
    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;

    manager->mpImContext->RSSetViewports(1, &mViewport);

    //�����܂ł�DirectX11�S�̂̏�����

#pragma endregion

    return hr;
}

void DirectX11Manager::ClearScreen(void)
{
    Shader* shaderShadow = GetShaderShadow();

    // ��ʃN���A�i�w��F�œh��Ԃ��j
    // �h��Ԃ��F�̎w��i�����F0.0f�`1.0f�j
    float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

    mpImContext->PSSetSamplers(0, 1, mpSampler.GetAddressOf());
    mpImContext->ClearRenderTargetView(mpRTView.Get(), clearColor);
    mpImContext->ClearDepthStencilView(mpDSView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    // �e�p
    mpImContext->ClearDepthStencilView(shaderShadow->mpShadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DirectX11Manager::RenderTargetsSet(void)
{
    manager->mpImContext->OMSetRenderTargets(1, manager->mpRTView.GetAddressOf(), manager->mpDSView.Get());
}

void DirectX11Manager::RenderTargetsSetDisableZbuffer()
{
    manager->mpImContext->OMSetRenderTargets(1, manager->mpRTView.GetAddressOf(), NULL);
}

void DirectX11Manager::RenderTargetsSetShadow()
{
    Shader* shaderShadow = GetShaderShadow();

    manager->mpImContext->OMSetRenderTargets(0, nullptr, shaderShadow->mpShadowDepthStencilView.Get()); // �[�x�X�e���V���r���[���e�p��
}

//void DirectX11Manager::RenderTargetsSetShadowNoMotion()
//{
//    Shader* shaderNoMotion = GetShaderNoMotion();
//
//    manager->mpImContext->OMSetRenderTargets(0, nullptr, shaderNoMotion->mpShadowDepthStencilView.Get()); // �[�x�X�e���V���r���[���e�p��
//}

void DirectX11Manager::UpdateScreen(void)
{
    mpSwapChain->Present(0, 0);
}

UINT DirectX11Manager::GetScreenWidth(void) {
    return mScrWidth;
}

UINT DirectX11Manager::GetScreenHeight(void) {
    return mScrHeight;
}


//*************************************
// �֐�
//*************************************

DirectX11Manager* GetDirectX11Manager(void) {
    return manager;
}

HRESULT DirectX11Init(HINSTANCE hInstance, int nCmdShow) {

    HRESULT hr;
    manager = new DirectX11Manager;
    hr = manager->Init(hInstance, nCmdShow);
    if (FAILED(hr)) {
        MessageBox(manager->mhWnd, TEXT("[_Init]�����Ɏ��s���܂����B"), TEXT("�G���[����"), MB_OK | MB_ICONERROR);
    }

    return hr;
}

void DirectX11Release(void) {

    if (manager != nullptr) {
        delete manager;
    }
}


//=============================================================================
// �E�C���h�E�v���V�[�W��
//  (CALLBACK : Win32API�֐����Ăяo�����̋K��)
//  hWnd   : �E�B���h�E�̃n���h��
//  uMsg   : ���b�Z�[�W�̎��ʎq
//  wParam : ���b�Z�[�W�̍ŏ��̃p�����[�^
//  lParam : ���b�Z�[�W��2�Ԗڂ̃p�����[�^
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int sts;

    switch (uMsg)
    {
    case WM_DESTROY: // �E�B���h�E�j���̃��b�Z�[�W
        PostQuitMessage(0); // "WM_QUIT"���b�Z�[�W��Ԃ�
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            sts = MessageBox(hWnd, TEXT("�I�����܂���?"), TEXT("�I���m�F"), MB_YESNO);
            if (sts == IDYES) {
                DestroyWindow(hWnd);
            }
            break;
        }
        break;

    case WM_CLOSE:
        sts = IDYES;
        if (sts == IDYES) {
            DestroyWindow(hWnd);
        }
        break;

    default:

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
        break;
    }


    return 0;		// ����̏������
}


