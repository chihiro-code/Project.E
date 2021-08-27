#include "dx11tex\WICTextureLoader.h"
#include "MeshElementHeader.h"
#include "GameObject.h"
#include "ShaderClass.h"
#include "Camera.h"
#include "ToonMap.h"
#include "input.h"
#include "Light.h"
#include "ComponentBase.h"
#include "ColliderComponent.h"
#include "Map.h"
 

//*************************************
// �}�N����`
//*************************************
#define COLLIDER_NUM 3


//*************************************
// �O���[�o���ϐ�
//*************************************
GameObject* gpGameObject[MAX_GAMEOBJECT];
ColliderComponent* gpColliderComponent[COLLIDER_NUM];
ColliderParameter gObjectColliderParameter[MAX_GAMEOBJECT];


//*************************************
// GameObject�֐�
//*************************************

//-------------------------------------
// �I�u�W�F�N�g
//-------------------------------------

GameObject::GameObject()
{
	mDrawFlag = true;		
	mDrawShadow = false;    
	mAlive = true; 
	mPos = { 0, 0, 0 };
	mRot = { 0, 0, 0 };
	mScl = { 1.0f, 1.0f, 1.0f };

}

GameObject::~GameObject()
{
	
}

void GameObject::Init(int modelId, int texId)
{
	// ���f���f�[�^�ǂݍ���
	FbxInit((char*)gpFbxModelFileNames[modelId], (char*)gpFbxTextureFileNames[texId]);

}

void GameObject::Update(void)
{
	
}

void GameObject::Draw(void)
{
	FbxDraw();
}

void GameObject::Release(void)
{
	// �{�[��
	if (mMeshData->motion == true) {
		if (mMeshData->animator.modelBone != nullptr) {
			free(mMeshData->animator.modelBone); // Bone*
			mMeshData->animator.modelBone = nullptr;
		}
	}

	// �T�u���b�V��
	if (mMeshData->pSubmeshArray->pIndexBuffer != nullptr) {
		SAFE_RELEASE(mMeshData->pSubmeshArray->pIndexBuffer);  // ID3D11Buffer*
	}
	if (mMeshData->pSubmeshArray->pVertexBuffer != nullptr) {
		SAFE_RELEASE(mMeshData->pSubmeshArray->pVertexBuffer); // ID3D11Buffer*
	}
	if (mMeshData->pSubmeshArray->pTexture != nullptr) {
		DX11TEXTURE_Release(mMeshData->pSubmeshArray->pTexture); // DX11TEXTURE*
	}
	if (mMeshData->pSubmeshArray != nullptr) {
		free(mMeshData->pSubmeshArray); // SUBMESH*
		mMeshData->pSubmeshArray = nullptr;
	}

	// ���b�V��
	if (mMeshData != nullptr) {
		free(mMeshData); // MESH*
		mMeshData = nullptr;
	}
}

void GameObject::PositionSet(float x, float y, float z)
{
	mPos = { x, y,z };
}

void GameObject::RotationSet(float x, float y, float z)
{
	mRot = { x, y,z };
}

void GameObject::ScaleSet(float x, float y, float z)
{
	mScl = { x, y,z };
}

void GameObject::SpecularSet(float red, float green, float blue, float power)
{
	mMeshData->pSubmeshArray->material.specular[0] = red;
	mMeshData->pSubmeshArray->material.specular[1] = green;
	mMeshData->pSubmeshArray->material.specular[2] = blue;
	mMeshData->pSubmeshArray->material.specular[3] = power;
	
}

void GameObject::ShadowOnOf(bool flag)
{
	mDrawShadow = flag;
}

XMFLOAT3 GameObject::PositionGet()
{
	XMFLOAT3 pos = { mPos.x, mPos.y, mPos.z };
	return pos;
}

XMFLOAT3 GameObject::RotationGet()
{
	XMFLOAT3 rot = { mRot.x, mRot.y, mRot.z };
	return rot;
}

XMFLOAT3 GameObject::ScaleGet()
{
	XMFLOAT3 scl = { mScl.x, mScl.y, mScl.z };
	return scl;
}

void GameObject::AliveSet(bool alive)
{
	mAlive = alive;
}


//-------------------------------------
// ���f���ǂݍ���
//-------------------------------------

void GameObject::FbxInit(char* fbxPath, char* texturePath)
{
	mMeshData = NULL;
	mMeshData = (Mesh*)malloc(sizeof(Mesh));

	FbxLoader(fbxPath);
	mMeshData->count = 0;

	mMeshData->pSubmeshArray->pTexture = NULL;
	if (mMeshData->pSubmeshArray->pTexture == NULL) {

		mMeshData->pSubmeshArray->pTexture = DX11TEXTURE_CreateAndLoadFile(texturePath);  // �e�N�X�`���f�[�^�̃��[�_�[�֐�

		if (mMeshData->pSubmeshArray->pTexture == NULL) {
			//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
			MessageBox(NULL, TEXT("Texture"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// ���[�V��������Ȃ��ŃV�F�[�_�[�𕪂���
	FbxShaderInitChoice(mMeshData);

}

void GameObject::FbxShaderInitChoice(Mesh* pMesh)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shader = GetShader();

	// ���[�V�������肩�Ȃ���
	if (pMesh->motion == true) {
		//�A�j���[�V�����Ɋւ��鏈��
		//������
		for (int i = 0; i < pMesh->boneNum; i++) {
			pMesh->animator.animeBuffer.animematrix[i] = XMMatrixIdentity();
		}

		////�����|�[�Y�Z�b�g
		FbxSetPose(0);

		//�`��p�R���e�L�X�g�ɍ����������W�ϊ��s���o�^
		/*if (shader ->mpConstantBufferAnime.Get() != NULL) {
			manager->mpImContext->UpdateSubresource(shader->mpConstantBufferAnime.Get(), 0, NULL, &pMesh->animator.animeBuffer, 0, 0);
		}*/
	}
	else {
		// ���[�V�����Ȃ��̂���NULL����
		pMesh->animator.modelBone = nullptr;
	}
}

void GameObject::FbxShaderDraw(CameraBuffer cb)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shader = GetShader();
	Camera* camera = GetCamera();
	Light*  light  = GetLight();


	//�`��p�R���e�L�X�g�ɍ����������W�ϊ��s���o�^
	if (shader->mpConstantBufferWorld.Get() != NULL) {
		manager->mpImContext->UpdateSubresource(shader->mpConstantBufferWorld.Get(), 0, NULL, &cb, 0, 0);
	}


	//������
	for (int i = 0; i < mMeshData->boneNum; i++) {
		mMeshData->animator.animeBuffer.animematrix[i] = XMMatrixIdentity();
	}


	mMeshData->count++;
	if (mMeshData->count == mMeshData->animator.endframe) mMeshData->count = mMeshData->animator.startframe;
	if (mMeshData->count < 0) mMeshData->count = 0;
	/*mMeshData->animator.animeBuffer = FbxSetPose(mMeshData->count);*/
	FbxSetPose(mMeshData->count);


	//�`��p�R���e�L�X�g�ɍ����������W�ϊ��s���o�^
	if (shader->mpConstantBufferAnime.Get() != NULL) {
		manager->mpImContext->UpdateSubresource(shader->mpConstantBufferAnime.Get(), 0, NULL, &mMeshData->animator.animeBuffer, 0, 0);
	}


	//���C�e�B���O
	//���E�̌��͂����Œ�`
	XMFLOAT3 lightPos   = light->GetPosition();
	XMFLOAT4 lightColor = light->GetColor();
	mLbuf.objectpos.x  = mPos.x;
	mLbuf.objectpos.y  = mPos.y;
	mLbuf.objectpos.z  = mPos.z;
	mLbuf.lightpos.x   = lightPos.x;
	mLbuf.lightpos.y   = lightPos.y;
	mLbuf.lightpos.z   = lightPos.z;
	mLbuf.lightpos.w   = 1.0f;
	mLbuf.lightcolor.x = lightColor.x;
	mLbuf.lightcolor.y = lightColor.y;
	mLbuf.lightcolor.z = lightColor.z;
	mLbuf.lightcolor.w = lightColor.w;
	mLbuf.camerapos = camera->GetPosition();


	UINT strides = sizeof(Vertex3D);
	UINT offsets = 0;

	manager->mpImContext->IASetInputLayout(shader->mpInputLayout.Get());                      //�`��p�R���e�L�X�g�ɒ��_���C�A�E�g��ݒ�
	manager->mpImContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);      //�`��p�R���e�L�X�g�̃|���S���`�揇���C���f�b�N�X�����ɐݒ�

	manager->mpImContext->VSSetConstantBuffers(0, 1, shader->mpConstantBufferWorld.GetAddressOf()); //�`��p�R���e�L�X�g�ɒ萔�o�b�t�@��ݒ�
	manager->mpImContext->VSSetConstantBuffers(1, 1, shader->mpConstantBufferAnime.GetAddressOf()); //�`��p�R���e�L�X�g�ɒ萔�o�b�t�@��ݒ�

	//�e�N�X�`�����s�N�Z���V�F�[�_�[�ɃZ�b�g
	manager->mpImContext->PSSetShaderResources(0, 1, DX11TEXTURE_GetShaderResourceViewAddr(mMeshData->pSubmeshArray->pTexture));
	manager->mpImContext->PSSetShaderResources(2, 1, shader->mpShadowShaderResourceView.GetAddressOf());

	manager->mpImContext->PSSetSamplers(0, 1, manager->mpSampler.GetAddressOf());             //�`��p�R���e�L�X�g�Ƀe�N�X�`���T���v���[��ݒ�
	manager->mpImContext->PSSetSamplers(1, 1, shader->mpShadowSampler.GetAddressOf());        //�`��p�R���e�L�X�g�Ƀe�N�X�`���T���v���[��ݒ�

	manager->mpImContext->VSSetShader(shader->mpVertexShader.Get(), NULL, 0);                 //�`��p�R���e�L�X�g�ɒ��_�V�F�[�_��ݒ�

	manager->mpImContext->RSSetViewports(1, &manager->mViewport);                             //�`��p�R���e�L�X�g�Ƀr���[�|�[�g��ݒ�                           
	manager->mpImContext->RSSetState(shader->mpRasterState.Get());

	// �e����
	if (mDrawShadow == true) manager->mpImContext->PSSetShader(shader->mpShadowPixelShader.Get(), NULL, 0); //�`��p�R���e�L�X�g�Ƀs�N�Z���V�F�[�_��ݒ�
	// �e�Ȃ�
	else manager->mpImContext->PSSetShader(shader->mpPixelShader.Get(), NULL, 0);                           //�`��p�R���e�L�X�g�Ƀs�N�Z���V�F�[�_��ݒ�

	// ToonMap
	ToonMap_SetShaderResourceView();

	//���݂̒��_���C�A�E�g�A���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�e�N�X�`���Ȃǂ̐ݒ�����ɁA
	//��1�����̐��������_���쐬���A�|���S����`�悷��

	//�ގ����ʂɕ`�悷�邽�߁A�T�u���b�V���񃋁[�v
	for (int i = 0; i < mMeshData->numSubmesh; i++) {

		//�`�悵�悤�Ƃ��Ă�����̂̃}�e���A���f�[�^��o�^
		mLbuf.diffuse.x  = mMeshData->pSubmeshArray[i].material.diffuse[0];
		mLbuf.diffuse.y  = mMeshData->pSubmeshArray[i].material.diffuse[1];
		mLbuf.diffuse.z  = mMeshData->pSubmeshArray[i].material.diffuse[2];
		mLbuf.ambient.x  = mMeshData->pSubmeshArray[i].material.ambient[0];
		mLbuf.ambient.y  = mMeshData->pSubmeshArray[i].material.ambient[1];
		mLbuf.ambient.z  = mMeshData->pSubmeshArray[i].material.ambient[2];
		mLbuf.specular.x = mMeshData->pSubmeshArray[i].material.specular[0];
		mLbuf.specular.y = mMeshData->pSubmeshArray[i].material.specular[1];
		mLbuf.specular.z = mMeshData->pSubmeshArray[i].material.specular[2];
		mLbuf.specular.w = mMeshData->pSubmeshArray[i].material.specular[3];

		//�`��p�R���e�L�X�g�ɍ����������W�ϊ��s���o�^
		if (shader->mpConstantBufferLight.Get() != NULL) {
			manager->mpImContext->UpdateSubresource(shader->mpConstantBufferLight.Get(), 0, NULL, &mLbuf, 0, 0);
		}

		//���C�e�B���O�萔�̓s�N�Z���V�F�[�_�ɑ��M
		manager->mpImContext->PSSetConstantBuffers(0, 1, shader->mpConstantBufferLight.GetAddressOf());                       //�`��p�R���e�L�X�g�ɒ萔�o�b�t�@��ݒ�

		manager->mpImContext->IASetVertexBuffers(0, 1, &mMeshData->pSubmeshArray[i].pVertexBuffer, &strides, &offsets); //�`��p�R���e�L�X�g�ɒ��_�o�b�t�@��ݒ�
		manager->mpImContext->IASetIndexBuffer(mMeshData->pSubmeshArray[i].pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);      //�`��p�R���e�L�X�g�ɃC���f�b�N�X�o�b�t�@��ݒ�
		manager->mpImContext->DrawIndexed(mMeshData->pSubmeshArray[i].numIndex, 0, 0);
	}
}

void GameObject::FbxNoMotionShaderDraw(CameraBuffer cb)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shaderNoMotion = GetShaderNoMotion();
	Camera* camera = GetCamera();
	Light*  light = GetLight();


	//�`��p�R���e�L�X�g�ɍ����������W�ϊ��s���o�^
	if (shaderNoMotion->mpConstantBufferWorld.Get() != NULL) {

		manager->mpImContext->UpdateSubresource(shaderNoMotion->mpConstantBufferWorld.Get(), 0, NULL, &cb, 0, 0);
	}


	//���C�e�B���O
	//���E�̌��͂����Œ�`
	XMFLOAT3 lightPos   = light->GetPosition();
	XMFLOAT4 lightColor = light->GetColor();
	mLbuf.objectpos.x  = mPos.x;
	mLbuf.objectpos.y  = mPos.y;
	mLbuf.objectpos.z  = mPos.z;
	mLbuf.lightpos.x   = lightPos.x;
	mLbuf.lightpos.y   = lightPos.y;
	mLbuf.lightpos.z   = lightPos.z;
	mLbuf.lightpos.w   = 1.0f;
	mLbuf.lightcolor.x = lightColor.x;
	mLbuf.lightcolor.y = lightColor.y;
	mLbuf.lightcolor.z = lightColor.z;
	mLbuf.lightcolor.w = lightColor.w;
	mLbuf.camerapos = camera->GetPosition();


	UINT strides = sizeof(Vertex3D);
	UINT offsets = 0;

	manager->mpImContext->IASetInputLayout(shaderNoMotion->mpInputLayout.Get());                            //�`��p�R���e�L�X�g�ɒ��_���C�A�E�g��ݒ�
	manager->mpImContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);                    //�`��p�R���e�L�X�g�̃|���S���`�揇���C���f�b�N�X�����ɐݒ�
	manager->mpImContext->VSSetConstantBuffers(0, 1, shaderNoMotion->mpConstantBufferWorld.GetAddressOf()); //�`��p�R���e�L�X�g�ɒ萔�o�b�t�@��ݒ�

	//�e�N�X�`�����s�N�Z���V�F�[�_�[�ɃZ�b�g
	manager->mpImContext->PSSetShaderResources(0, 1, DX11TEXTURE_GetShaderResourceViewAddr(mMeshData->pSubmeshArray->pTexture));
	manager->mpImContext->PSSetShaderResources(2, 1, shaderNoMotion->mpShadowShaderResourceView.GetAddressOf());

	manager->mpImContext->PSSetSamplers(0, 1, manager->mpSampler.GetAddressOf());                     //�`��p�R���e�L�X�g�Ƀe�N�X�`���T���v���[��ݒ�
	manager->mpImContext->PSSetSamplers(1, 1, shaderNoMotion->mpShadowSampler.GetAddressOf());
	manager->mpImContext->VSSetShader(shaderNoMotion->mpVertexShader.Get(), NULL, 0);                 //�`��p�R���e�L�X�g�ɒ��_�V�F�[�_��ݒ�
	manager->mpImContext->RSSetViewports(1, &manager->mViewport);                                     //�`��p�R���e�L�X�g�Ƀr���[�|�[�g��ݒ�
	manager->mpImContext->RSSetState(shaderNoMotion->mpRasterState.Get());                            //�`��p�R���e�L�X�g�Ƀ��X�^���C�U��ݒ�

	// �e����
	if (mDrawShadow == true) manager->mpImContext->PSSetShader(shaderNoMotion->mpShadowPixelShader.Get(), NULL, 0); //�`��p�R���e�L�X�g�Ƀs�N�Z���V�F�[�_��ݒ�
	// �e�Ȃ�
	else manager->mpImContext->PSSetShader(shaderNoMotion->mpPixelShader.Get(), NULL, 0);                           //�`��p�R���e�L�X�g�Ƀs�N�Z���V�F�[�_��ݒ�
		
	// ToonMap
	ToonMap_SetShaderResourceView();


	//���݂̒��_���C�A�E�g�A���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�e�N�X�`���Ȃǂ̐ݒ�����ɁA
	//��1�����̐��������_���쐬���A�|���S����`�悷��

	//�ގ����ʂɕ`�悷�邽�߁A�T�u���b�V���񃋁[�v
	for (int i = 0; i < mMeshData->numSubmesh; i++) {

		//�`�悵�悤�Ƃ��Ă�����̂̃}�e���A���f�[�^��o�^
		mLbuf.diffuse.x = mMeshData->pSubmeshArray[i].material.diffuse[0];
		mLbuf.diffuse.y = mMeshData->pSubmeshArray[i].material.diffuse[1];
		mLbuf.diffuse.z = mMeshData->pSubmeshArray[i].material.diffuse[2];
		mLbuf.diffuse.w = 1.0f;
		mLbuf.ambient.x = mMeshData->pSubmeshArray[i].material.ambient[0];
		mLbuf.ambient.y = mMeshData->pSubmeshArray[i].material.ambient[1];
		mLbuf.ambient.z = mMeshData->pSubmeshArray[i].material.ambient[2];
		mLbuf.ambient.w = 1.0f;
		mLbuf.specular.x = mMeshData->pSubmeshArray[i].material.specular[0];
		mLbuf.specular.y = mMeshData->pSubmeshArray[i].material.specular[1];
		mLbuf.specular.z = mMeshData->pSubmeshArray[i].material.specular[2];
		mLbuf.specular.w = mMeshData->pSubmeshArray[i].material.specular[3];

		//�`��p�R���e�L�X�g�ɍ����������W�ϊ��s���o�^
		if (shaderNoMotion->mpConstantBufferLight.Get() != NULL) {

			manager->mpImContext->UpdateSubresource(shaderNoMotion->mpConstantBufferLight.Get(), 0, NULL, &mLbuf, 0, 0);
		}

		//���C�e�B���O�萔�̓s�N�Z���V�F�[�_�ɑ��M
		manager->mpImContext->PSSetConstantBuffers(0, 1, shaderNoMotion->mpConstantBufferLight.GetAddressOf());          //�`��p�R���e�L�X�g�ɒ萔�o�b�t�@��ݒ�

		manager->mpImContext->IASetVertexBuffers(0, 1, &mMeshData->pSubmeshArray[i].pVertexBuffer, &strides, &offsets);  //�`��p�R���e�L�X�g�ɒ��_�o�b�t�@��ݒ�
		manager->mpImContext->IASetIndexBuffer(mMeshData->pSubmeshArray[i].pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);       //�`��p�R���e�L�X�g�ɃC���f�b�N�X�o�b�t�@��ݒ�
		manager->mpImContext->DrawIndexed(mMeshData->pSubmeshArray[i].numIndex, 0, 0);
	}
}

void GameObject::FbxDraw(void)
{
	Camera* camera = GetCamera();
	Light*  light = GetLight();


	XMMATRIX worldMatrix = XMMatrixTranslation(mPos.x, mPos.y, mPos.z);

	//���[���h���W�̍쐬 ��World�\���̒�`���������Ċp�x���ݒ�ł���悤�ɂ���̂�����
	//-------------------------------------------
	XMMATRIX worldScale = XMMatrixScaling(mScl.x, mScl.y, mScl.z);
	XMMATRIX matrixRotX = XMMatrixRotationX(XMConvertToRadians(mRot.x));
	XMMATRIX matrixRotY = XMMatrixRotationY(XMConvertToRadians(mRot.y));
	XMMATRIX matrixRotZ = XMMatrixRotationZ(XMConvertToRadians(mRot.z));
	worldMatrix = worldScale * mMeshData->baseRot * matrixRotX * matrixRotY * matrixRotZ * worldMatrix;
	//-------------------------------------------


	//�r���[���W�̍쐬
	//-------------------------------------------
	// �J����
	XMMATRIX viewMatrix_c = camera->GetViewMatrix();
	// ���C�g
	XMMATRIX viewMatrix_l = light->GetViewMatrix();
	//-------------------------------------------

	//�v���W�F�N�V�������W
	//-------------------------------------------
	// �J����
	XMMATRIX projMatrix_c = camera->GetProjectionMatrix();
	// ���C�g
	XMMATRIX projMatrix_l = light->GetProjectionMatrix();
	//-------------------------------------------

	//�S�Ă̕ϊ��s����R���e�L�X�g�ɐݒ�
	CameraBuffer cb;  

	XMStoreFloat4x4(&cb.world,        XMMatrixTranspose(worldMatrix));  //�v��cd.world      = XMMatrixTranspose(worldMatrix);
	XMStoreFloat4x4(&cb.view_c,       XMMatrixTranspose(viewMatrix_c)); //�v��cd.view       = XMMatrixTranspose(viewMatrix);
	XMStoreFloat4x4(&cb.view_l,       XMMatrixTranspose(viewMatrix_l)); //�v��cd.view       = XMMatrixTranspose(viewMatrix);
	XMStoreFloat4x4(&cb.projection_c, XMMatrixTranspose(projMatrix_c)); //�v��cd.projection = XMMatrixTranspose(projMatrix);
	XMStoreFloat4x4(&cb.projection_l, XMMatrixTranspose(projMatrix_l)); //�v��cd.projection = XMMatrixTranspose(projMatrix);

	// �V�F�[�_�[�`��i�e�j�e���ɕ`��
	//ShadowShaderDraw(cb);

	// �V�F�[�_�[�`��
	if (mMeshData->motion == true) FbxShaderDraw(cb);
	else FbxNoMotionShaderDraw(cb);

}

void GameObject::FbxLoader(const char filename[])
{
	FbxManager* fbx_manager;
	FbxImporter* fbx_importer;
	FbxScene* fbx_scene;

	//FBX�̃h���C�o�݂����Ȃ���
	//FbxManager�FFBX��ǂݍ��ނ��߂ɕK�v�Ȃ��̂����܂Ƃ߂邽�߂̃N���X
	//FbxManager::Create�֐��@FbxManager�̏�����
	fbx_manager = FbxManager::Create();

	if (fbx_manager == nullptr) {

		return;
	}

	//FILE *fp�݂����Ȃ���
	//FbxImporter�FFBX���̒��_�f�[�^�A�A�j���[�V�����f�[�^�Ȃǂ𕪉�����N���X
	//FbxImporter::Create�֐��@FbxImporter�̏�����
	//�������F�e�ƂȂ�FbxManager�̃|�C���^�@�������F�C���|�[�^�[�̖��O�i�C�Ӂj
	fbx_importer = FbxImporter::Create(fbx_manager, "importtest");

	if (fbx_manager == nullptr) {

		fbx_manager->Destroy();
		return;
	}

	//zip�t�@�C���̉𓀂݂����Ȃ���
	//FbxScene�F�������ꂽ�e�f�[�^��ۊǂ���N���X
	//FbxScene::Create�֐��@FbxScene�̏�����
	//�������F�e�ƂȂ�FbxManager�̃|�C���^�@�������F�V�[���̖��O�i�C�Ӂj
	fbx_scene = FbxScene::Create(fbx_manager, "scenetest");

	if (fbx_manager == nullptr) {

		fbx_manager->Destroy();
		fbx_importer->Destroy();
		return;
	}

	//�C���|�[�^�[�Ńt�@�C�����J��
	//FbxImporter::Initialize�֐��@FBX�t�@�C�����J���@fopen�Ɠ������ʁ@���̎�FBX���̃f�[�^���o�������
	//�������F�t�@�C���p�X�i�t�@�C���p�X�ɓ��{�ꂪ�܂܂��ƃ_�����ۂ��@���Ӂj
	if (fbx_importer->Initialize(filename) == FALSE) { //fp=fopen();

		fbx_scene->Destroy();
		fbx_importer->Destroy();
		fbx_manager->Destroy();
		return;
	}

	//��قǂ΂炵���f�[�^���V�[���ɓǂݍ��܂���
	//FbxImporter::Import�֐��@�ǂݍ��񂾃f�[�^���V�[���Ɏ󂯓n��
	//�������F�󂯓n���V�[���N���X
	if (fbx_importer->Import(fbx_scene) == FALSE) {

		fbx_scene->Destroy();
		fbx_importer->Destroy();
		fbx_manager->Destroy();
		return;
	}

	//��H�߂̂Ă��؂����������
	//fbx�͒��_�A�C���f�b�N�X�A�A�j���[�V�����f�[�^�Ȃǂ̓����f�[�^�i�m�[�h�j���c���[�\���ŕێ����Ă���
	//���̂��ߑS�Ẵm�[�h����������K�v������
	//FbxScene::GetRootNode�֐��@�V�[���Ɋi�[���ꂽ�f�[�^�i�m�[�h�j�̂����A���ׂĂ̐e�ƂȂ�m�[�h��ǂݍ���
	FbxNode* root_node = fbx_scene->GetRootNode();

	if (root_node == nullptr) {

		fbx_scene->Destroy();
		fbx_importer->Destroy();
		fbx_manager->Destroy();
		return;
	}

	//�����Ń��f���|���S�����O�p�|���S���ɓ��ꂷ��
	//FbxGeometryConverter::Triangulate�֐��@�|���S�����O�p�|���S���ɕϊ�����
	//�������F�K������V�[���@�������F�ϊ��t���O�i��{TRUE�j
	FbxGeometryConverter converter(fbx_manager);
	converter.Triangulate(fbx_scene, TRUE);

	//���b�V���f�[�^���ގ����Ƃɕ������Ă���
	converter.SplitMeshesPerMaterial(fbx_scene, TRUE);


	//�A�j���[�V�����Ɋւ���f�[�^�̎擾
	FbxArray<FbxString*> animation_names;
	fbx_scene->FillAnimStackNameArray(animation_names);//�m�[�h����A�j���[�V�����̗v�f��T���Ă���

	if (animation_names.GetCount() != 0) {

		//�A�j���ԍ�0�Ԃ�����FbxTakeInfo���擾
		auto take_info = fbx_scene->GetTakeInfo(animation_names[0]->Buffer());

		//���Ԋ֘A�̃f�[�^�擾
		mStartTime = take_info->mLocalTimeSpan.GetStart().Get();
		mEndTime = take_info->mLocalTimeSpan.GetStop().Get();
		FbxLongLong oneFrameValue = FbxTime::GetOneFrameValue(FbxTime::eFrames60);
		FbxTime t = (mEndTime - mStartTime) / oneFrameValue;   //�P�ʂ͕b�I�I
		mFrameTime.SetTime(0, 0, 0, 1, 0, fbx_scene->GetGlobalSettings().GetTimeMode());

		//�A�j���f�[�^���g���₷���`���ɂ��ēo�^
		double start = mStartTime.GetSecondDouble();
		double end = mEndTime.GetSecondDouble();

		mFrame = (int)(end * 60);

		mMeshData->animator.startframe = (int)(start * 60);
		mMeshData->animator.endframe = (int)(end * 60);

		mMeshData->motion = true;
	}



	//FBX�Ɋ܂܂��}�e���A���̐����擾
	mMeshData->numSubmesh = fbx_scene->GetSrcObjectCount<FbxMesh>(); //subMesh�̌����킩��
	mMeshData->pSubmeshArray = (SubMesh*)malloc(sizeof(SubMesh) * mMeshData->numSubmesh);

	//���b�V�����ƂɃ��[�v
	for (int i = 0; i < mMeshData->numSubmesh; i++) {

		//�e��f�[�^�擾
		GetMeshData(i, fbx_scene);
	}


	fbx_scene->Destroy();
	fbx_importer->Destroy();
	fbx_manager->Destroy();
	return;
}

void GameObject::GetMeshData(int target, FbxScene* scene)
{
	//�A�g���r���[�g����f�[�^�擾
	FbxMesh* mesh = scene->GetSrcObject<FbxMesh>(target);

	//���_���擾
	mMeshData->pSubmeshArray[target].numVertex = mesh->GetControlPointsCount();

	//���_������K�v�ȕ��������_�ʒu�f�[�^�̗̈�m��
	Vertex3D* verbuf;
	verbuf = (Vertex3D*)malloc(sizeof(Vertex3D) * mMeshData->pSubmeshArray[target].numVertex);

	//���_�f�[�^�̂����A�E�F�C�g����-1�ŏ��������Ă���
	for (int j = 0; j < mMeshData->pSubmeshArray[target].numVertex; j++) {

		verbuf[j].Vweight.bone[0] = -1;
		verbuf[j].Vweight.bone[1] = -1;
		verbuf[j].Vweight.bone[2] = -1;
		verbuf[j].Vweight.bone[3] = -1;
		verbuf[j].Vweight.value[0] = 0.0f;
		verbuf[j].Vweight.value[1] = 0.0f;
		verbuf[j].Vweight.value[2] = 0.0f;
		verbuf[j].Vweight.value[3] = 0.0f;
	}

	//�C���f�b�N�X���擾
	int* indices = mesh->GetPolygonVertices();
	mMeshData->pSubmeshArray[target].numIndex = mesh->GetPolygonVertexCount();

	//�C���f�b�N�X�̈�̊m��
	unsigned short* indbuf;
	indbuf = (unsigned short*)malloc(sizeof(unsigned short) * mMeshData->pSubmeshArray[target].numIndex);

	//�S�̂̃��[���h���W���擾����
	FbxNode* n = mesh->GetNode();

	FbxDouble3 translation = n->LclTranslation.Get();
	FbxDouble3 rotation = n->LclRotation.Get();
	FbxDouble3 scaling = n->LclScaling.Get();

	mMeshData->baseRot = XMMatrixRotationX(XMConvertToRadians(rotation.mData[0]));
	mMeshData->baseRot *= XMMatrixRotationY(XMConvertToRadians(rotation.mData[1]));
	mMeshData->baseRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.mData[2]));

	//���_�f�[�^��荞��
	for (int i = 0; i < mMeshData->pSubmeshArray[target].numVertex; i++) {

		//���_�ʒu
		verbuf[i].pos[0] = (float)mesh->GetControlPointAt(i)[0]; // x
		verbuf[i].pos[1] = (float)mesh->GetControlPointAt(i)[1]; // y
		verbuf[i].pos[2] = (float)mesh->GetControlPointAt(i)[2]; // z

		//���_�J���[�i�_�~�[�j
		verbuf[i].color[0] = 0.0f;
		verbuf[i].color[1] = 0.0f;
		verbuf[i].color[2] = 0.0f;
		verbuf[i].color[3] = 0.5f; // �A���t�@�l�@������Ɠ�����
	}

	//�@���擾�@�C���f�b�N�X��
	FbxArray<FbxVector4> normals;
	mesh->GetPolygonVertexNormals(normals);

	//UV�摜���擾
	FbxStringList uvsetName;
	mesh->GetUVSetNames(uvsetName);

	//UV���W�擾
	FbxArray<FbxVector2> uvsets;
	mesh->GetPolygonVertexUVs(uvsetName.GetStringAt(0), uvsets);

	int t = normals.Size();
	//�@���AUV�f�[�^��荞��
	for (int i = 0; i < normals.Size(); i++) {

		verbuf[indices[i]].normal[0] = normals[i][0];
		verbuf[indices[i]].normal[1] = normals[i][1];
		verbuf[indices[i]].normal[2] = normals[i][2];

		verbuf[indices[i]].tex[0] = uvsets[i][0];
		verbuf[indices[i]].tex[1] = 1.0f - uvsets[i][1];

	}

	//�C���f�b�N�X�f�[�^�쐬
	for (int i = 0; i < mMeshData->pSubmeshArray[target].numIndex / 3; i++) {

		indbuf[i * 3] = indices[i * 3 + 2];
		indbuf[i * 3 + 1] = indices[i * 3 + 1];
		indbuf[i * 3 + 2] = indices[i * 3];
	}

	//�E�F�C�g�f�[�^�擾
	FbxSkin* skin = static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));

	if (skin != NULL) {

		//�{�[�������擾
		mMeshData->boneNum = skin->GetClusterCount();

		//�{�[���\���̗̂̈�m��
		mMeshData->animator.modelBone = (Bone*)malloc(sizeof(Bone) * mMeshData->boneNum);

		//�A�j���z����{�[�������m��
		mMeshData->animator.matrix = (XMMATRIX**)malloc(sizeof(XMMATRIX*) * mMeshData->boneNum);

		//�{�[�����������[�v
		for (int i = 0; i < mMeshData->boneNum; i++) {

			//�N���X�^�i�e�����_�̂����܂�j�擾
			FbxCluster* cluster = skin->GetCluster(i);

			//�S�̂̃��[���h�ϊ��s����擾
			FbxVector4 t0 = n->GetGeometricTranslation(FbxNode::eSourcePivot);
			FbxVector4 r0 = n->GetGeometricRotation(FbxNode::eSourcePivot);
			FbxVector4 s0 = n->GetGeometricScaling(FbxNode::eSourcePivot);
			FbxAMatrix geometryOffset = FbxAMatrix(t0, r0, s0);

			//�N���X�^�ɑ��݂��钸�_�����擾
			int pointnum = cluster->GetControlPointIndicesCount();

			for (int j = 0; j < pointnum; j++) {

				//���_�ԍ��擾
				int pointindex = cluster->GetControlPointIndices()[j];

				//���݃{�[���ii�j�ɑ΂���E�F�C�g�̒l���擾
				double weight = cluster->GetControlPointWeights()[j];

				//�e���{�[���A�E�F�C�g�ʂ�o�^
				for (int k = 0; k < 4; k++) {

					if (verbuf[pointindex].Vweight.bone[k] == -1) {

						verbuf[pointindex].Vweight.bone[k] = i;
						verbuf[pointindex].Vweight.value[k] = (float)weight;
						break;
					}
				}

			}

			//���[�V�����擾

			if (mFrame != 0) {

				//�e�{�[���ɂ�����e�t���[�����Ƃ̕ϊ��s���ۑ����邽�߂̗̈�m��
				mMeshData->animator.matrix[i] = (XMMATRIX*)malloc(sizeof(XMMATRIX) * mFrame);

				//�ꎞ�ϐ�
				FbxMatrix vertexTransformMatrix;
				FbxAMatrix referenceGlobalInitPosition;
				FbxAMatrix clusterGlobalInitPosition;
				FbxMatrix clusterGlobalCurrentPosition;
				FbxMatrix clusterRelativeInitPosition;
				FbxMatrix clusterRelativeCurrentPositionInverse;

				//�e�t���[�������[�v
				for (int o = 0; o < mFrame; o++) {

					//fbx���ɂ����鎞�Ԍv�Z
					FbxTime timeCount = mStartTime + mFrameTime * o;
					if (timeCount > mEndTime) timeCount = mStartTime;

					//���t���[���ɂ����郏�[���h�ϊ��s����擾
					FbxMatrix globalPosition = n->EvaluateGlobalTransform(timeCount);

					//�N�_�i���_�j�̏����p���s��擾
					cluster->GetTransformMatrix(referenceGlobalInitPosition);

					//�N�_�i���_�j�����[���h���W�Ɉړ�
					referenceGlobalInitPosition *= geometryOffset;

					//�{�[���̏����p���s��擾
					cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

					//�{�[���̌��t���[���ɂ�����ʒu�s����擾
					clusterGlobalCurrentPosition = cluster->GetLink()->EvaluateGlobalTransform(timeCount);

					//�{�[�������[���h���W�Ɉړ�
					clusterRelativeInitPosition = clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition;

					//�{�[�����A�j���[�V�����̋����ɏ]���ړ�
					clusterRelativeCurrentPositionInverse = globalPosition.Inverse() * clusterGlobalCurrentPosition;

					//���[���h���W�ɂ�����ŏI�ʒu�ֈړ�
					vertexTransformMatrix = clusterRelativeCurrentPositionInverse * clusterRelativeInitPosition;

					//���ʂɈ�����XMMATRIX�ֈڂ��ւ���
					mMeshData->animator.matrix[i][o].r[0].m128_f32[0] = vertexTransformMatrix.mData[0].mData[0];
					mMeshData->animator.matrix[i][o].r[0].m128_f32[1] = vertexTransformMatrix.mData[0].mData[1];
					mMeshData->animator.matrix[i][o].r[0].m128_f32[2] = vertexTransformMatrix.mData[0].mData[2];
					mMeshData->animator.matrix[i][o].r[0].m128_f32[3] = vertexTransformMatrix.mData[0].mData[3];
					mMeshData->animator.matrix[i][o].r[1].m128_f32[0] = vertexTransformMatrix.mData[1].mData[0];
					mMeshData->animator.matrix[i][o].r[1].m128_f32[1] = vertexTransformMatrix.mData[1].mData[1];
					mMeshData->animator.matrix[i][o].r[1].m128_f32[2] = vertexTransformMatrix.mData[1].mData[2];
					mMeshData->animator.matrix[i][o].r[1].m128_f32[3] = vertexTransformMatrix.mData[1].mData[3];
					mMeshData->animator.matrix[i][o].r[2].m128_f32[0] = vertexTransformMatrix.mData[2].mData[0];
					mMeshData->animator.matrix[i][o].r[2].m128_f32[1] = vertexTransformMatrix.mData[2].mData[1];
					mMeshData->animator.matrix[i][o].r[2].m128_f32[2] = vertexTransformMatrix.mData[2].mData[2];
					mMeshData->animator.matrix[i][o].r[2].m128_f32[3] = vertexTransformMatrix.mData[2].mData[3];
					mMeshData->animator.matrix[i][o].r[3].m128_f32[0] = vertexTransformMatrix.mData[3].mData[0];
					mMeshData->animator.matrix[i][o].r[3].m128_f32[1] = vertexTransformMatrix.mData[3].mData[1];
					mMeshData->animator.matrix[i][o].r[3].m128_f32[2] = vertexTransformMatrix.mData[3].mData[2];
					mMeshData->animator.matrix[i][o].r[3].m128_f32[3] = vertexTransformMatrix.mData[3].mData[3];

				}
			}
		}
	}

	else {

		mMeshData->baseRot = XMMatrixScaling(scaling.mData[0], scaling.mData[1], scaling.mData[2]) * mMeshData->baseRot;
	}

	//�ގ��f�[�^�̎擾
	//�ގ����������[�v
	
	//�}�e���A�����擾
	FbxSurfaceMaterial* material = n->GetMaterial(target);

	if (material != NULL) { // �}�e���A���Q�߂���Ƃ΂����
		//�ގ��f�[�^�̓����o�[�g���H
		if (material->GetClassId().Is(FbxSurfaceLambert::ClassId)) {

			//�����o�[�g�Ȃ烉���o�[�g�p�f�[�^��ǂݏo��
			GetMaterialLambert(mMeshData->pSubmeshArray[target], material);
		}

		//�����o�[�g�łȂ��t�H��������
		else if (material->GetClassId().Is(FbxSurfacePhong::ClassId)) {

			//�t�H���̓����o�[�g�̏�ʌ݊��Ȃ̂ŁA�Ƃ肠���������o�[�g�f�[�^�ǂݏo��
			GetMaterialLambert(mMeshData->pSubmeshArray[target], material);

			//�}�e���A�����t�H���̃f�[�^�Ƃ��Ĉ���
			FbxSurfacePhong* phong = (FbxSurfacePhong*)material;

			FbxProperty prop;

			// �X�y�L�����[
			prop = phong->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (prop.IsValid()) {

				mMeshData->pSubmeshArray[target].material.specular[0] = prop.Get<FbxDouble3>()[0];
				mMeshData->pSubmeshArray[target].material.specular[1] = prop.Get<FbxDouble3>()[1];
				mMeshData->pSubmeshArray[target].material.specular[2] = prop.Get<FbxDouble3>()[2];
			}

		}
	}
	


	DirectX11Manager* manager = GetDirectX11Manager();

	//���_�o�b�t�@�쐬
	//���_�o�b�t�@ : 1�̃��f���̒��_�̈ʒu���W��ێ�����o�b�t�@
	//D3D11_BUFFER_DESC : DirectX11��œ��삷��o�b�t�@(�f�[�^�ێ��p�̍\����)�@BindFlags��"���Ɏg����"��ݒ肷�邱�Ƃŗp�r���ς��

	D3D11_BUFFER_DESC hBufferDesc;                                                    //���_�o�b�t�@���L�^���邽�߂̃o�b�t�@
	hBufferDesc.ByteWidth = sizeof(Vertex3D) * mMeshData->pSubmeshArray[target].numVertex;    //�o�b�t�@�̑��T�C�Y�@����͓ǂ݂��������_�����̃f�[�^���m��
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;                                          //�o�b�t�@�ւ̃A�N�Z�X���@D3D11_USAGE_DEFAULT�Ŗ��Ȃ�
	hBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;                                 //�o�b�t�@���ǂ��g�����@D3D11_BIND_VERTEX_BUFFER�Œ��_�o�b�t�@�Ƃ��Ĉ����悤�ɂȂ�
	hBufferDesc.CPUAccessFlags = 0;                                                   //CPU�A�N�Z�X���@0�͕s�v
	hBufferDesc.MiscFlags = 0;                                                        //���̑��I�v�V�����@0�ŕs�v
	hBufferDesc.StructureByteStride = 0;                                              //�o�b�t�@�̑��T�C�Y�@���J�ɂ����Ȃ�ByteWidth�@0�ł��������o�����̂Ŗ��͂Ȃ�

	D3D11_SUBRESOURCE_DATA hSubResourceData;                 //D3D11_SUBRESOURCE_DATA : �o�b�t�@�쐬���̐ݒ�l���L�^����\����
	hSubResourceData.pSysMem = verbuf;                       //�Q�Ƃ��钸�_�f�[�^�������Ă�����̕�
	hSubResourceData.SysMemPitch = 0;                        //�o�b�t�@���e�N�X�`���Ƃ��Ďg�p����ۂ�0�ȊO������
	hSubResourceData.SysMemSlicePitch = 0;                   //�o�b�t�@���e�N�X�`���Ƃ��Ďg�p����ۂ�0�ȊO������


	//(ID3D11Device*�^�ϐ�)->CreateBuffer
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ���o�b�t�@���쐬����
	//��1����:�ݒ�l���i�[���ꂽ�o�b�t�@�@��2����:D3D11_SUBRESOURCE_DATA�̃A�h���X�@��3����:�쐬���ꂽ�o�b�t�@�̊i�[��

	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&hBufferDesc, &hSubResourceData, &mMeshData->pSubmeshArray[target].pVertexBuffer))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateBuffer1"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//�C���f�b�N�X�o�b�t�@�쐬
	//�C���f�b�N�X�o�b�t�@ : �e���_�̌q����̊֌W����ێ�����o�b�t�@

	D3D11_BUFFER_DESC ibDesc;                                       //�C���f�b�N�X�o�b�t�@���L�^���邽�߂̃o�b�t�@
	ibDesc.ByteWidth = sizeof(unsigned short) * mMeshData->pSubmeshArray[target].numIndex;     //�C���f�b�N�X�o�b�t�@�̃T�C�Y
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;                     //�o�b�t�@���ǂ��g�����@D3D11_BIND_INDEX_BUFFER�ŃC���f�b�N�X�o�b�t�@�Ƃ��Ĉ����悤�ɂȂ�
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA irData;                                  //D3D11_SUBRESOURCE_DATA : �o�b�t�@�쐬���̐ݒ�l���L�^����\����
	irData.pSysMem = indbuf;                                        //�Q�Ƃ���C���f�b�N�X�f�[�^�������Ă�����̕�
	irData.SysMemPitch = 0;
	irData.SysMemSlicePitch = 0;


	//�C���f�b�N�X�o�b�t�@���쐬
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&ibDesc, &irData, &mMeshData->pSubmeshArray[target].pIndexBuffer))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateBuffer2"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	mMeshData->pSubmeshArray[target].material.diffuse[0] = 1.0f;
	mMeshData->pSubmeshArray[target].material.diffuse[1] = 1.0f;
	mMeshData->pSubmeshArray[target].material.diffuse[2] = 1.0f;
	mMeshData->pSubmeshArray[target].material.ambient[0] = 1.0f;
	mMeshData->pSubmeshArray[target].material.ambient[1] = 1.0f;
	mMeshData->pSubmeshArray[target].material.ambient[2] = 1.0f;
	mMeshData->pSubmeshArray[target].material.specular[0] = 1.0f;
	mMeshData->pSubmeshArray[target].material.specular[1] = 1.0f;
	mMeshData->pSubmeshArray[target].material.specular[2] = 1.0f;
	mMeshData->pSubmeshArray[target].material.specular[3] = 1.0f;


	// �������
	free(verbuf);
	free(indbuf);
}

void GameObject::FbxSetPose(int frame)
{
	XMMATRIX mat;
	XMVECTOR buf;

	int target = frame;

	//�e�{�[�����[�v
	for (int i = 0; i < mMeshData->boneNum; i++) {

		//mat = XMMatrixTranslationFromVector(fbxmodelbone[i].positionVector);
		//anibuf.animematrix[i] = XMMatrixInverse(&buf, fbxmodelbone[i].boneinit);

		mMeshData->animator.animeBuffer.animematrix[i] = mMeshData->animator.matrix[i][target];

		/*for (int j = i; j < bonenum; j++) {
			anibuf.animematrix[j] = anibuf.animematrix[i];
		}*/

	}

	//�Ō�ɓ]�u�s��ɂ��ăV�F�[�_�ɓn��
	for (int i = 0; i < mMeshData->boneNum; i++) {
		mMeshData->animator.animeBuffer.animematrix[i] = XMMatrixTranspose(mMeshData->animator.animeBuffer.animematrix[i]);
	}

}

void GameObject::GetMaterialLambert(SubMesh sm, FbxSurfaceMaterial* mati)
{
	FbxProperty prop; //HRESULT�݂����Ȃ�[��?

	//�}�e���A���������o�[�g�̃f�[�^�Ƃ��Ĉ���
	FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)mati;

	// �A���r�G���g
	prop = lambert->FindProperty(FbxSurfaceMaterial::sAmbient);
	if (prop.IsValid()) {

		sm.material.ambient[0] = prop.Get<FbxDouble3>()[0];
		sm.material.ambient[1] = prop.Get<FbxDouble3>()[1];
		sm.material.ambient[2] = prop.Get<FbxDouble3>()[2];
	}

	// �f�B�q���[�Y
	prop = lambert->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (prop.IsValid()) {

		sm.material.diffuse[0] = prop.Get<FbxDouble3>()[0];
		sm.material.diffuse[1] = prop.Get<FbxDouble3>()[1];
		sm.material.diffuse[2] = prop.Get<FbxDouble3>()[2];
	}
}


//-------------------------------------
// �e
//-------------------------------------

void GameObject::ShadowShaderDraw(CameraBuffer cb)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shader = GetShader();


	//�`��p�R���e�L�X�g�ɍ����������W�ϊ��s���o�^
	if (shader->mpShadowConstantBufferWorld.Get() != NULL) {
		manager->mpImContext->UpdateSubresource(shader->mpShadowConstantBufferWorld.Get(), 0, NULL, &cb, 0, 0);
	}


	//������
	for (int i = 0; i < mMeshData->boneNum; i++) {
		mMeshData->animator.animeBuffer.animematrix[i] = XMMatrixIdentity();
	}


	mMeshData->count++;
	if (mMeshData->count == mMeshData->animator.endframe) mMeshData->count = mMeshData->animator.startframe;
	if (mMeshData->count < 0) mMeshData->count = 0;
	/*mMeshData->animator.animeBuffer = FbxSetPose(mMeshData->count);*/
	FbxSetPose(mMeshData->count);


	//�`��p�R���e�L�X�g�ɍ����������W�ϊ��s���o�^
	if (shader->mpShadowConstantBufferAnime.Get() != NULL) {
		manager->mpImContext->UpdateSubresource(shader->mpShadowConstantBufferAnime.Get(), 0, NULL, &mMeshData->animator.animeBuffer, 0, 0);
	}


	UINT strides = sizeof(Vertex3D);
	UINT offsets = 0;

	manager->mpImContext->IASetInputLayout(shader->mpShadowInputLayout.Get());                      //�`��p�R���e�L�X�g�ɒ��_���C�A�E�g��ݒ�
	manager->mpImContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);      //�`��p�R���e�L�X�g�̃|���S���`�揇���C���f�b�N�X�����ɐݒ�

	manager->mpImContext->VSSetConstantBuffers(0, 1, shader->mpShadowConstantBufferWorld.GetAddressOf()); //�`��p�R���e�L�X�g�ɒ萔�o�b�t�@��ݒ�
	manager->mpImContext->VSSetConstantBuffers(1, 1, shader->mpShadowConstantBufferAnime.GetAddressOf()); //�`��p�R���e�L�X�g�ɒ萔�o�b�t�@��ݒ�

	//�e�N�X�`�����s�N�Z���V�F�[�_�[�ɃZ�b�g
	//manager->mpImContext->PSSetShaderResources(0, 1, DX11TEXTURE_GetShaderResourceViewAddr(mMeshData->pSubmeshArray->pTexture));
	//manager->mpImContext->PSSetShaderResources(2, 1, shader->mpShadowShaderResourceView.GetAddressOf());
	//ToonMap_SetShaderResourceView();
	//manager->mpImContext->PSSetSamplers(0, 1, manager->mpSampler.GetAddressOf());             //�`��p�R���e�L�X�g�Ƀe�N�X�`���T���v���[��ݒ�
	//manager->mpImContext->PSSetSamplers(1, 1, shader->mpShadowSampler.GetAddressOf());        //�`��p�R���e�L�X�g�Ƀe�N�X�`���T���v���[��ݒ�

	manager->mpImContext->VSSetShader(shader->mpShadowVertexShader.Get(), NULL, 0);       //�`��p�R���e�L�X�g�ɒ��_�V�F�[�_��ݒ�
	manager->mpImContext->PSSetShader(shader->mpShadowPixelShader.Get(), nullptr, 0);     // �`��p�R���e�L�X�g�Ƀs�N�Z���V�F�[�_��ݒ�
	manager->mpImContext->RSSetViewports(1, &shader->mShadowViewport);                    // �`��p�R���e�L�X�g�Ƀr���[�|�[�g��ݒ�                        
	manager->mpImContext->RSSetState(shader->mpShadowRasterState.Get());


	//���݂̒��_���C�A�E�g�A���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�e�N�X�`���Ȃǂ̐ݒ�����ɁA
	//��1�����̐��������_���쐬���A�|���S����`�悷��

	//�ގ����ʂɕ`�悷�邽�߁A�T�u���b�V���񃋁[�v
	for (int i = 0; i < mMeshData->numSubmesh; i++) {

		manager->mpImContext->IASetVertexBuffers(0, 1, &mMeshData->pSubmeshArray[i].pVertexBuffer, &strides, &offsets); //�`��p�R���e�L�X�g�ɒ��_�o�b�t�@��ݒ�
		manager->mpImContext->IASetIndexBuffer(mMeshData->pSubmeshArray[i].pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);      //�`��p�R���e�L�X�g�ɃC���f�b�N�X�o�b�t�@��ݒ�
		manager->mpImContext->DrawIndexed(mMeshData->pSubmeshArray[i].numIndex, 0, 0);
	}
}

void GameObject::ShadowShaderDrawNomotion(CameraBuffer cb)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shaderNoMotion = GetShaderNoMotion();


	//�`��p�R���e�L�X�g�ɍ����������W�ϊ��s���o�^
	if (shaderNoMotion->mpShadowConstantBufferWorld != NULL) {

		manager->mpImContext->UpdateSubresource(shaderNoMotion->mpShadowConstantBufferWorld.Get(), 0, nullptr, &cb, 0, 0);
	}

	UINT strides = sizeof(Vertex3D);
	UINT offsets = 0;
	manager->mpImContext->IASetInputLayout(shaderNoMotion->mpShadowInputLayout.Get());            // �`��p�R���e�L�X�g�ɒ��_���C�A�E�g��ݒ�
	manager->mpImContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);          // �`��p�R���e�L�X�g�̃|���S���`�揇���C���f�b�N�X�����ɐݒ�
	manager->mpImContext->VSSetConstantBuffers(0, 1, shaderNoMotion->mpShadowConstantBufferWorld.GetAddressOf()); // �`��p�R���e�L�X�g�ɒ萔�o�b�t�@��ݒ�

	//�e�N�X�`�����s�N�Z���V�F�[�_�[�ɃZ�b�g
	//DX11_GetImmediateContext()->PSSetShaderResources(0, 1, DX11TEXTURE_GetShaderResourceViewAddr(dc.mesh->pSubmeshArray->pTexture));
	//ToonMap_SetShaderResourceView();                                                            // ToonMap
	//DX11_GetImmediateContext()->PSSetShaderResources(2, 1, &gpShaderResourceView_Shadow);
	//DX11_GetImmediateContext()->PSSetSamplers(0, 1, &g_Dx11Instances->pSampler);                // �`��p�R���e�L�X�g�Ƀe�N�X�`���T���v���[��ݒ�
	//DX11_GetImmediateContext()->PSSetSamplers(1, 1, &gpSampler_Shadow);                         // �`��p�R���e�L�X�g�Ƀe�N�X�`���T���v���[��ݒ�

	manager->mpImContext->VSSetShader(shaderNoMotion->mpShadowVertexShader.Get(), nullptr, 0);    // �`��p�R���e�L�X�g�ɒ��_�V�F�[�_��ݒ�
	manager->mpImContext->PSSetShader(shaderNoMotion->mpShadowPixelShader.Get(), nullptr, 0);     // �`��p�R���e�L�X�g�Ƀs�N�Z���V�F�[�_��ݒ�
	manager->mpImContext->RSSetViewports(1, &shaderNoMotion->mShadowViewport);                    // �`��p�R���e�L�X�g�Ƀr���[�|�[�g��ݒ�
	manager->mpImContext->RSSetState(shaderNoMotion->mpShadowRasterState.Get());



	//���݂̒��_���C�A�E�g�A���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�e�N�X�`���Ȃǂ̐ݒ�����ɁA
	//��1�����̐��������_���쐬���A�|���S����`�悷��
	//�ގ����ʂɕ`�悷�邽�߁A�T�u���b�V���񃋁[�v
	for (int i = 0; i < mMeshData->numSubmesh; i++) {

		manager->mpImContext->IASetVertexBuffers(0, 1, &mMeshData->pSubmeshArray[i].pVertexBuffer, &strides, &offsets);  //�`��p�R���e�L�X�g�ɒ��_�o�b�t�@��ݒ�
		manager->mpImContext->IASetIndexBuffer(mMeshData->pSubmeshArray[i].pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);       //�`��p�R���e�L�X�g�ɃC���f�b�N�X�o�b�t�@��ݒ�
		manager->mpImContext->DrawIndexed(mMeshData->pSubmeshArray[i].numIndex, 0, 0);
	}
}

void GameObject::ShadowDraw()
{
	Camera* camera = GetCamera();
	Light*  light = GetLight();


	XMMATRIX worldMatrix = XMMatrixTranslation(mPos.x, mPos.y, mPos.z);

	//���[���h���W�̍쐬
	//-------------------------------------------
	XMMATRIX worldScale = XMMatrixScaling(mScl.x, mScl.y, mScl.z);
	XMMATRIX matrixRotX = XMMatrixRotationX(XMConvertToRadians(mRot.x));
	XMMATRIX matrixRotY = XMMatrixRotationY(XMConvertToRadians(mRot.y));
	XMMATRIX matrixRotZ = XMMatrixRotationZ(XMConvertToRadians(mRot.z));
	worldMatrix = worldScale * mMeshData->baseRot * matrixRotX * matrixRotY * matrixRotZ * worldMatrix;
	//-------------------------------------------

	//�r���[���W�̍쐬
	//-------------------------------------------
	// �J����
	XMMATRIX viewMatrix_c = camera->GetViewMatrix();
	// ���C�g
	XMMATRIX viewMatrix_l = light->GetViewMatrix();
	//-------------------------------------------

	//�v���W�F�N�V�������W
	//-------------------------------------------
	// �J����
	XMMATRIX projMatrix_c = camera->GetProjectionMatrix();
	// ���C�g
	XMMATRIX projMatrix_l = light->GetProjectionMatrix();
	//-------------------------------------------


	//�S�Ă̕ϊ��s����R���e�L�X�g�ɐݒ�
	CameraBuffer cb;  //�����p�̕ϐ���錾

	//XMStoreFloat4x4�֐�
	//XMFLOAT4X4���󂯓n�����߂̂���
	//��1����:�����̃|�C���^�@��2����:�󂯓n����

	//XMMatrixTranspose�֐�
	//�s���]�u�s��ɂ���

	XMStoreFloat4x4(&cb.world, XMMatrixTranspose(worldMatrix));     //�v��cd.world = XMMatrixTranspose(worldMatrix);
	XMStoreFloat4x4(&cb.view_c, XMMatrixTranspose(viewMatrix_c));   //�v��cd.view = XMMatrixTranspose(viewMatrix);
	XMStoreFloat4x4(&cb.view_l, XMMatrixTranspose(viewMatrix_l));   //�v��cd.view = XMMatrixTranspose(viewMatrix);
	XMStoreFloat4x4(&cb.projection_c, XMMatrixTranspose(projMatrix_c));   //�v��cd.projection = XMMatrixTranspose(projMatrix);
	XMStoreFloat4x4(&cb.projection_l, XMMatrixTranspose(projMatrix_l));   //�v��cd.projection = XMMatrixTranspose(projMatrix);

	// �V�F�[�_�[�`��i�e�j
	if(mMeshData->motion == true) ShadowShaderDraw(cb);
	else ShadowShaderDrawNomotion(cb);
}

void GameObject::ShadowClearScreen()
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shaderNoMotion = GetShaderNoMotion();

	manager->mpImContext->ClearDepthStencilView(shaderNoMotion->mpShadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	manager->mpImContext->OMSetRenderTargets(0, nullptr, shaderNoMotion->mpShadowDepthStencilView.Get()); // �[�x�X�e���V���r���[���e�p��
}


//*************************************
// �֐�
//*************************************

// ID���w�肵�ăI�u�W�F�N�g���擾
GameObject* GetGameObject(int id)
{
	return gpGameObject[id];
}

// ������
void GameObjectInit(void) {

	for (int i = 0; i < MAX_GAMEOBJECT; i++) {
		gpGameObject[i] = new GameObject;
		gpGameObject[i]->Init(i, i);
		gpGameObject[i]->ShadowOnOf(true); // �e�`��OnOf
	}
	

	// �p�����[�^�[�ݒ�
	GameObjectSetParameter();


	//-------------------------------------------
	// �R���|�[�l���g�ǉ�
	//-------------------------------------------
	// �����蔻��
	// �S�[�X�g
	gpColliderComponent[ENEMY_01] = new ColliderComponent;
	gpColliderComponent[ENEMY_01]->SetOwner(gpGameObject[ENEMY_01]);
	gpColliderComponent[ENEMY_01]->InitSphere(gObjectColliderParameter[ENEMY_01].sphereRadius);
	// ��
	gpColliderComponent[TREE_01] = new ColliderComponent;
	gpColliderComponent[TREE_01]->SetOwner(gpGameObject[TREE_01]);
	gpColliderComponent[TREE_01]->InitAabb(gObjectColliderParameter[TREE_01].aabbLength);
	// ��
	gpColliderComponent[TREASURECHEST] = new ColliderComponent;
	gpColliderComponent[TREASURECHEST]->SetOwner(gpGameObject[TREASURECHEST]);
	gpColliderComponent[TREASURECHEST]->InitAabb(gObjectColliderParameter[TREASURECHEST].aabbLength);

}

// �X�V
void GameObjectUpdate(void) {

	// �S�[�X�g�ړ��@�����player�N���X������Ă��̒���
	XMFLOAT3 pos = gpGameObject[ENEMY_01]->PositionGet();
	if (GetKeyboardPress(DIK_W)) {
		pos.z += 1.0f;
	}
	if (GetKeyboardPress(DIK_S)) {
		pos.z -= 1.0f;
	}
	if (GetKeyboardPress(DIK_D)) {
		pos.x += 1.0f;
	}
	if (GetKeyboardPress(DIK_A)) {
		pos.x -= 1.0f;
	}

	gpGameObject[ENEMY_01]->PositionSet(pos.x, pos.y, pos.z);


	//-------------------------------------------
	// �R���|�[�l���g�ǉ�
	//-------------------------------------------
	// �����蔻��
	Map* gridMap = GetGridMap();

	gpColliderComponent[ENEMY_01]->Update(gpColliderComponent[TREE_01]);
	gpColliderComponent[ENEMY_01]->Update(gpColliderComponent[TREASURECHEST]);
	gridMap->Update(gpColliderComponent[ENEMY_01], MAP_TREE_01);
	gpColliderComponent[ENEMY_01]->UpdateBackup();

}

void GameObjectDrawShadow() {

	//-------------------------------------------
	// ��ɃV���h�E�}�b�v�쐬
	//-------------------------------------------
	for (int i = 0; i < MAX_GAMEOBJECT; i++) {
		gpGameObject[i]->ShadowDraw();
	}

}

// �`��
void GameObjectDraw(void) {

	//-------------------------------------------
	// ���f���`��
	//-------------------------------------------
	for (int i = 0; i < MAX_GAMEOBJECT; i++) {
		gpGameObject[i]->Draw();
	}


	//-------------------------------------------
	// �R���|�[�l���g�ǉ�
	//-------------------------------------------
	// �����蔻��
	/*for (int i = 0; i < COLLIDER_NUM; i++) {
		gpColliderComponent[i]->Draw();
	}*/

}

// ���
void GameObjectRelease(void) {

	for (int i = 0; i < MAX_GAMEOBJECT; i++) {
		gpGameObject[i]->Release();
		delete gpGameObject[i];
	}


	//-------------------------------------------
	// �R���|�[�l���g�ǉ�
	//-------------------------------------------
	// �����蔻��
	for (int i = 0; i < COLLIDER_NUM; i++) {
		gpColliderComponent[i]->Release();
		delete gpColliderComponent[i];
	}

}

// �p�����[�^�[�ݒ�
void GameObjectSetParameter(void) {

	// �S�[�X�g
	gpGameObject[ENEMY_01]->PositionSet(0, 10.0f, 0);
	gpGameObject[ENEMY_01]->RotationSet(0, 0, 0);
	gpGameObject[ENEMY_01]->ScaleSet(0.1f, 0.1f, 0.1f);
	gObjectColliderParameter[ENEMY_01].sphereRadius = 60.0f;

	// ��01
	gpGameObject[TREE_01]->PositionSet(60.0f, 0.0f, 20.0f);
	gpGameObject[TREE_01]->RotationSet(0, 0, 0);
	gpGameObject[TREE_01]->ScaleSet(2.0f, 2.0f, 2.0f);
	gObjectColliderParameter[TREE_01].aabbLength = {1.0f, 10.0f, 1.0f};

	// ��
	gpGameObject[TREASURECHEST]->PositionSet(50.0f, 0, 80.0f);
	gpGameObject[TREASURECHEST]->RotationSet(0, 0, 0);
	gpGameObject[TREASURECHEST]->ScaleSet(1.0f, 1.0f, 1.0f);
	gObjectColliderParameter[TREASURECHEST].aabbLength = { 10.0f, 20.0f, 8.0f };

	// �}�b�v01
	gpGameObject[MAP_01]->PositionSet(0, 0, 0);
	gpGameObject[MAP_01]->RotationSet(0, 0, 0);
	gpGameObject[MAP_01]->ScaleSet(1.0f, 1.0f, 1.0f);

	// ��01
	gpGameObject[SKY_01]->PositionSet(0, 0, 0);
	gpGameObject[SKY_01]->RotationSet(0, 0, 0);
	gpGameObject[SKY_01]->ScaleSet(1.0f, 1.0f, 1.0f);

	// ��
	gpGameObject[BOX_01]->PositionSet(-40.0f, 15.0f, -40.0f);
	gpGameObject[BOX_01]->RotationSet(0, 0, 0);
	gpGameObject[BOX_01]->ScaleSet(0.1f, 0.1f, 0.1f);

}

