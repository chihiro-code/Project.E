#include "ToonMap.h"
#include "DirectX11Manager.h"


//*************************************
// グローバル変数
//*************************************
DX11TEXTURE* gpTextureArrayToonMap;


//*************************************
// 関数
//*************************************
DX11TEXTURE* Get_TEXTURE(void) {

	return gpTextureArrayToonMap;
}

void ToonMap_Init(void) {

	// テクスチャを読み込む
	gpTextureArrayToonMap = DX11TEXTURE_CreateAndLoadFile("assets/texture/ToonMap_02.png");

}

void ToonMap_SetShaderResourceView(void) {

	DirectX11Manager* manager = GetDirectX11Manager();

	// ピクセルシェーダーにテクスチャをセット
	manager->mpImContext->PSSetShaderResources(1, 1, DX11TEXTURE_GetShaderResourceViewAddr(gpTextureArrayToonMap));

}

void ToonMap_Release(void) {

	free(gpTextureArrayToonMap);
}