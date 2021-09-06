#pragma once
#include "DirectX11Manager.h"
#include "dx11texture.h"
#include "UI.h"


//***********************************************
// 画像のIDリストを定義する
//***********************************************
enum IMAGE_ID {
	TITLE, // タイトル

	MAX_IMAGE
};


//***********************************************
// 画像のファイルパス
//***********************************************
static const char* gpTextureFileNames[] = {
	"assets/image/event-02.png",

};


//*************************************
// Sprite2D class
//*************************************
class Sprite2D
{
	//*******************************************
	// 変数
	//*******************************************
private:

	static VertexShader    mpVertexShader;
	static PixelShader     mpPixelShader;
	static InputLayout     mpInputLayout;
	static RasterizerState mpRasterizerState;

	struct VERTEX_SPRITE2D {
		XMFLOAT3 pos; // 頂点の座標（位置）
		FRGBA color;  // 頂点の色
		FUV uv;       // テクスチャをポリゴンに貼る時の画像上の相対位置  
	};

	VERTEX_SPRITE2D mVertices[4]; // 頂点4つを表す配列
	VertexBuffer mpVertexBuffer;  // 頂点バッファ型の変数
	DX11TEXTURE* mpTexture;       // ポリゴンに張り付けるテクスチャ


	//*******************************************
	// 関数
	//*******************************************
public:

	Sprite2D();
	~Sprite2D();

	void Init(int);
	void Draw();

	void SetTexture(DX11TEXTURE*);
	void SetRect(float, float, float, float);
	void SetColor(FRGBA, FRGBA, FRGBA, FRGBA);


private:
	bool CreateVertexBuffer(void);

};


// スマートポインタ型定義
typedef unique_ptr<Sprite2D*> spSprite2D;