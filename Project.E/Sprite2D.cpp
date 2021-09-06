#include "Sprite2D.h"


//*************************************
// グローバル変数
//*************************************
VertexShader    Sprite2D::mpVertexShader;
PixelShader     Sprite2D::mpPixelShader;
InputLayout     Sprite2D::mpInputLayout;
RasterizerState Sprite2D::mpRasterizerState;


//*************************************
// シェーダー
//*************************************
#include "shader\vssprite2d.h"
#include "shader\pssprite2d.h"


Sprite2D::Sprite2D()
{

}

Sprite2D::~Sprite2D()
{

}

void Sprite2D::Init(int texID)
{
	DirectX11Manager* manager = GetDirectX11Manager();


	// インプットレイアウト
	if (mpInputLayout == NULL) {
		D3D11_INPUT_ELEMENT_DESC desc[] = {
			{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",		 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		if (FAILED(manager->mpDevice.Get()->CreateInputLayout(desc, 3, g_vmain, sizeof(g_vmain), mpInputLayout.GetAddressOf()))) {
			//作成に失敗したら警告を出してプログラムを強制終了
			MessageBox(NULL, TEXT("CreateInputLayout"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// 頂点
	if (mpVertexShader == NULL) {
		if (FAILED(manager->mpDevice.Get()->CreateVertexShader(g_vmain, sizeof(g_vmain), NULL, mpVertexShader.GetAddressOf()))) {
			//作成に失敗したら警告を出してプログラムを強制終了
			MessageBox(NULL, TEXT("CreateVertexShader"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// ピクセル
	if (mpPixelShader == NULL) {
		if (FAILED(manager->mpDevice.Get()->CreatePixelShader(g_pmain, sizeof(g_pmain), NULL, mpPixelShader.GetAddressOf()))) {
			//作成に失敗したら警告を出してプログラムを強制終了
			MessageBox(NULL, TEXT("CreatePixelShader"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// ラスタライザー
	if (mpRasterizerState == NULL) {
		D3D11_RASTERIZER_DESC rasterDesc;
		// setup the raster description which will determine how and what polygons will be drawn
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		//	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;		// ワイヤフレームにしたいとき
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		if (FAILED(manager->mpDevice.Get()->CreateRasterizerState(&rasterDesc, mpRasterizerState.GetAddressOf()))) {
			//作成に失敗したら警告を出してプログラムを強制終了
			MessageBox(NULL, TEXT("CreateRasterizerState"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// 座標、色
	for (int i = 0; i < 4; i++) {
		mVertices[i].pos.z = 0.0f;
		mVertices[i].color.r = 1.0f;
		mVertices[i].color.g = 1.0f;
		mVertices[i].color.b = 1.0f;
		mVertices[i].color.a = 1.0f;
	}
	SetRect(0, 0, 50, 50);  // デフォルト位置サイズ

	// UV
	mVertices[0].uv.u = 0.0f;
	mVertices[0].uv.v = 0.0f;
	mVertices[1].uv.u = 1.0f;
	mVertices[1].uv.v = 0.0f;
	mVertices[2].uv.u = 0.0f;
	mVertices[2].uv.v = 1.0f;
	mVertices[3].uv.u = 1.0f;
	mVertices[3].uv.v = 1.0f;

	// テクスチャのロード
	if (mpTexture == NULL) {
		
		mpTexture = DX11TEXTURE_CreateAndLoadFile(gpTextureFileNames[texID]);  // テクスチャのファイル名を指定

		if (mpTexture == NULL) {
			//作成に失敗したら警告を出してプログラムを強制終了
			MessageBox(NULL, TEXT("CreateAndLoadFile"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// バッファー作成
	CreateVertexBuffer();

}

void Sprite2D::Draw()
{
	DirectX11Manager* manager = GetDirectX11Manager();

	// 頂点バッファの頂点の各パラメータを書き換える、　頂点データの更新
	D3D11_MAPPED_SUBRESOURCE mapRes;
	HRESULT hr = manager->mpImContext.Get()->Map(mpVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapRes);

	if (SUCCEEDED(hr)) {
		memcpy_s(mapRes.pData, mapRes.RowPitch, mVertices, sizeof(mVertices));
		manager->mpImContext.Get()->Unmap(mpVertexBuffer.Get(), 0);
	}

	const UINT stride = sizeof(VERTEX_SPRITE2D);
	const UINT offset = 0;

	manager->mpImContext.Get()->IASetVertexBuffers(0, 1, mpVertexBuffer.GetAddressOf(), &stride, &offset);
	manager->mpImContext.Get()->IASetInputLayout(mpInputLayout.Get());
	manager->mpImContext.Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	manager->mpImContext.Get()->RSSetState(mpRasterizerState.Get());

	manager->mpImContext.Get()->VSSetShader(mpVertexShader.Get(), NULL, 0);
	manager->mpImContext.Get()->PSSetShader(mpPixelShader.Get(), NULL, 0);
	manager->mpImContext.Get()->PSSetShaderResources(0, 1, DX11TEXTURE_GetShaderResourceViewAddr(mpTexture));
	manager->mpImContext.Get()->PSSetSamplers(0, 1, manager->mpSampler.GetAddressOf()); // サンプラーをセットする

	manager->RenderTargetsSetDisableZbuffer(); // Zバッファの無効化
	manager->mpImContext.Get()->Draw(4, 0);
	manager->RenderTargetsSet();  // Zバッファの有効化
}

void Sprite2D::SetTexture(DX11TEXTURE* pTex)
{
	mpTexture = pTex;
}

void Sprite2D::SetRect(float left, float top, float right, float bottom)
{
	DirectX11Manager* manager = GetDirectX11Manager();

	// 絶対座標を相対座標に変換
	left   = left          / manager->GetScreenWidth() * 2.0f - 1.0f;
	top    = 1.0f - top    / manager->GetScreenHeight() * 2.0f;
	right  = right         / manager->GetScreenWidth() * 2.0f - 1.0f;
	bottom = 1.0f - bottom / manager->GetScreenHeight() * 2.0f;

	// 各頂点に座標を設定
	mVertices[0].pos.x = left;
	mVertices[0].pos.y = top;
	mVertices[1].pos.x = right;
	mVertices[1].pos.y = top;
	mVertices[2].pos.x = left;
	mVertices[2].pos.y = bottom;
	mVertices[3].pos.x = right;
	mVertices[3].pos.y = bottom;
}

void Sprite2D::SetColor(FRGBA v0, FRGBA v1, FRGBA v2, FRGBA v3)
{
	mVertices[0].color = v0;
	mVertices[1].color = v1;
	mVertices[2].color = v2;
	mVertices[3].color = v3;
}

bool Sprite2D::CreateVertexBuffer(void)
{
	DirectX11Manager* manager = GetDirectX11Manager();

	//頂点バッファ作成
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(mVertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResourceData;
	subResourceData.pSysMem = mVertices;
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;

	HRESULT hr = manager->mpDevice.Get()->CreateBuffer(&bufferDesc, &subResourceData, mpVertexBuffer.GetAddressOf());

	return SUCCEEDED(hr);
}
