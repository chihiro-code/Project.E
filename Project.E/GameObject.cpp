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
// マクロ定義
//*************************************
#define COLLIDER_NUM 3


//*************************************
// グローバル変数
//*************************************
GameObject* gpGameObject[MAX_GAMEOBJECT];
ColliderComponent* gpColliderComponent[COLLIDER_NUM];
ColliderParameter gObjectColliderParameter[MAX_GAMEOBJECT];


//*************************************
// GameObject関数
//*************************************

//-------------------------------------
// オブジェクト
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
	// モデルデータ読み込み
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
	// ボーン
	if (mMeshData->motion == true) {
		if (mMeshData->animator.modelBone != nullptr) {
			free(mMeshData->animator.modelBone); // Bone*
			mMeshData->animator.modelBone = nullptr;
		}
	}

	// サブメッシュ
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

	// メッシュ
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
// モデル読み込み
//-------------------------------------

void GameObject::FbxInit(char* fbxPath, char* texturePath)
{
	mMeshData = NULL;
	mMeshData = (Mesh*)malloc(sizeof(Mesh));

	FbxLoader(fbxPath);
	mMeshData->count = 0;

	mMeshData->pSubmeshArray->pTexture = NULL;
	if (mMeshData->pSubmeshArray->pTexture == NULL) {

		mMeshData->pSubmeshArray->pTexture = DX11TEXTURE_CreateAndLoadFile(texturePath);  // テクスチャデータのローダー関数

		if (mMeshData->pSubmeshArray->pTexture == NULL) {
			//作成に失敗したら警告を出してプログラムを強制終了
			MessageBox(NULL, TEXT("Texture"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// モーションありなしでシェーダーを分ける
	FbxShaderInitChoice(mMeshData);

}

void GameObject::FbxShaderInitChoice(Mesh* pMesh)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shader = GetShader();

	// モーションありかなしか
	if (pMesh->motion == true) {
		//アニメーションに関する処理
		//初期化
		for (int i = 0; i < pMesh->boneNum; i++) {
			pMesh->animator.animeBuffer.animematrix[i] = XMMatrixIdentity();
		}

		////初期ポーズセット
		FbxSetPose(0);

		//描画用コンテキストに合成した座標変換行列を登録
		/*if (shader ->mpConstantBufferAnime.Get() != NULL) {
			manager->mpImContext->UpdateSubresource(shader->mpConstantBufferAnime.Get(), 0, NULL, &pMesh->animator.animeBuffer, 0, 0);
		}*/
	}
	else {
		// モーションなしのためNULLを代入
		pMesh->animator.modelBone = nullptr;
	}
}

void GameObject::FbxShaderDraw(CameraBuffer cb)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shader = GetShader();
	Camera* camera = GetCamera();
	Light*  light  = GetLight();


	//描画用コンテキストに合成した座標変換行列を登録
	if (shader->mpConstantBufferWorld.Get() != NULL) {
		manager->mpImContext->UpdateSubresource(shader->mpConstantBufferWorld.Get(), 0, NULL, &cb, 0, 0);
	}


	//初期化
	for (int i = 0; i < mMeshData->boneNum; i++) {
		mMeshData->animator.animeBuffer.animematrix[i] = XMMatrixIdentity();
	}


	mMeshData->count++;
	if (mMeshData->count == mMeshData->animator.endframe) mMeshData->count = mMeshData->animator.startframe;
	if (mMeshData->count < 0) mMeshData->count = 0;
	/*mMeshData->animator.animeBuffer = FbxSetPose(mMeshData->count);*/
	FbxSetPose(mMeshData->count);


	//描画用コンテキストに合成した座標変換行列を登録
	if (shader->mpConstantBufferAnime.Get() != NULL) {
		manager->mpImContext->UpdateSubresource(shader->mpConstantBufferAnime.Get(), 0, NULL, &mMeshData->animator.animeBuffer, 0, 0);
	}


	//ライティング
	//世界の光はここで定義
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

	manager->mpImContext->IASetInputLayout(shader->mpInputLayout.Get());                      //描画用コンテキストに頂点レイアウトを設定
	manager->mpImContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);      //描画用コンテキストのポリゴン描画順をインデックス準拠に設定

	manager->mpImContext->VSSetConstantBuffers(0, 1, shader->mpConstantBufferWorld.GetAddressOf()); //描画用コンテキストに定数バッファを設定
	manager->mpImContext->VSSetConstantBuffers(1, 1, shader->mpConstantBufferAnime.GetAddressOf()); //描画用コンテキストに定数バッファを設定

	//テクスチャをピクセルシェーダーにセット
	manager->mpImContext->PSSetShaderResources(0, 1, DX11TEXTURE_GetShaderResourceViewAddr(mMeshData->pSubmeshArray->pTexture));
	manager->mpImContext->PSSetShaderResources(2, 1, shader->mpShadowShaderResourceView.GetAddressOf());

	manager->mpImContext->PSSetSamplers(0, 1, manager->mpSampler.GetAddressOf());             //描画用コンテキストにテクスチャサンプラーを設定
	manager->mpImContext->PSSetSamplers(1, 1, shader->mpShadowSampler.GetAddressOf());        //描画用コンテキストにテクスチャサンプラーを設定

	manager->mpImContext->VSSetShader(shader->mpVertexShader.Get(), NULL, 0);                 //描画用コンテキストに頂点シェーダを設定

	manager->mpImContext->RSSetViewports(1, &manager->mViewport);                             //描画用コンテキストにビューポートを設定                           
	manager->mpImContext->RSSetState(shader->mpRasterState.Get());

	// 影あり
	if (mDrawShadow == true) manager->mpImContext->PSSetShader(shader->mpShadowPixelShader.Get(), NULL, 0); //描画用コンテキストにピクセルシェーダを設定
	// 影なし
	else manager->mpImContext->PSSetShader(shader->mpPixelShader.Get(), NULL, 0);                           //描画用コンテキストにピクセルシェーダを設定

	// ToonMap
	ToonMap_SetShaderResourceView();

	//現在の頂点レイアウト、頂点バッファ、インデックスバッファ、テクスチャなどの設定を元に、
	//第1引数の数だけ頂点を作成し、ポリゴンを描画する

	//材質を個別に描画するため、サブメッシュ回ループ
	for (int i = 0; i < mMeshData->numSubmesh; i++) {

		//描画しようとしているもののマテリアルデータを登録
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

		//描画用コンテキストに合成した座標変換行列を登録
		if (shader->mpConstantBufferLight.Get() != NULL) {
			manager->mpImContext->UpdateSubresource(shader->mpConstantBufferLight.Get(), 0, NULL, &mLbuf, 0, 0);
		}

		//ライティング定数はピクセルシェーダに送信
		manager->mpImContext->PSSetConstantBuffers(0, 1, shader->mpConstantBufferLight.GetAddressOf());                       //描画用コンテキストに定数バッファを設定

		manager->mpImContext->IASetVertexBuffers(0, 1, &mMeshData->pSubmeshArray[i].pVertexBuffer, &strides, &offsets); //描画用コンテキストに頂点バッファを設定
		manager->mpImContext->IASetIndexBuffer(mMeshData->pSubmeshArray[i].pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);      //描画用コンテキストにインデックスバッファを設定
		manager->mpImContext->DrawIndexed(mMeshData->pSubmeshArray[i].numIndex, 0, 0);
	}
}

void GameObject::FbxNoMotionShaderDraw(CameraBuffer cb)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shaderNoMotion = GetShaderNoMotion();
	Camera* camera = GetCamera();
	Light*  light = GetLight();


	//描画用コンテキストに合成した座標変換行列を登録
	if (shaderNoMotion->mpConstantBufferWorld.Get() != NULL) {

		manager->mpImContext->UpdateSubresource(shaderNoMotion->mpConstantBufferWorld.Get(), 0, NULL, &cb, 0, 0);
	}


	//ライティング
	//世界の光はここで定義
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

	manager->mpImContext->IASetInputLayout(shaderNoMotion->mpInputLayout.Get());                            //描画用コンテキストに頂点レイアウトを設定
	manager->mpImContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);                    //描画用コンテキストのポリゴン描画順をインデックス準拠に設定
	manager->mpImContext->VSSetConstantBuffers(0, 1, shaderNoMotion->mpConstantBufferWorld.GetAddressOf()); //描画用コンテキストに定数バッファを設定

	//テクスチャをピクセルシェーダーにセット
	manager->mpImContext->PSSetShaderResources(0, 1, DX11TEXTURE_GetShaderResourceViewAddr(mMeshData->pSubmeshArray->pTexture));
	manager->mpImContext->PSSetShaderResources(2, 1, shaderNoMotion->mpShadowShaderResourceView.GetAddressOf());

	manager->mpImContext->PSSetSamplers(0, 1, manager->mpSampler.GetAddressOf());                     //描画用コンテキストにテクスチャサンプラーを設定
	manager->mpImContext->PSSetSamplers(1, 1, shaderNoMotion->mpShadowSampler.GetAddressOf());
	manager->mpImContext->VSSetShader(shaderNoMotion->mpVertexShader.Get(), NULL, 0);                 //描画用コンテキストに頂点シェーダを設定
	manager->mpImContext->RSSetViewports(1, &manager->mViewport);                                     //描画用コンテキストにビューポートを設定
	manager->mpImContext->RSSetState(shaderNoMotion->mpRasterState.Get());                            //描画用コンテキストにラスタライザを設定

	// 影あり
	if (mDrawShadow == true) manager->mpImContext->PSSetShader(shaderNoMotion->mpShadowPixelShader.Get(), NULL, 0); //描画用コンテキストにピクセルシェーダを設定
	// 影なし
	else manager->mpImContext->PSSetShader(shaderNoMotion->mpPixelShader.Get(), NULL, 0);                           //描画用コンテキストにピクセルシェーダを設定
		
	// ToonMap
	ToonMap_SetShaderResourceView();


	//現在の頂点レイアウト、頂点バッファ、インデックスバッファ、テクスチャなどの設定を元に、
	//第1引数の数だけ頂点を作成し、ポリゴンを描画する

	//材質を個別に描画するため、サブメッシュ回ループ
	for (int i = 0; i < mMeshData->numSubmesh; i++) {

		//描画しようとしているもののマテリアルデータを登録
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

		//描画用コンテキストに合成した座標変換行列を登録
		if (shaderNoMotion->mpConstantBufferLight.Get() != NULL) {

			manager->mpImContext->UpdateSubresource(shaderNoMotion->mpConstantBufferLight.Get(), 0, NULL, &mLbuf, 0, 0);
		}

		//ライティング定数はピクセルシェーダに送信
		manager->mpImContext->PSSetConstantBuffers(0, 1, shaderNoMotion->mpConstantBufferLight.GetAddressOf());          //描画用コンテキストに定数バッファを設定

		manager->mpImContext->IASetVertexBuffers(0, 1, &mMeshData->pSubmeshArray[i].pVertexBuffer, &strides, &offsets);  //描画用コンテキストに頂点バッファを設定
		manager->mpImContext->IASetIndexBuffer(mMeshData->pSubmeshArray[i].pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);       //描画用コンテキストにインデックスバッファを設定
		manager->mpImContext->DrawIndexed(mMeshData->pSubmeshArray[i].numIndex, 0, 0);
	}
}

void GameObject::FbxDraw(void)
{
	Camera* camera = GetCamera();
	Light*  light = GetLight();


	XMMATRIX worldMatrix = XMMatrixTranslation(mPos.x, mPos.y, mPos.z);

	//ワールド座標の作成 →World構造体定義を改造して角度も設定できるようにするのもあり
	//-------------------------------------------
	XMMATRIX worldScale = XMMatrixScaling(mScl.x, mScl.y, mScl.z);
	XMMATRIX matrixRotX = XMMatrixRotationX(XMConvertToRadians(mRot.x));
	XMMATRIX matrixRotY = XMMatrixRotationY(XMConvertToRadians(mRot.y));
	XMMATRIX matrixRotZ = XMMatrixRotationZ(XMConvertToRadians(mRot.z));
	worldMatrix = worldScale * mMeshData->baseRot * matrixRotX * matrixRotY * matrixRotZ * worldMatrix;
	//-------------------------------------------


	//ビュー座標の作成
	//-------------------------------------------
	// カメラ
	XMMATRIX viewMatrix_c = camera->GetViewMatrix();
	// ライト
	XMMATRIX viewMatrix_l = light->GetViewMatrix();
	//-------------------------------------------

	//プロジェクション座標
	//-------------------------------------------
	// カメラ
	XMMATRIX projMatrix_c = camera->GetProjectionMatrix();
	// ライト
	XMMATRIX projMatrix_l = light->GetProjectionMatrix();
	//-------------------------------------------

	//全ての変換行列をコンテキストに設定
	CameraBuffer cb;  

	XMStoreFloat4x4(&cb.world,        XMMatrixTranspose(worldMatrix));  //要はcd.world      = XMMatrixTranspose(worldMatrix);
	XMStoreFloat4x4(&cb.view_c,       XMMatrixTranspose(viewMatrix_c)); //要はcd.view       = XMMatrixTranspose(viewMatrix);
	XMStoreFloat4x4(&cb.view_l,       XMMatrixTranspose(viewMatrix_l)); //要はcd.view       = XMMatrixTranspose(viewMatrix);
	XMStoreFloat4x4(&cb.projection_c, XMMatrixTranspose(projMatrix_c)); //要はcd.projection = XMMatrixTranspose(projMatrix);
	XMStoreFloat4x4(&cb.projection_l, XMMatrixTranspose(projMatrix_l)); //要はcd.projection = XMMatrixTranspose(projMatrix);

	// シェーダー描画（影）影を先に描画
	//ShadowShaderDraw(cb);

	// シェーダー描画
	if (mMeshData->motion == true) FbxShaderDraw(cb);
	else FbxNoMotionShaderDraw(cb);

}

void GameObject::FbxLoader(const char filename[])
{
	FbxManager* fbx_manager;
	FbxImporter* fbx_importer;
	FbxScene* fbx_scene;

	//FBXのドライバみたいなもん
	//FbxManager：FBXを読み込むために必要なものを取りまとめるためのクラス
	//FbxManager::Create関数　FbxManagerの初期化
	fbx_manager = FbxManager::Create();

	if (fbx_manager == nullptr) {

		return;
	}

	//FILE *fpみたいなもん
	//FbxImporter：FBX内の頂点データ、アニメーションデータなどを分解するクラス
	//FbxImporter::Create関数　FbxImporterの初期化
	//第一引数：親となるFbxManagerのポインタ　第二引数：インポーターの名前（任意）
	fbx_importer = FbxImporter::Create(fbx_manager, "importtest");

	if (fbx_manager == nullptr) {

		fbx_manager->Destroy();
		return;
	}

	//zipファイルの解凍みたいなもん
	//FbxScene：分解された各データを保管するクラス
	//FbxScene::Create関数　FbxSceneの初期化
	//第一引数：親となるFbxManagerのポインタ　第二引数：シーンの名前（任意）
	fbx_scene = FbxScene::Create(fbx_manager, "scenetest");

	if (fbx_manager == nullptr) {

		fbx_manager->Destroy();
		fbx_importer->Destroy();
		return;
	}

	//インポーターでファイルを開く
	//FbxImporter::Initialize関数　FBXファイルを開く　fopenと同じ効果　この時FBX内のデータがバラされる
	//第一引数：ファイルパス（ファイルパスに日本語が含まれるとダメっぽい　注意）
	if (fbx_importer->Initialize(filename) == FALSE) { //fp=fopen();

		fbx_scene->Destroy();
		fbx_importer->Destroy();
		fbx_manager->Destroy();
		return;
	}

	//先ほどばらしたデータをシーンに読み込ませる
	//FbxImporter::Import関数　読み込んだデータをシーンに受け渡す
	//第一引数：受け渡すシーンクラス
	if (fbx_importer->Import(fbx_scene) == FALSE) {

		fbx_scene->Destroy();
		fbx_importer->Destroy();
		fbx_manager->Destroy();
		return;
	}

	//千羽鶴のてっぺんを引っ張る
	//fbxは頂点、インデックス、アニメーションデータなどの内部データ（ノード）をツリー構造で保持している
	//そのため全てのノードを検索する必要がある
	//FbxScene::GetRootNode関数　シーンに格納されたデータ（ノード）のうち、すべての親となるノードを読み込む
	FbxNode* root_node = fbx_scene->GetRootNode();

	if (root_node == nullptr) {

		fbx_scene->Destroy();
		fbx_importer->Destroy();
		fbx_manager->Destroy();
		return;
	}

	//ここでモデルポリゴンを三角ポリゴンに統一する
	//FbxGeometryConverter::Triangulate関数　ポリゴンを三角ポリゴンに変換する
	//第一引数：適応するシーン　第二引数：変換フラグ（基本TRUE）
	FbxGeometryConverter converter(fbx_manager);
	converter.Triangulate(fbx_scene, TRUE);

	//メッシュデータを材質ごとに分解しておく
	converter.SplitMeshesPerMaterial(fbx_scene, TRUE);


	//アニメーションに関するデータの取得
	FbxArray<FbxString*> animation_names;
	fbx_scene->FillAnimStackNameArray(animation_names);//ノードからアニメーションの要素を探している

	if (animation_names.GetCount() != 0) {

		//アニメ番号0番を元にFbxTakeInfoを取得
		auto take_info = fbx_scene->GetTakeInfo(animation_names[0]->Buffer());

		//時間関連のデータ取得
		mStartTime = take_info->mLocalTimeSpan.GetStart().Get();
		mEndTime = take_info->mLocalTimeSpan.GetStop().Get();
		FbxLongLong oneFrameValue = FbxTime::GetOneFrameValue(FbxTime::eFrames60);
		FbxTime t = (mEndTime - mStartTime) / oneFrameValue;   //単位は秒！！
		mFrameTime.SetTime(0, 0, 0, 1, 0, fbx_scene->GetGlobalSettings().GetTimeMode());

		//アニメデータを使いやすい形式にして登録
		double start = mStartTime.GetSecondDouble();
		double end = mEndTime.GetSecondDouble();

		mFrame = (int)(end * 60);

		mMeshData->animator.startframe = (int)(start * 60);
		mMeshData->animator.endframe = (int)(end * 60);

		mMeshData->motion = true;
	}



	//FBXに含まれるマテリアルの数を取得
	mMeshData->numSubmesh = fbx_scene->GetSrcObjectCount<FbxMesh>(); //subMeshの個数がわかる
	mMeshData->pSubmeshArray = (SubMesh*)malloc(sizeof(SubMesh) * mMeshData->numSubmesh);

	//メッシュごとにループ
	for (int i = 0; i < mMeshData->numSubmesh; i++) {

		//各種データ取得
		GetMeshData(i, fbx_scene);
	}


	fbx_scene->Destroy();
	fbx_importer->Destroy();
	fbx_manager->Destroy();
	return;
}

void GameObject::GetMeshData(int target, FbxScene* scene)
{
	//アトリビュートからデータ取得
	FbxMesh* mesh = scene->GetSrcObject<FbxMesh>(target);

	//頂点数取得
	mMeshData->pSubmeshArray[target].numVertex = mesh->GetControlPointsCount();

	//頂点数から必要な分だけ頂点位置データの領域確保
	Vertex3D* verbuf;
	verbuf = (Vertex3D*)malloc(sizeof(Vertex3D) * mMeshData->pSubmeshArray[target].numVertex);

	//頂点データのうち、ウェイト情報は-1で初期化しておく
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

	//インデックス数取得
	int* indices = mesh->GetPolygonVertices();
	mMeshData->pSubmeshArray[target].numIndex = mesh->GetPolygonVertexCount();

	//インデックス領域の確保
	unsigned short* indbuf;
	indbuf = (unsigned short*)malloc(sizeof(unsigned short) * mMeshData->pSubmeshArray[target].numIndex);

	//全体のワールド座標を取得する
	FbxNode* n = mesh->GetNode();

	FbxDouble3 translation = n->LclTranslation.Get();
	FbxDouble3 rotation = n->LclRotation.Get();
	FbxDouble3 scaling = n->LclScaling.Get();

	mMeshData->baseRot = XMMatrixRotationX(XMConvertToRadians(rotation.mData[0]));
	mMeshData->baseRot *= XMMatrixRotationY(XMConvertToRadians(rotation.mData[1]));
	mMeshData->baseRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.mData[2]));

	//頂点データ取り込み
	for (int i = 0; i < mMeshData->pSubmeshArray[target].numVertex; i++) {

		//頂点位置
		verbuf[i].pos[0] = (float)mesh->GetControlPointAt(i)[0]; // x
		verbuf[i].pos[1] = (float)mesh->GetControlPointAt(i)[1]; // y
		verbuf[i].pos[2] = (float)mesh->GetControlPointAt(i)[2]; // z

		//頂点カラー（ダミー）
		verbuf[i].color[0] = 0.0f;
		verbuf[i].color[1] = 0.0f;
		verbuf[i].color[2] = 0.0f;
		verbuf[i].color[3] = 0.5f; // アルファ値　下げると透ける
	}

	//法線取得　インデックス順
	FbxArray<FbxVector4> normals;
	mesh->GetPolygonVertexNormals(normals);

	//UV画像名取得
	FbxStringList uvsetName;
	mesh->GetUVSetNames(uvsetName);

	//UV座標取得
	FbxArray<FbxVector2> uvsets;
	mesh->GetPolygonVertexUVs(uvsetName.GetStringAt(0), uvsets);

	int t = normals.Size();
	//法線、UVデータ取り込み
	for (int i = 0; i < normals.Size(); i++) {

		verbuf[indices[i]].normal[0] = normals[i][0];
		verbuf[indices[i]].normal[1] = normals[i][1];
		verbuf[indices[i]].normal[2] = normals[i][2];

		verbuf[indices[i]].tex[0] = uvsets[i][0];
		verbuf[indices[i]].tex[1] = 1.0f - uvsets[i][1];

	}

	//インデックスデータ作成
	for (int i = 0; i < mMeshData->pSubmeshArray[target].numIndex / 3; i++) {

		indbuf[i * 3] = indices[i * 3 + 2];
		indbuf[i * 3 + 1] = indices[i * 3 + 1];
		indbuf[i * 3 + 2] = indices[i * 3];
	}

	//ウェイトデータ取得
	FbxSkin* skin = static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));

	if (skin != NULL) {

		//ボーン総数取得
		mMeshData->boneNum = skin->GetClusterCount();

		//ボーン構造体の領域確保
		mMeshData->animator.modelBone = (Bone*)malloc(sizeof(Bone) * mMeshData->boneNum);

		//アニメ配列をボーン数分確保
		mMeshData->animator.matrix = (XMMATRIX**)malloc(sizeof(XMMATRIX*) * mMeshData->boneNum);

		//ボーン数だけループ
		for (int i = 0; i < mMeshData->boneNum; i++) {

			//クラスタ（影響頂点のかたまり）取得
			FbxCluster* cluster = skin->GetCluster(i);

			//全体のワールド変換行列を取得
			FbxVector4 t0 = n->GetGeometricTranslation(FbxNode::eSourcePivot);
			FbxVector4 r0 = n->GetGeometricRotation(FbxNode::eSourcePivot);
			FbxVector4 s0 = n->GetGeometricScaling(FbxNode::eSourcePivot);
			FbxAMatrix geometryOffset = FbxAMatrix(t0, r0, s0);

			//クラスタに存在する頂点数を取得
			int pointnum = cluster->GetControlPointIndicesCount();

			for (int j = 0; j < pointnum; j++) {

				//頂点番号取得
				int pointindex = cluster->GetControlPointIndices()[j];

				//現在ボーン（i）に対するウェイトの値を取得
				double weight = cluster->GetControlPointWeights()[j];

				//影響ボーン、ウェイト量を登録
				for (int k = 0; k < 4; k++) {

					if (verbuf[pointindex].Vweight.bone[k] == -1) {

						verbuf[pointindex].Vweight.bone[k] = i;
						verbuf[pointindex].Vweight.value[k] = (float)weight;
						break;
					}
				}

			}

			//モーション取得

			if (mFrame != 0) {

				//各ボーンにおける各フレームごとの変換行列を保存するための領域確保
				mMeshData->animator.matrix[i] = (XMMATRIX*)malloc(sizeof(XMMATRIX) * mFrame);

				//一時変数
				FbxMatrix vertexTransformMatrix;
				FbxAMatrix referenceGlobalInitPosition;
				FbxAMatrix clusterGlobalInitPosition;
				FbxMatrix clusterGlobalCurrentPosition;
				FbxMatrix clusterRelativeInitPosition;
				FbxMatrix clusterRelativeCurrentPositionInverse;

				//各フレーム分ループ
				for (int o = 0; o < mFrame; o++) {

					//fbx内における時間計算
					FbxTime timeCount = mStartTime + mFrameTime * o;
					if (timeCount > mEndTime) timeCount = mStartTime;

					//現フレームにおけるワールド変換行列を取得
					FbxMatrix globalPosition = n->EvaluateGlobalTransform(timeCount);

					//起点（原点）の初期姿勢行列取得
					cluster->GetTransformMatrix(referenceGlobalInitPosition);

					//起点（原点）をワールド座標に移動
					referenceGlobalInitPosition *= geometryOffset;

					//ボーンの初期姿勢行列取得
					cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

					//ボーンの現フレームにおける位置行列を取得
					clusterGlobalCurrentPosition = cluster->GetLink()->EvaluateGlobalTransform(timeCount);

					//ボーンをワールド座標に移動
					clusterRelativeInitPosition = clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition;

					//ボーンをアニメーションの挙動に従い移動
					clusterRelativeCurrentPositionInverse = globalPosition.Inverse() * clusterGlobalCurrentPosition;

					//ワールド座標における最終位置へ移動
					vertexTransformMatrix = clusterRelativeCurrentPositionInverse * clusterRelativeInitPosition;

					//普通に扱えるXMMATRIXへ移し替える
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

	//材質データの取得
	//材質数だけループ
	
	//マテリアルを取得
	FbxSurfaceMaterial* material = n->GetMaterial(target);

	if (material != NULL) { // マテリアル２個めからとばされる
		//材質データはランバートか？
		if (material->GetClassId().Is(FbxSurfaceLambert::ClassId)) {

			//ランバートならランバート用データを読み出し
			GetMaterialLambert(mMeshData->pSubmeshArray[target], material);
		}

		//ランバートでなくフォンだった
		else if (material->GetClassId().Is(FbxSurfacePhong::ClassId)) {

			//フォンはランバートの上位互換なので、とりあえずランバートデータ読み出し
			GetMaterialLambert(mMeshData->pSubmeshArray[target], material);

			//マテリアルをフォンのデータとして扱う
			FbxSurfacePhong* phong = (FbxSurfacePhong*)material;

			FbxProperty prop;

			// スペキュラー
			prop = phong->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (prop.IsValid()) {

				mMeshData->pSubmeshArray[target].material.specular[0] = prop.Get<FbxDouble3>()[0];
				mMeshData->pSubmeshArray[target].material.specular[1] = prop.Get<FbxDouble3>()[1];
				mMeshData->pSubmeshArray[target].material.specular[2] = prop.Get<FbxDouble3>()[2];
			}

		}
	}
	


	DirectX11Manager* manager = GetDirectX11Manager();

	//頂点バッファ作成
	//頂点バッファ : 1つのモデルの頂点の位置座標を保持するバッファ
	//D3D11_BUFFER_DESC : DirectX11上で動作するバッファ(データ保持用の構造体)　BindFlagsに"何に使うか"を設定することで用途が変わる

	D3D11_BUFFER_DESC hBufferDesc;                                                    //頂点バッファを記録するためのバッファ
	hBufferDesc.ByteWidth = sizeof(Vertex3D) * mMeshData->pSubmeshArray[target].numVertex;    //バッファの総サイズ　今回は読みだした頂点数分のデータを確保
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;                                          //バッファへのアクセス許可　D3D11_USAGE_DEFAULTで問題ない
	hBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;                                 //バッファをどう使うか　D3D11_BIND_VERTEX_BUFFERで頂点バッファとして扱うようになる
	hBufferDesc.CPUAccessFlags = 0;                                                   //CPUアクセス許可　0は不要
	hBufferDesc.MiscFlags = 0;                                                        //その他オプション　0で不要
	hBufferDesc.StructureByteStride = 0;                                              //バッファの総サイズ　丁寧にいくならByteWidth　0でも自動検出されるので問題はない

	D3D11_SUBRESOURCE_DATA hSubResourceData;                 //D3D11_SUBRESOURCE_DATA : バッファ作成時の設定値を記録する構造体
	hSubResourceData.pSysMem = verbuf;                       //参照する頂点データを持っている実体部
	hSubResourceData.SysMemPitch = 0;                        //バッファをテクスチャとして使用する際に0以外を入れる
	hSubResourceData.SysMemSlicePitch = 0;                   //バッファをテクスチャとして使用する際に0以外を入れる


	//(ID3D11Device*型変数)->CreateBuffer
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からバッファを作成する
	//第1引数:設定値が格納されたバッファ　第2引数:D3D11_SUBRESOURCE_DATAのアドレス　第3引数:作成されたバッファの格納先

	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&hBufferDesc, &hSubResourceData, &mMeshData->pSubmeshArray[target].pVertexBuffer))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateBuffer1"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//インデックスバッファ作成
	//インデックスバッファ : 各頂点の繋がりの関係性を保持するバッファ

	D3D11_BUFFER_DESC ibDesc;                                       //インデックスバッファを記録するためのバッファ
	ibDesc.ByteWidth = sizeof(unsigned short) * mMeshData->pSubmeshArray[target].numIndex;     //インデックスバッファのサイズ
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;                     //バッファをどう使うか　D3D11_BIND_INDEX_BUFFERでインデックスバッファとして扱うようになる
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA irData;                                  //D3D11_SUBRESOURCE_DATA : バッファ作成時の設定値を記録する構造体
	irData.pSysMem = indbuf;                                        //参照するインデックスデータを持っている実体部
	irData.SysMemPitch = 0;
	irData.SysMemSlicePitch = 0;


	//インデックスバッファを作成
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&ibDesc, &irData, &mMeshData->pSubmeshArray[target].pIndexBuffer))) {
		//作成に失敗したら警告を出してプログラムを強制終了
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


	// 解放処理
	free(verbuf);
	free(indbuf);
}

void GameObject::FbxSetPose(int frame)
{
	XMMATRIX mat;
	XMVECTOR buf;

	int target = frame;

	//各ボーンループ
	for (int i = 0; i < mMeshData->boneNum; i++) {

		//mat = XMMatrixTranslationFromVector(fbxmodelbone[i].positionVector);
		//anibuf.animematrix[i] = XMMatrixInverse(&buf, fbxmodelbone[i].boneinit);

		mMeshData->animator.animeBuffer.animematrix[i] = mMeshData->animator.matrix[i][target];

		/*for (int j = i; j < bonenum; j++) {
			anibuf.animematrix[j] = anibuf.animematrix[i];
		}*/

	}

	//最後に転置行列にしてシェーダに渡す
	for (int i = 0; i < mMeshData->boneNum; i++) {
		mMeshData->animator.animeBuffer.animematrix[i] = XMMatrixTranspose(mMeshData->animator.animeBuffer.animematrix[i]);
	}

}

void GameObject::GetMaterialLambert(SubMesh sm, FbxSurfaceMaterial* mati)
{
	FbxProperty prop; //HRESULTみたいなやーつ?

	//マテリアルをランバートのデータとして扱う
	FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)mati;

	// アンビエント
	prop = lambert->FindProperty(FbxSurfaceMaterial::sAmbient);
	if (prop.IsValid()) {

		sm.material.ambient[0] = prop.Get<FbxDouble3>()[0];
		sm.material.ambient[1] = prop.Get<FbxDouble3>()[1];
		sm.material.ambient[2] = prop.Get<FbxDouble3>()[2];
	}

	// ディヒューズ
	prop = lambert->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (prop.IsValid()) {

		sm.material.diffuse[0] = prop.Get<FbxDouble3>()[0];
		sm.material.diffuse[1] = prop.Get<FbxDouble3>()[1];
		sm.material.diffuse[2] = prop.Get<FbxDouble3>()[2];
	}
}


//-------------------------------------
// 影
//-------------------------------------

void GameObject::ShadowShaderDraw(CameraBuffer cb)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shader = GetShader();


	//描画用コンテキストに合成した座標変換行列を登録
	if (shader->mpShadowConstantBufferWorld.Get() != NULL) {
		manager->mpImContext->UpdateSubresource(shader->mpShadowConstantBufferWorld.Get(), 0, NULL, &cb, 0, 0);
	}


	//初期化
	for (int i = 0; i < mMeshData->boneNum; i++) {
		mMeshData->animator.animeBuffer.animematrix[i] = XMMatrixIdentity();
	}


	mMeshData->count++;
	if (mMeshData->count == mMeshData->animator.endframe) mMeshData->count = mMeshData->animator.startframe;
	if (mMeshData->count < 0) mMeshData->count = 0;
	/*mMeshData->animator.animeBuffer = FbxSetPose(mMeshData->count);*/
	FbxSetPose(mMeshData->count);


	//描画用コンテキストに合成した座標変換行列を登録
	if (shader->mpShadowConstantBufferAnime.Get() != NULL) {
		manager->mpImContext->UpdateSubresource(shader->mpShadowConstantBufferAnime.Get(), 0, NULL, &mMeshData->animator.animeBuffer, 0, 0);
	}


	UINT strides = sizeof(Vertex3D);
	UINT offsets = 0;

	manager->mpImContext->IASetInputLayout(shader->mpShadowInputLayout.Get());                      //描画用コンテキストに頂点レイアウトを設定
	manager->mpImContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);      //描画用コンテキストのポリゴン描画順をインデックス準拠に設定

	manager->mpImContext->VSSetConstantBuffers(0, 1, shader->mpShadowConstantBufferWorld.GetAddressOf()); //描画用コンテキストに定数バッファを設定
	manager->mpImContext->VSSetConstantBuffers(1, 1, shader->mpShadowConstantBufferAnime.GetAddressOf()); //描画用コンテキストに定数バッファを設定

	//テクスチャをピクセルシェーダーにセット
	//manager->mpImContext->PSSetShaderResources(0, 1, DX11TEXTURE_GetShaderResourceViewAddr(mMeshData->pSubmeshArray->pTexture));
	//manager->mpImContext->PSSetShaderResources(2, 1, shader->mpShadowShaderResourceView.GetAddressOf());
	//ToonMap_SetShaderResourceView();
	//manager->mpImContext->PSSetSamplers(0, 1, manager->mpSampler.GetAddressOf());             //描画用コンテキストにテクスチャサンプラーを設定
	//manager->mpImContext->PSSetSamplers(1, 1, shader->mpShadowSampler.GetAddressOf());        //描画用コンテキストにテクスチャサンプラーを設定

	manager->mpImContext->VSSetShader(shader->mpShadowVertexShader.Get(), NULL, 0);       //描画用コンテキストに頂点シェーダを設定
	manager->mpImContext->PSSetShader(shader->mpShadowPixelShader.Get(), nullptr, 0);     // 描画用コンテキストにピクセルシェーダを設定
	manager->mpImContext->RSSetViewports(1, &shader->mShadowViewport);                    // 描画用コンテキストにビューポートを設定                        
	manager->mpImContext->RSSetState(shader->mpShadowRasterState.Get());


	//現在の頂点レイアウト、頂点バッファ、インデックスバッファ、テクスチャなどの設定を元に、
	//第1引数の数だけ頂点を作成し、ポリゴンを描画する

	//材質を個別に描画するため、サブメッシュ回ループ
	for (int i = 0; i < mMeshData->numSubmesh; i++) {

		manager->mpImContext->IASetVertexBuffers(0, 1, &mMeshData->pSubmeshArray[i].pVertexBuffer, &strides, &offsets); //描画用コンテキストに頂点バッファを設定
		manager->mpImContext->IASetIndexBuffer(mMeshData->pSubmeshArray[i].pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);      //描画用コンテキストにインデックスバッファを設定
		manager->mpImContext->DrawIndexed(mMeshData->pSubmeshArray[i].numIndex, 0, 0);
	}
}

void GameObject::ShadowShaderDrawNomotion(CameraBuffer cb)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shaderNoMotion = GetShaderNoMotion();


	//描画用コンテキストに合成した座標変換行列を登録
	if (shaderNoMotion->mpShadowConstantBufferWorld != NULL) {

		manager->mpImContext->UpdateSubresource(shaderNoMotion->mpShadowConstantBufferWorld.Get(), 0, nullptr, &cb, 0, 0);
	}

	UINT strides = sizeof(Vertex3D);
	UINT offsets = 0;
	manager->mpImContext->IASetInputLayout(shaderNoMotion->mpShadowInputLayout.Get());            // 描画用コンテキストに頂点レイアウトを設定
	manager->mpImContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);          // 描画用コンテキストのポリゴン描画順をインデックス準拠に設定
	manager->mpImContext->VSSetConstantBuffers(0, 1, shaderNoMotion->mpShadowConstantBufferWorld.GetAddressOf()); // 描画用コンテキストに定数バッファを設定

	//テクスチャをピクセルシェーダーにセット
	//DX11_GetImmediateContext()->PSSetShaderResources(0, 1, DX11TEXTURE_GetShaderResourceViewAddr(dc.mesh->pSubmeshArray->pTexture));
	//ToonMap_SetShaderResourceView();                                                            // ToonMap
	//DX11_GetImmediateContext()->PSSetShaderResources(2, 1, &gpShaderResourceView_Shadow);
	//DX11_GetImmediateContext()->PSSetSamplers(0, 1, &g_Dx11Instances->pSampler);                // 描画用コンテキストにテクスチャサンプラーを設定
	//DX11_GetImmediateContext()->PSSetSamplers(1, 1, &gpSampler_Shadow);                         // 描画用コンテキストにテクスチャサンプラーを設定

	manager->mpImContext->VSSetShader(shaderNoMotion->mpShadowVertexShader.Get(), nullptr, 0);    // 描画用コンテキストに頂点シェーダを設定
	manager->mpImContext->PSSetShader(shaderNoMotion->mpShadowPixelShader.Get(), nullptr, 0);     // 描画用コンテキストにピクセルシェーダを設定
	manager->mpImContext->RSSetViewports(1, &shaderNoMotion->mShadowViewport);                    // 描画用コンテキストにビューポートを設定
	manager->mpImContext->RSSetState(shaderNoMotion->mpShadowRasterState.Get());



	//現在の頂点レイアウト、頂点バッファ、インデックスバッファ、テクスチャなどの設定を元に、
	//第1引数の数だけ頂点を作成し、ポリゴンを描画する
	//材質を個別に描画するため、サブメッシュ回ループ
	for (int i = 0; i < mMeshData->numSubmesh; i++) {

		manager->mpImContext->IASetVertexBuffers(0, 1, &mMeshData->pSubmeshArray[i].pVertexBuffer, &strides, &offsets);  //描画用コンテキストに頂点バッファを設定
		manager->mpImContext->IASetIndexBuffer(mMeshData->pSubmeshArray[i].pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);       //描画用コンテキストにインデックスバッファを設定
		manager->mpImContext->DrawIndexed(mMeshData->pSubmeshArray[i].numIndex, 0, 0);
	}
}

void GameObject::ShadowDraw()
{
	Camera* camera = GetCamera();
	Light*  light = GetLight();


	XMMATRIX worldMatrix = XMMatrixTranslation(mPos.x, mPos.y, mPos.z);

	//ワールド座標の作成
	//-------------------------------------------
	XMMATRIX worldScale = XMMatrixScaling(mScl.x, mScl.y, mScl.z);
	XMMATRIX matrixRotX = XMMatrixRotationX(XMConvertToRadians(mRot.x));
	XMMATRIX matrixRotY = XMMatrixRotationY(XMConvertToRadians(mRot.y));
	XMMATRIX matrixRotZ = XMMatrixRotationZ(XMConvertToRadians(mRot.z));
	worldMatrix = worldScale * mMeshData->baseRot * matrixRotX * matrixRotY * matrixRotZ * worldMatrix;
	//-------------------------------------------

	//ビュー座標の作成
	//-------------------------------------------
	// カメラ
	XMMATRIX viewMatrix_c = camera->GetViewMatrix();
	// ライト
	XMMATRIX viewMatrix_l = light->GetViewMatrix();
	//-------------------------------------------

	//プロジェクション座標
	//-------------------------------------------
	// カメラ
	XMMATRIX projMatrix_c = camera->GetProjectionMatrix();
	// ライト
	XMMATRIX projMatrix_l = light->GetProjectionMatrix();
	//-------------------------------------------


	//全ての変換行列をコンテキストに設定
	CameraBuffer cb;  //合成用の変数を宣言

	//XMStoreFloat4x4関数
	//XMFLOAT4X4を受け渡すためのもの
	//第1引数:代入先のポインタ　第2引数:受け渡し元

	//XMMatrixTranspose関数
	//行列を転置行列にする

	XMStoreFloat4x4(&cb.world, XMMatrixTranspose(worldMatrix));     //要はcd.world = XMMatrixTranspose(worldMatrix);
	XMStoreFloat4x4(&cb.view_c, XMMatrixTranspose(viewMatrix_c));   //要はcd.view = XMMatrixTranspose(viewMatrix);
	XMStoreFloat4x4(&cb.view_l, XMMatrixTranspose(viewMatrix_l));   //要はcd.view = XMMatrixTranspose(viewMatrix);
	XMStoreFloat4x4(&cb.projection_c, XMMatrixTranspose(projMatrix_c));   //要はcd.projection = XMMatrixTranspose(projMatrix);
	XMStoreFloat4x4(&cb.projection_l, XMMatrixTranspose(projMatrix_l));   //要はcd.projection = XMMatrixTranspose(projMatrix);

	// シェーダー描画（影）
	if(mMeshData->motion == true) ShadowShaderDraw(cb);
	else ShadowShaderDrawNomotion(cb);
}

void GameObject::ShadowClearScreen()
{
	DirectX11Manager* manager = GetDirectX11Manager();
	Shader* shaderNoMotion = GetShaderNoMotion();

	manager->mpImContext->ClearDepthStencilView(shaderNoMotion->mpShadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	manager->mpImContext->OMSetRenderTargets(0, nullptr, shaderNoMotion->mpShadowDepthStencilView.Get()); // 深度ステンシルビューを影用へ
}


//*************************************
// 関数
//*************************************

// IDを指定してオブジェクトを取得
GameObject* GetGameObject(int id)
{
	return gpGameObject[id];
}

// 初期化
void GameObjectInit(void) {

	for (int i = 0; i < MAX_GAMEOBJECT; i++) {
		gpGameObject[i] = new GameObject;
		gpGameObject[i]->Init(i, i);
		gpGameObject[i]->ShadowOnOf(true); // 影描画OnOf
	}
	

	// パラメーター設定
	GameObjectSetParameter();


	//-------------------------------------------
	// コンポーネント追加
	//-------------------------------------------
	// 当たり判定
	// ゴースト
	gpColliderComponent[ENEMY_01] = new ColliderComponent;
	gpColliderComponent[ENEMY_01]->SetOwner(gpGameObject[ENEMY_01]);
	gpColliderComponent[ENEMY_01]->InitSphere(gObjectColliderParameter[ENEMY_01].sphereRadius);
	// 木
	gpColliderComponent[TREE_01] = new ColliderComponent;
	gpColliderComponent[TREE_01]->SetOwner(gpGameObject[TREE_01]);
	gpColliderComponent[TREE_01]->InitAabb(gObjectColliderParameter[TREE_01].aabbLength);
	// 宝箱
	gpColliderComponent[TREASURECHEST] = new ColliderComponent;
	gpColliderComponent[TREASURECHEST]->SetOwner(gpGameObject[TREASURECHEST]);
	gpColliderComponent[TREASURECHEST]->InitAabb(gObjectColliderParameter[TREASURECHEST].aabbLength);

}

// 更新
void GameObjectUpdate(void) {

	// ゴースト移動　これはplayerクラスを作ってその中へ
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
	// コンポーネント追加
	//-------------------------------------------
	// 当たり判定
	Map* gridMap = GetGridMap();

	gpColliderComponent[ENEMY_01]->Update(gpColliderComponent[TREE_01]);
	gpColliderComponent[ENEMY_01]->Update(gpColliderComponent[TREASURECHEST]);
	gridMap->Update(gpColliderComponent[ENEMY_01], MAP_TREE_01);
	gpColliderComponent[ENEMY_01]->UpdateBackup();

}

void GameObjectDrawShadow() {

	//-------------------------------------------
	// 先にシャドウマップ作成
	//-------------------------------------------
	for (int i = 0; i < MAX_GAMEOBJECT; i++) {
		gpGameObject[i]->ShadowDraw();
	}

}

// 描画
void GameObjectDraw(void) {

	//-------------------------------------------
	// モデル描画
	//-------------------------------------------
	for (int i = 0; i < MAX_GAMEOBJECT; i++) {
		gpGameObject[i]->Draw();
	}


	//-------------------------------------------
	// コンポーネント追加
	//-------------------------------------------
	// 当たり判定
	/*for (int i = 0; i < COLLIDER_NUM; i++) {
		gpColliderComponent[i]->Draw();
	}*/

}

// 解放
void GameObjectRelease(void) {

	for (int i = 0; i < MAX_GAMEOBJECT; i++) {
		gpGameObject[i]->Release();
		delete gpGameObject[i];
	}


	//-------------------------------------------
	// コンポーネント追加
	//-------------------------------------------
	// 当たり判定
	for (int i = 0; i < COLLIDER_NUM; i++) {
		gpColliderComponent[i]->Release();
		delete gpColliderComponent[i];
	}

}

// パラメーター設定
void GameObjectSetParameter(void) {

	// ゴースト
	gpGameObject[ENEMY_01]->PositionSet(0, 10.0f, 0);
	gpGameObject[ENEMY_01]->RotationSet(0, 0, 0);
	gpGameObject[ENEMY_01]->ScaleSet(0.1f, 0.1f, 0.1f);
	gObjectColliderParameter[ENEMY_01].sphereRadius = 60.0f;

	// 木01
	gpGameObject[TREE_01]->PositionSet(60.0f, 0.0f, 20.0f);
	gpGameObject[TREE_01]->RotationSet(0, 0, 0);
	gpGameObject[TREE_01]->ScaleSet(2.0f, 2.0f, 2.0f);
	gObjectColliderParameter[TREE_01].aabbLength = {1.0f, 10.0f, 1.0f};

	// 宝箱
	gpGameObject[TREASURECHEST]->PositionSet(50.0f, 0, 80.0f);
	gpGameObject[TREASURECHEST]->RotationSet(0, 0, 0);
	gpGameObject[TREASURECHEST]->ScaleSet(1.0f, 1.0f, 1.0f);
	gObjectColliderParameter[TREASURECHEST].aabbLength = { 10.0f, 20.0f, 8.0f };

	// マップ01
	gpGameObject[MAP_01]->PositionSet(0, 0, 0);
	gpGameObject[MAP_01]->RotationSet(0, 0, 0);
	gpGameObject[MAP_01]->ScaleSet(1.0f, 1.0f, 1.0f);

	// 空01
	gpGameObject[SKY_01]->PositionSet(0, 0, 0);
	gpGameObject[SKY_01]->RotationSet(0, 0, 0);
	gpGameObject[SKY_01]->ScaleSet(1.0f, 1.0f, 1.0f);

	// 箱
	gpGameObject[BOX_01]->PositionSet(-40.0f, 15.0f, -40.0f);
	gpGameObject[BOX_01]->RotationSet(0, 0, 0);
	gpGameObject[BOX_01]->ScaleSet(0.1f, 0.1f, 0.1f);

}

