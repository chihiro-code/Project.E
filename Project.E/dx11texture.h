#pragma once
#include <d3d11.h>


struct DX11TEXTURE;


DX11TEXTURE* DX11TEXTURE_Create(void);
DX11TEXTURE* DX11TEXTURE_CreateAndLoadFile(const char*);
ID3D11ShaderResourceView** DX11TEXTURE_GetShaderResourceViewAddr(DX11TEXTURE*);
HRESULT DX11TEXTURE_LoadFile(DX11TEXTURE*, const char*);
void DX11TEXTURE_Release(DX11TEXTURE*);
