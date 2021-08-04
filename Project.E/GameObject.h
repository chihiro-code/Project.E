#pragma once
#include "fbxsdk.h"
#include "DirectX11Manager.h"
#include "dx11texture.h"
#include "DirectXMath.h"


using namespace DirectX;


//***********************************************
// GameObjectで管理するモデルのIDリストを定義する
//***********************************************
enum GAMEOBJECT_ID {
	ENEMY_01,   // ゴースト
	TREE_01,    // 木01
	MAP_01,     // マップ01
	SKY_01,     // 空01
	TREASURECHEST, // 宝箱
	BOX_01,     // 箱

	MAX_GAMEOBJECT
};


//***********************************************
// FBXモデルのメッシュとテクスチャのファイルパス
//***********************************************
// FBXモデル　メッシュ
static const char* gpFbxModelFileNames[] = {
	"assets/model/Ghost02.fbx",
	"assets/model/tree_01.fbx",
	"assets/model/field_01.fbx",
	"assets/model/sky_01.fbx",
	"assets/model/TreasureChest.fbx",
	"assets/model/box_01.fbx",
};

// FBXモデル　テクスチャ
static const char* gpFbxTextureFileNames[] = {
	"assets/model/Ghost02.png",
	"assets/model/tree_01.png",
	"assets/model/field_01.png",
	"assets/model/sky_01.png",
	"assets/model/TreasureChest.png",
	"assets/model/box_01.png",
};


//*************************************
// GameObject class
//*************************************
class GameObject 
{
	//*******************************************
	// 変数
	//*******************************************
protected:

	//---------------------------------
	// オブジェクト
	//---------------------------------
	
	// 位置
	struct Position {
		float x;
		float y;
		float z;
	};

	// 回転
	struct Rotation {
		float x;
		float y;
		float z;
	};

	// 拡縮
	struct Scale {
		float x;
		float y;
		float z;
	};

	// アニメバッファ
	struct AnimeBuffer {
		XMMATRIX animematrix[128];
	};

	//ボーン構造体
	struct Bone {
		short firstChild;        //自分の子ボーンのうち、一番若い番号のボーン番号
		short firstBrother;      //自分と同じ親ボーンを持つボーン番号
		XMVECTOR positionVector; //ボーンの初期位置ベクトル(x,y,z)、回転の初期値は(0,0,0)
		XMMATRIX boneinit;
	};

	// アニメーション構造体
	struct Animator {
		AnimeBuffer animeBuffer;
		Bone* modelBone;
		XMMATRIX** matrix;
		int startframe;
		int endframe;
	};

	// 材質構造体
	struct Material {
		float ambient[3];
		float diffuse[3];
		float specular[4];
	};

	// サブメッシュ（オブジェクト事に複数もつ）
	struct SubMesh {
		ID3D11Buffer* pVertexBuffer; // 頂点バッファ
		ID3D11Buffer* pIndexBuffer;  // インデックスバッファ
		UINT numIndex;				 // インデックスデータの個数
		UINT numVertex;		         // 頂点バッファの個数						    
		Material material;			 // 各材質データ							    
		DX11TEXTURE* pTexture;		 // テクスチャ―
	};

	// メッシュ
	struct Mesh {
		SubMesh* pSubmeshArray; // サブメッシュの配列を確保したアドレスを持つ
		Animator animator;      // アニメーションに関する構造体
		XMMATRIX baseRot;		// FBX描画時の初期位置の調整用行列
		UINT numSubmesh;	    // サブメッシュの個数
		UINT modelType;
		int count;
		int boneNum;
		bool motion;			// Motionの有り無し
	};


	//---------------------------------
	// モデル読み込みで使用↓
	//---------------------------------
	
	//ウェイト保存用構造体
	struct Weight {
#pragma pack(push, 1)
		int bone[4];    //-1はエラー用
		float value[4];
#pragma pack(pop)
	};

	//ポリゴン頂点構造体
	struct Vertex3D {
#pragma pack(push, 1)
		float pos[3];   //xyz
		float color[4]; //rgb-a
		float normal[3];//xyz
		float tex[2];   //uv
		Weight Vweight; //各頂点のウェイト情報
#pragma pack(pop)
	};

	//アニメーションデータ構造体
	struct Anime {
		short targetbone;
		int frame;
		float bonepos[3];
		float bonelotate[4];
		BYTE bezier[4];
	};

	//アニメーション構造体　定数バッファとしてシェーダに送信
	struct LightBuffer {
		XMFLOAT4 objectpos;
		XMFLOAT4 lightpos;
		XMFLOAT4 lightcolor;
		XMFLOAT4 diffuse;
		XMFLOAT4 ambient;
		XMFLOAT4 specular;
		XMFLOAT4 camerapos;
	};

	//座標変換用の構造体　定数バッファとしてシェーダに送信
	//XMFLOAT4X4 はDirectXMath.hで定義される4*4の行列
	struct CameraBuffer {
		XMFLOAT4X4 world;            //ワールド変換行列
		XMFLOAT4X4 view_c;           //ビュー変換行列（カメラ）
		XMFLOAT4X4 view_l;           //ビュー変換行列（ライト）
		XMFLOAT4X4 projection_c;     //プロジェクション変換行列（カメラ）
		XMFLOAT4X4 projection_l;     //プロジェクション変換行列（ライト）
	};

protected:
	Mesh* mMeshData;
	Position mPos;
	Rotation mRot;
	Scale mScl;

	bool mDrawFlag;		// 描画 ON OF
	bool mDrawShadow;   // 影の描画 ON OF
	bool mAlive;        // 生存フラグ


	//---------------------------------
	// モデル読み込みで使用↓
	//---------------------------------
	
	//読み込み用一時変数
	int mFrame;
	FbxTime mStartTime;
	FbxTime mEndTime;
	FbxTime mFrameTime;

	//ライティング用
	LightBuffer mLbuf;

	
	//*******************************************
	// 関数
	//*******************************************
public:
	GameObject();
	~GameObject();

	//---------------------------------
	// オブジェクト
	//---------------------------------

	// 引数１：モデルID　引数２：テクスチャID
	void Init(int modelId, int texId);
	void Update(void);
	void Draw(void);
	void Release(void);
	void PositionSet(float x, float y, float z);
	void RotationSet(float x, float y, float z);
	void ScaleSet(float x, float y, float z);
	void SpecularSet(float red, float green, float blue, float power);
	void ShadowOnOf(bool);
	void ShadowDraw(); // 描画（影）

	XMFLOAT3 PositionGet();
	XMFLOAT3 RotationGet();
	XMFLOAT3 ScaleGet();
	


private:
	//---------------------------------
	// モデル読み込みで使用↓
	//---------------------------------

	// 初期化
	void FbxInit(char*, char*);

	// シェーダー初期化をモーションありかなしで分岐させる
	void FbxShaderInitChoice(Mesh*);

	// シェーダー描画（モーションあり）
	void FbxShaderDraw(CameraBuffer);

	// シェーダー描画（モーションなし）
	void FbxNoMotionShaderDraw(CameraBuffer);

	// 描画
	void FbxDraw();

	//fbxファイルのローダー関数の前方宣言
	void FbxLoader(const char[]);

	//fbxのノードを検索する関数の前方宣言
	void GetMeshData(int, FbxScene*);

	//fbxアニメーション関数の前方宣言
	void FbxSetPose(int);

	//fbxから材質データ（ランバート）を抽出
	void GetMaterialLambert(SubMesh, FbxSurfaceMaterial*);


	//---------------------------------
	// 影表示で使用↓
	//---------------------------------

	// シェーダー描画（影）
	void ShadowShaderDraw(CameraBuffer);
	
	// 画面クリア（影）
	void ShadowClearScreen();


};


//*************************************
// 関数
//*************************************

// ゲット関数
// IDを指定してオブジェクトを取得
GameObject* GetGameObject(int);

void GameObjectInit();
void GameObjectUpdate();
void GameObjectDraw();
void GameObjectRelease();
void GameObjectSetParameter();
