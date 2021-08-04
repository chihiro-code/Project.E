#pragma once
#include "dx11texture.h"


DX11TEXTURE* Get_TEXTURE(void);
void ToonMap_Init(void);
void ToonMap_SetShaderResourceView(void);
void ToonMap_Release(void);