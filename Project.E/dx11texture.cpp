
#include "dx11texture.h"
#include "DirectX11Manager.h"
#include "dx11tex\WICTextureLoader.h"
#include <locale.h>



struct DX11TEXTURE {
	ID3D11Resource* pTex;
	ID3D11ShaderResourceView* pTexView;
};



// DX11TEXTURE構造体を動的確保して実体化する
DX11TEXTURE* DX11TEXTURE_Create(void) {
	DX11TEXTURE* pTex = (DX11TEXTURE*)malloc(sizeof(DX11TEXTURE));

	pTex->pTex = NULL;
	pTex->pTexView = NULL;

	return pTex;
}


DX11TEXTURE* DX11TEXTURE_CreateAndLoadFile(const char* path) {
	DX11TEXTURE* pTex = DX11TEXTURE_Create();

	if (pTex != NULL) {
		HRESULT hr = DX11TEXTURE_LoadFile(pTex, path);

		if (SUCCEEDED(hr)) {
			return pTex;
		}
		else {
			DX11TEXTURE_Release(pTex);
		}
	}

	return NULL;
}


ID3D11ShaderResourceView** DX11TEXTURE_GetShaderResourceViewAddr(DX11TEXTURE* pTex) {
	return &pTex->pTexView;
}


// 画像ファイルを読み込んでテクスチャとしてメモリに展開する
HRESULT DX11TEXTURE_LoadFile(DX11TEXTURE* pTex, const char* path) {
	wchar_t wc_path[256];

	char* localeInfo = setlocale(LC_ALL, "Japanese_Japan.932");
	mbstowcs_s(NULL, wc_path, 256, path, 255);

	DirectX11Manager* manager = GetDirectX11Manager();

	return DirectX::CreateWICTextureFromFile(manager->mpDevice.Get(), wc_path, &pTex->pTex, &pTex->pTexView);
}


void DX11TEXTURE_Release(DX11TEXTURE* pTex) {
	if (pTex != NULL) {
		if (pTex->pTexView != NULL) {
			pTex->pTexView->Release();
			pTex->pTexView = NULL;
		}
		if (pTex->pTex != NULL) {
			pTex->pTex->Release();
			pTex->pTex = NULL;
		}

		free(pTex);
	}
}
