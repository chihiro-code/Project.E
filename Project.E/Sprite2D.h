#pragma once
#include "DirectX11Manager.h"
#include "dx11texture.h"
#include "UI.h"


//***********************************************
// �摜��ID���X�g���`����
//***********************************************
enum IMAGE_ID {
	TITLE, // �^�C�g��

	MAX_IMAGE
};


//***********************************************
// �摜�̃t�@�C���p�X
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
	// �ϐ�
	//*******************************************
private:

	static VertexShader    mpVertexShader;
	static PixelShader     mpPixelShader;
	static InputLayout     mpInputLayout;
	static RasterizerState mpRasterizerState;

	struct VERTEX_SPRITE2D {
		XMFLOAT3 pos; // ���_�̍��W�i�ʒu�j
		FRGBA color;  // ���_�̐F
		FUV uv;       // �e�N�X�`�����|���S���ɓ\�鎞�̉摜��̑��Έʒu  
	};

	VERTEX_SPRITE2D mVertices[4]; // ���_4��\���z��
	VertexBuffer mpVertexBuffer;  // ���_�o�b�t�@�^�̕ϐ�
	DX11TEXTURE* mpTexture;       // �|���S���ɒ���t����e�N�X�`��


	//*******************************************
	// �֐�
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


// �X�}�[�g�|�C���^�^��`
typedef unique_ptr<Sprite2D*> spSprite2D;