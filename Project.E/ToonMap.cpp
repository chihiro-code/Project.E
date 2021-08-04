#include "ToonMap.h"
#include "DirectX11Manager.h"


//*************************************
// �O���[�o���ϐ�
//*************************************
DX11TEXTURE* gpTextureArrayToonMap;


//*************************************
// �֐�
//*************************************
DX11TEXTURE* Get_TEXTURE(void) {

	return gpTextureArrayToonMap;
}

void ToonMap_Init(void) {

	// �e�N�X�`����ǂݍ���
	gpTextureArrayToonMap = DX11TEXTURE_CreateAndLoadFile("assets/texture/ToonMap_02.png");

}

void ToonMap_SetShaderResourceView(void) {

	DirectX11Manager* manager = GetDirectX11Manager();

	// �s�N�Z���V�F�[�_�[�Ƀe�N�X�`�����Z�b�g
	manager->mpImContext->PSSetShaderResources(1, 1, DX11TEXTURE_GetShaderResourceViewAddr(gpTextureArrayToonMap));

}

void ToonMap_Release(void) {

	free(gpTextureArrayToonMap);
}