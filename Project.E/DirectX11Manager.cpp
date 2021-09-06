#include <iostream>
#include "DirectX11Manager.h"
#include "ShaderClass.h"

using namespace std;


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	CLASS_NAME		"サンプル"				// ウインドウクラスの名前
#define WINDOW_NAME		"Project.E"	            // ウィンドウの名前

#define	SCREEN_WIDTH	(1920)					// ウインドウの幅
#define	SCREEN_HEIGHT	(1080)					// ウインドウの高さ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//*****************************************************************************
// グローバル変数宣言
//*****************************************************************************
DirectX11Manager* manager;


//*************************************
// DirectX11Manager関数
//*************************************

HRESULT DirectX11Manager::Init(HINSTANCE hInstance, int nCmdShow)
{

    //*****************************************************************************
    // Windows関係の初期化
    //*****************************************************************************
    TCHAR ClassName[]  = TEXT(CLASS_NAME);
    TCHAR WindowName[] = TEXT(WINDOW_NAME);
    mScrWidth  = SCREEN_WIDTH;
    mScrHeight = SCREEN_HEIGHT;

    //Windowsウィンドウの作成
    WNDCLASSEX wcex =
    {
        wcex.cbSize        = sizeof(WNDCLASSEX),			// WNDCLASSEXのメモリサイズを指定
        wcex.style         = CS_CLASSDC,					// 表示するウインドウのスタイルを設定
        wcex.lpfnWndProc   = WndProc,						// ウィンドウプロシージャのアドレス(関数名)を指定
        wcex.cbClsExtra    = 0,								// 通常は使用しないので"0"を指定
        wcex.cbWndExtra    = 0,								// 通常は使用しないので"0"を指定
        wcex.hInstance     = hInstance,						// WinMainのパラメータのインスタンスハンドルを設定
        wcex.hIcon         = NULL,							// 使用するアイコンを指定(Windowsがもっているアイコンを使うならNULL)
        wcex.hCursor       = LoadCursor(NULL, IDC_ARROW),	// マウスカーソルを指定
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),	// ウインドウのクライアント領域の背景色を設定
        wcex.lpszMenuName  = NULL,							// Windowにつけるメニューを設定
        wcex.lpszClassName = ClassName,					    // ウインドウクラスの名前
        wcex.hIconSm       = NULL							// 小さいアイコンが設定された場合の情報を記述
    };
    // ウィンドウクラスの登録
    if (!RegisterClassEx(&wcex)) {
        return E_FAIL;
    }

    // ウインドウを作成
    mhWnd = CreateWindowEx(0,         // 拡張ウィンドウスタイル
        ClassName,				     // ウィンドウクラスの名前
        WindowName,                  // アプリケーションの名前
        WS_POPUP,	                 // ウィンドウスタイル    前の(WS_OVERLAPPEDWINDOW) 全画面(POPUP)
        CW_USEDEFAULT,			     // ウィンドウの左上Ｘ座標
        CW_USEDEFAULT,			     // ウィンドウの左上Ｙ座標 
        SCREEN_WIDTH,			     // ウィンドウの幅
        SCREEN_HEIGHT,			     // ウィンドウの高さ
        NULL,					     // 親ウィンドウのハンドル
        NULL,					     // メニューハンドルまたは子ウィンドウID
        hInstance,				     // インスタンスハンドル
        NULL);					     // ウィンドウ作成データ

    // ウインドウの表示
    ShowWindow(mhWnd, nCmdShow);
    UpdateWindow(mhWnd);

    //Windowsウィンドウの作成ここまで


    //*****************************************************************************
    // ハードウェアのチェック
    //*****************************************************************************
#pragma region HardWare Check
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes = 0;
    size_t stringLength;
    DXGI_ADAPTER_DESC adapterDesc;

    //グラフィック インタフェース ファクトリを作成
    auto hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(hr))
    {
        return hr;
    }

    int GPUNumber = 0;
    int GPUMaxMem = 0;
    //一番強いGPUアダプタを検索
    for (int i = 0; i < 100; i++)
    {
        IDXGIAdapter* add;
        hr = factory->EnumAdapters(i, &add);
        if (FAILED(hr))
            break;
        hr = add->GetDesc(&adapterDesc);

        char videoCardDescription[128];
        //ビデオカード名を取得
        int error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
        if (error != 0)
        {
            break;
        }
        cout << "ビデオカード名 : " << videoCardDescription << endl;

        //ビデオカードメモリを取得（MB単位）
        int videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
        cout << "ビデオメモリー : " << videoCardMemory << endl;

        //アウトプット（モニター）に番号IDを付ける
        hr = add->EnumOutputs(0, &adapterOutput);
        if (FAILED(hr))
        {
            continue;
        }

        //DXGI_FORMAT_R8G8B8A8_UNORM の表示形式数を取得する
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
        //アダプタアウトプットを解放
        adapterOutput->Release();
        adapterOutput = 0;
    }

    //グラフィック インタフェース アダプターを作成
    hr = factory->EnumAdapters(GPUNumber, &adapter);
    if (FAILED(hr))
    {
        return hr;
    }
#pragma endregion


    //*****************************************************************************
    // DirectXの初期化
    //*****************************************************************************
#pragma region DirectX11Init
    UINT cdev_flags = 0;
#ifdef _DEBUG
    cdev_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    // スワップチェイン設定
    DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
    ZeroMemory(&hDXGISwapChainDesc, sizeof(hDXGISwapChainDesc));
    hDXGISwapChainDesc.BufferDesc.Width  = SCREEN_WIDTH;                                   //スワップチェインの幅
    hDXGISwapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;                                  //スワップチェインの高さ
    hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;                              //リフレッシュ（再描画）に関わる設定　詳細は不明
    hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;  //1/60 = 60fps             //リフレッシュ（再描画）に関わる設定　詳細は不明
    hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                     //スワップチェインの1ピクセルが保持するデータ型
    hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //画面を描画するスキャンラインの方式（この設定ならDirectXが適当に決めてくれる)
    hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                 //画像の引き延ばしに関するフラグ(この設定ならDirectXが適当に決めてくれる)
    hDXGISwapChainDesc.SampleDesc.Count = 1;                                               //ピクセルシェーダの処理の際、ピクセル1つを何回サンプリングするか
    hDXGISwapChainDesc.SampleDesc.Quality = 0;                                             //ピクセル1つに使うデータ量 0～1の間で決定
    hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                      //どれを裏画面とするか(この設定ならRENDER_TARGETに指定されたウィンドウ)
    hDXGISwapChainDesc.BufferCount = 1;                                                    //スワップチェインの数、基本は裏画面は1つでいいと思う
    hDXGISwapChainDesc.OutputWindow = mhWnd;                                                //どのウィンドウを表画面として描画するか
    hDXGISwapChainDesc.Windowed = true;                                                    //起動時のウィンドウモード(TRUE:ウィンドウモード　FALSE:フルスクリーンで起動)
    hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                              //描画後にバックバッファの内容を破棄する
    hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;                     //ウィンドウのサイズ変更を許可する

    D3D_FEATURE_LEVEL pLevels[] = { D3D_FEATURE_LEVEL_11_1 };     //DirectXのバージョンを送るための変数
    D3D_FEATURE_LEVEL level;                                      //最終的に選択されたDirectXのバージョンを受け取るための変数

    //設定されたDXGI_SWAP_CHAIN_DESCからスワップチェイン（裏画面）とそれにアクセスするためのコンテキストを作成
    hr = D3D11CreateDeviceAndSwapChain(
        adapter,                               //ディスプレイアダプタの指定　NULLなら使えるものを使う
        D3D_DRIVER_TYPE_UNKNOWN,               //描画ドライバの設定　基本はこれでいい  D3D_DRIVER_TYPE_HARDWARE
        NULL,                                  //↑でソフトウェア指定した場合、ここにソフトウェアのポインタをいれる
        0,                                     //ランタイムレイヤ指定　0でOK
        pLevels,                               //DirectXのバージョンを送る
        1,                                     //ランタイムレイヤ指定ある場合、ここに配列の要素数を入れる
        D3D11_SDK_VERSION,                     //DirectXSDKバージョン　11の場合これでOK
        &hDXGISwapChainDesc,                   //設定されたDXGI_SWAP_CHAIN_DESC
        &mpSwapChain,                          //成功の際スワップチェインを格納するポインタ
        &mpDevice,                             //成功の際全ての親となるデバイスを格納するポインタ
        &level,                                //成功の際最終的に選択されたDirectXのバージョンを受け取るためのポインタ
        &mpImContext);                         //成功の際スワップチェインを管理するコンテキストを格納するポインタ

    if (FAILED(hr)) {
        //作成に失敗したら警告を出してプログラムを強制終了
        MessageBox(mhWnd, TEXT("D3D11CreateDeviceAndSwapChain"), TEXT("ERROR!!"), MB_ICONSTOP);
        PostQuitMessage(0);
    }

    //アダプタを解放
    adapter->Release();
    adapter = 0;

    //ファクトリを解放
    factory->Release();
    factory = 0;

    //作成したスワップチェインのバックバッファのアドレス取得
    //(IDXGISwapChain*型変数)->GetBuffer
    //(IDXGISwapChain*型変数)が持つバッファの設定値を第3引数の構造体ポインタに格納する
    //第1引数:バッファインデックス(基本は0)　第2引数:格納する構造体のサイズ　第3引数:取得した設定値の格納先
    if (FAILED(mpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&mpRTTex)))) {
        //取得に失敗したら警告を出してプログラムを強制終了
        MessageBox(mhWnd, TEXT("SwpChain GetBuffer"), TEXT("ERROR!!"), MB_ICONSTOP);
        PostQuitMessage(0);
    }

    //取得したバックバッファの設定値から、RenderTargetView(レンダリングから作成されたピクセル情報を受け取る機能を持つ)作成
    //(ID3D11Device*型変数)->CreateRenderTargetView
    //一番の親であるデバイス(ID3D11Device*型変数)の権利で、バッファからRenderTargetViewを作成する
    //第1引数:設定値が格納されたバッファ　第2引数:データの詳細指定(NULLで標準の値が自動設定)　第3引数:RenderTargetViewのアドレスを受け取る構造体
    if (FAILED(mpDevice->CreateRenderTargetView(mpRTTex.Get(), NULL, &mpRTView))) {
        //作成に失敗したら警告を出してプログラムを強制終了
        MessageBox(mhWnd, TEXT("CreateRenderTargetView"), TEXT("ERROR!!"), MB_ICONSTOP);
        PostQuitMessage(0);
    }


    //デプスステンシルビュー作成
    //デプスステンシルビュー : 奥行きの表現を行うデプス機能と、描画先のピクセル色にマスクするかどうか判定するステンシル機能を持つビュー
    D3D11_TEXTURE2D_DESC hTexture2dDesc;

    //宣言したD3D11_TEXTURE2D_DESCに設定値を入れていく
    hTexture2dDesc.Width  = hDXGISwapChainDesc.BufferDesc.Width;    //テクスチャ幅:画面サイズと同じでOK
    hTexture2dDesc.Height = hDXGISwapChainDesc.BufferDesc.Height;   //テクスチャ高さ:画面サイズと同じでOK
    hTexture2dDesc.MipLevels = 1;                                   //ミップマップの数　値を増やすとテクスチャがきれいになるが重くなる
    hTexture2dDesc.ArraySize = 1;                                   //テクスチャの配列サイズ
    hTexture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;          //テクスチャ1ピクセルのデータサイズ　この設定は特殊で、デプス24bit、ステンシル8bitを確保する
    hTexture2dDesc.SampleDesc = hDXGISwapChainDesc.SampleDesc;      //マルチサンプリング設定　デプスステンシルでは不要(ここでは適当に値を入れているだけ)
    hTexture2dDesc.Usage = D3D11_USAGE_DEFAULT;                     //テクスチャの使用法(ここでは普通にテクスチャとして使う)
    hTexture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;            //Bind設定(このテクスチャは何者なのか)→デプスステンシルとして使うことを明言
    hTexture2dDesc.CPUAccessFlags = 0;                              //CPU資源へのアクセス権　基本は0
    hTexture2dDesc.MiscFlags = 0;                                   //リソースオプションフラグ　基本は0

    //(ID3D11Device*型変数)->CreateTexture2D
    //一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目から2次元テクスチャを作成する
    //第1引数:設定値が格納されたバッファ　第2引数:データの詳細指定(NULLで標準の値が自動設定)　第3引数:作成されたテクスチャのアドレスを受け取る構造体
    hr = mpDevice->CreateTexture2D(&hTexture2dDesc, NULL, &mpDSTex);
    if (FAILED(hr)) {
        //作成に失敗したら警告を出してプログラムを強制終了
        MessageBox(mhWnd, TEXT("CreateTexture2D"), TEXT("ERROR!!"), MB_ICONSTOP);
        PostQuitMessage(0);
    }

    if (SUCCEEDED(hr)) {
        //さきほど作成したデプスステンシル用の2次元テクスチャを利用してデプスステンシルビュー作成
        //D3D11_DEPTH_STENCIL_VIEW_DESC : デプスステンシルビューの設定を記録するための構造体
        D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;       

        //宣言したD3D11_DEPTH_STENCIL_VIEW_DESCに設定値を入れていく
        hDepthStencilViewDesc.Format = hTexture2dDesc.Format;                     //ビューのサイズ、データサイズなどの設定、さっきのテクスチャと同じ
        hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;    //対応したテクスチャはどんなものなのか(ここでは2次元テクスチャを指定)
        hDepthStencilViewDesc.Flags = 0;                                          //テクスチャは読み取り専用かどうか(0で読み書き可能、1で読み取り専用)

        //(ID3D11Device*型変数)->CreateDepthStencilView
        //一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からデプスステンシルビューを作成する
        //第1引数:元となるテクスチャ　第2引数:設定値が格納されたバッファ　第3引数:作成されたデプスステンシルビューを受け取る構造体
        hr = mpDevice->CreateDepthStencilView(mpDSTex.Get(), &hDepthStencilViewDesc, &mpDSView);
        if (FAILED(hr)) {
            //作成に失敗したら警告を出してプログラムを強制終了
            MessageBox(mhWnd, TEXT("CreateDepthStencilView"), TEXT("ERROR!!"), MB_ICONSTOP);
            PostQuitMessage(0);
        }
    }


    //テクスチャを張る際のサンプラーの作成
    //サンプラー : テクスチャを読み込む際の設定を登録するためのもの
    D3D11_SAMPLER_DESC smpDesc; //D3D11_SAMPLER_DESC : サンプラーの設定値を格納するための構造体
    ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));
    //宣言したD3D11_SAMPLER_DESCに設定値を入れていく
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

    //(ID3D11Device*型変数)->CreateSamplerState
    //一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からサンプラーを作成する
    //第1引数:設定値の構造体　第2引数:成功時にアドレスを与える構造体
    if (FAILED(mpDevice->CreateSamplerState(&smpDesc, &mpSampler))) {
        //作成に失敗したら警告を出してプログラムを強制終了
        MessageBox(NULL, TEXT("CreateSamplerState"), TEXT("ERROR!!"), MB_ICONSTOP);
        PostQuitMessage(0);
    }

    //ビューポート設定
    mViewport.Width  = static_cast<FLOAT>SCREEN_WIDTH;
    mViewport.Height = static_cast<FLOAT>SCREEN_HEIGHT;
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;
    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;

    manager->mpImContext->RSSetViewports(1, &mViewport);

    //ここまでがDirectX11全体の初期化

#pragma endregion

    return hr;
}

void DirectX11Manager::ClearScreen(void)
{
    Shader* shaderShadow = GetShaderShadow();

    // 画面クリア（指定色で塗りつぶし）
    // 塗りつぶし色の指定（小数：0.0f～1.0f）
    float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

    mpImContext->PSSetSamplers(0, 1, mpSampler.GetAddressOf());
    mpImContext->ClearRenderTargetView(mpRTView.Get(), clearColor);
    mpImContext->ClearDepthStencilView(mpDSView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    // 影用
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

    manager->mpImContext->OMSetRenderTargets(0, nullptr, shaderShadow->mpShadowDepthStencilView.Get()); // 深度ステンシルビューを影用へ
}

//void DirectX11Manager::RenderTargetsSetShadowNoMotion()
//{
//    Shader* shaderNoMotion = GetShaderNoMotion();
//
//    manager->mpImContext->OMSetRenderTargets(0, nullptr, shaderNoMotion->mpShadowDepthStencilView.Get()); // 深度ステンシルビューを影用へ
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
// 関数
//*************************************

DirectX11Manager* GetDirectX11Manager(void) {
    return manager;
}

HRESULT DirectX11Init(HINSTANCE hInstance, int nCmdShow) {

    HRESULT hr;
    manager = new DirectX11Manager;
    hr = manager->Init(hInstance, nCmdShow);
    if (FAILED(hr)) {
        MessageBox(manager->mhWnd, TEXT("[_Init]処理に失敗しました。"), TEXT("エラー発生"), MB_OK | MB_ICONERROR);
    }

    return hr;
}

void DirectX11Release(void) {

    if (manager != nullptr) {
        delete manager;
    }
}


//=============================================================================
// ウインドウプロシージャ
//  (CALLBACK : Win32API関数を呼び出す時の規約)
//  hWnd   : ウィンドウのハンドル
//  uMsg   : メッセージの識別子
//  wParam : メッセージの最初のパラメータ
//  lParam : メッセージの2番目のパラメータ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int sts;

    switch (uMsg)
    {
    case WM_DESTROY: // ウィンドウ破棄のメッセージ
        PostQuitMessage(0); // "WM_QUIT"メッセージを返す
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            sts = MessageBox(hWnd, TEXT("終了しますか?"), TEXT("終了確認"), MB_YESNO);
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


    return 0;		// 既定の処理を提供
}


