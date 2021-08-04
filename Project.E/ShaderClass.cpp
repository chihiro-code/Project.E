#include "ShaderClass.h"


//*************************************
//�V�F�[�_�̃C���N���[�h
//*************************************
// �ʏ�`��p
#include "shader/vs.h"  //���[�V��������
#include "shader/vs2.h" //���[�V�����Ȃ�
#include "shader/ps.h"
// �e�`��p
#include "shader/vs2_shadow.h" //���[�V�����Ȃ�
#include "shader/ps_shadow.h"


//*************************************
// �O���[�o���ϐ�
//*************************************
Shader* gpShader;
Shader* gpShaderNoMotion;


//*************************************
// ShaderClass�֐�
//*************************************

//-------------------------------------
// �ʏ�`��p
//-------------------------------------

void Shader::ShaderInit()
{
	DirectX11Manager* manager = GetDirectX11Manager();


	D3D11_INPUT_ELEMENT_DESC hInElementDesc[] = {     //D3D11_INPUT_ELEMENT_DESC : ���_���C�A�E�g���i�[����\����
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	//(ID3D11Device*�^�ϐ�)->CreateInputLayout
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ��璸�_���C�A�E�g���쐬����
	//��1����:�ݒ�l���i�[���ꂽ�o�b�t�@�@��2����:�ݒ�l�o�b�t�@�̃T�C�Y�@��3����:�����ƂȂ钸�_�V�F�[�_�̖��O�@��4����:�����ƂȂ钸�_�V�F�[�_�̃T�C�Y�@��5����:�쐬���ꂽ���C�A�E�g�̊i�[��
	if (FAILED(manager->mpDevice.Get()->CreateInputLayout(hInElementDesc, ARRAYSIZE(hInElementDesc), g_vs_main, sizeof(g_vs_main), mpInputLayout.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateInputLayout"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	// �萔�o�b�t�@�쐬
	// �萔�o�b�t�@ : ���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�Ƃ͕ʂ�GPU�ɕ`��w���𑗂邽�߂̂���
	// ���W
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(CameraBuffer);        //���W�ϊ��p�̍\���̂Ɗ֘A������̂ŁA�����̃T�C�Y��o�^
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //�o�b�t�@���ǂ��g�����@D3D11_BIND_CONSTANT_BUFFER�Œ萔�o�b�t�@�Ƃ��Ĉ����悤�ɂȂ�
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	//�萔�o�b�t�@�쐬
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&cbDesc, NULL, mpConstantBufferWorld.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateBuffer3"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	// �A�j���[�V����
	D3D11_BUFFER_DESC abDesc;
	abDesc.ByteWidth = sizeof(AnimeBuffer);
	abDesc.Usage = D3D11_USAGE_DEFAULT;
	abDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //�o�b�t�@���ǂ��g�����@D3D11_BIND_CONSTANT_BUFFER�Œ萔�o�b�t�@�Ƃ��Ĉ����悤�ɂȂ�
	abDesc.CPUAccessFlags = 0;
	abDesc.MiscFlags = 0;
	abDesc.StructureByteStride = 0;

	//�萔�o�b�t�@�쐬
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&abDesc, NULL, mpConstantBufferAnime.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateBuffer4"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	// ���C�g
	D3D11_BUFFER_DESC liDesc;
	liDesc.ByteWidth = sizeof(LightBuffer);
	liDesc.Usage = D3D11_USAGE_DEFAULT;
	liDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //�o�b�t�@���ǂ��g�����@D3D11_BIND_CONSTANT_BUFFER�Œ萔�o�b�t�@�Ƃ��Ĉ����悤�ɂȂ�
	liDesc.CPUAccessFlags = 0;
	liDesc.MiscFlags = 0;
	liDesc.StructureByteStride = 0;

	//�萔�o�b�t�@�쐬
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&liDesc, NULL, mpConstantBufferLight.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateBuffer5"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//�e�N�X�`���𒣂�ۂ̃T���v���[�̍쐬
	//�T���v���[ : �e�N�X�`����ǂݍ��ލۂ̐ݒ��o�^���邽�߂̂���
	D3D11_SAMPLER_DESC smpDesc;                         //D3D11_SAMPLER_DESC : �T���v���[�̐ݒ�l���i�[���邽�߂̍\����
	ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));   //���������̓[���N���A���Ȃ��Ƃ��܂������Ȃ�

	//�錾����D3D11_SAMPLER_DESC�ɐݒ�l�����Ă���
	smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;   //�e�N�X�`���摜�ɂ�������`��Ԃ̎�ށ@���Ԃ񂳂����̕����D�悳���
	smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;     //U���W�̐F�l��0�`1.0f�̊ԂɎ��܂�悤�ɂ���
	smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;     //V���W�̐F�l��0�`1.0f�̊ԂɎ��܂�悤�ɂ���
	smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;     //W���W�̐F�l��0�`1.0f�̊ԂɎ��܂�悤�ɂ���

	//(ID3D11Device*�^�ϐ�)->CreateSamplerState
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ���T���v���[���쐬����
	//��1����:�ݒ�l�̍\���́@��2����:�������ɃA�h���X��^����\����
	if (FAILED(manager->mpDevice.Get()->CreateSamplerState(&smpDesc, manager->mpSampler.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateSamplerState"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//���_�V�F�[�_�쐬
	//���_�V�F�[�_ : �|���S����`�悷��ۂɂ�����G�t�F�N�g�̑���

	//(ID3D11Device*�^�ϐ�)->CreateVertexShader
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ��璸�_�V�F�[�_���쐬����
	//��1����:���_�V�F�[�_�̖��́@��2����:���_�V�F�[�_�̃T�C�Y�@�@��3����:�쐬�������_�V�F�[�_���i�[����\����
	if (FAILED(manager->mpDevice.Get()->CreateVertexShader(&g_vs_main, sizeof(g_vs_main), NULL, mpVertexShader.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateVertexShader"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//�s�N�Z���V�F�[�_�쐬
	//�s�N�Z���V�F�[�_ : �|���S�����s�N�Z�����������Ƃɂ�����G�t�F�N�g�̑���

	//(ID3D11Device*�^�ϐ�)->CreatePixelShader
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ���s�N�Z���V�F�[�_���쐬����
	//��1����:�s�N�Z���V�F�[�_�̖��́@��2����:�s�N�Z���V�F�[�_�̃T�C�Y�@�@��3����:�쐬�����s�N�Z���V�F�[�_���i�[����\����
	if (FAILED(manager->mpDevice.Get()->CreatePixelShader(&g_ps_main, sizeof(g_ps_main), NULL, mpPixelShader.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreatePixelShader"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//���X�^���C�U����
	//���X�^���C�U :�@�|���S����`�悷��ۂɕK�v�ƂȂ�ݒ�l�̂��Ɓ@�G�t�F�N�g�I�Ȃ��̂������K�{�ł͂Ȃ�
	D3D11_RASTERIZER_DESC hRasterizerDesc = {           //���X�^���C�U�̐ݒ�l���i�[����\����
			D3D11_FILL_SOLID,                           //�`����@�̑I���@����͕��ʂ̖ʕ\���@D3D11_FILL_WIREFRAME�Ń��C���[�t���[���\��
			D3D11_CULL_BACK,                            //�ʕ`������̑I���@����͗���\�����Ȃ��@D3D11_CULL_FRONT�ŕ\��\�����Ȃ��@D3D11_CULL_NONE�ŗ��ʕ\��
			TRUE,                                      //TRUE�Ŕ����v����\�AFALSE�Ŏ��v���Ƃ���
			0,                                          //�|���S�����̂��̂ɐ[�x��^���邽�߂̐��l�@���ʂ�0
			0.0f,                                       //���̐ݒ�l�̍ő�l
			FALSE,                                      //Z�����̃N���b�s���O��L���ɂ��邩
			FALSE,                                      //�J�����O�ݒ�@�ڍׂ͕�����܂���ł���
			FALSE,                                      //��萸���ȃA���`�G�C���A�V���O�����邩�ǂ���
			FALSE,                                      //����FALSE�̍ہA����ȃA���`�G�C���A�V���O���s�����ǂ���
			FALSE                                       //???�����Ȃ�
	};

	//(ID3D11Device*�^�ϐ�)->CreateRasterizerState
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ��烉�X�^���C�U���쐬����
	//��1����:���X�^���C�U�̐ݒ�l�@��2����:�쐬�������X�^���C�U���i�[����\����
	if (FAILED(manager->mpDevice.Get()->CreateRasterizerState(&hRasterizerDesc, mpRasterState.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateRasterizerState"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	//�e�b�Z���[�V��������
	//�v���O����������|���S�����𐧌䂷�邽�߂̏����@�{���̓n�����e�b�Z���[�^���h���C�����W�I���g���̏��ōs�����A�e�b�Z���[�^�̓R�[�f�B���O�s�\�Ȃ̂ŃX���[
	//��肭�g���΋Ȗʂ����Ȃ߂炩�ɂ�����ł���Ǝv��

	//�n���V�F�[�_�[�͖���
	//�n���V�F�[�_ : �ʂ̕����̐ݒ���߂邽�߂̂���
	manager->mpImContext->HSSetShader(NULL, NULL, 0);

	//�h���C���V�F�[�_�[�͖���
	//�h���C���V�F�[�_ : �n���V�F�[�_�ŕ������ꂽ���̂ɑ΂��A�ǂ̂悤�ɔz�u���邩���߂�
	manager->mpImContext->DSSetShader(NULL, NULL, 0);

	//�W�I���g���V�F�[�_�[�͖���
	//�W�I���g���V�F�[�_ :�@�|���S���ł͂Ȃ����_�𑝉������邽�߂̂���
	manager->mpImContext->GSSetShader(NULL, NULL, 0);
}

void Shader::ShaderInitNoMotion()
{
	DirectX11Manager* manager = GetDirectX11Manager();


	D3D11_INPUT_ELEMENT_DESC hInElementDesc[] = {     //D3D11_INPUT_ELEMENT_DESC : ���_���C�A�E�g���i�[����\����
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"DEPTH",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};


	//(ID3D11Device*�^�ϐ�)->CreateInputLayout
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ��璸�_���C�A�E�g���쐬����
	//��1����:�ݒ�l���i�[���ꂽ�o�b�t�@�@��2����:�ݒ�l�o�b�t�@�̃T�C�Y�@��3����:�����ƂȂ钸�_�V�F�[�_�̖��O�@��4����:�����ƂȂ钸�_�V�F�[�_�̃T�C�Y�@��5����:�쐬���ꂽ���C�A�E�g�̊i�[��
	if (FAILED(manager->mpDevice.Get()->CreateInputLayout(hInElementDesc, ARRAYSIZE(hInElementDesc), g_vs2_main, sizeof(g_vs2_main), mpInputLayout.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateInputLayout"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	// �萔�o�b�t�@�쐬
	// �萔�o�b�t�@ : ���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�Ƃ͕ʂ�GPU�ɕ`��w���𑗂邽�߂̂���
	// ���W
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(CameraBuffer);        //���W�ϊ��p�̍\���̂Ɗ֘A������̂ŁA�����̃T�C�Y��o�^
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //�o�b�t�@���ǂ��g�����@D3D11_BIND_CONSTANT_BUFFER�Œ萔�o�b�t�@�Ƃ��Ĉ����悤�ɂȂ�
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	//�萔�o�b�t�@�쐬
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&cbDesc, NULL, mpConstantBufferWorld.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateBuffer3"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	// ���C�g
	D3D11_BUFFER_DESC liDesc;
	liDesc.ByteWidth = sizeof(LightBuffer);
	liDesc.Usage = D3D11_USAGE_DEFAULT;
	liDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //�o�b�t�@���ǂ��g�����@D3D11_BIND_CONSTANT_BUFFER�Œ萔�o�b�t�@�Ƃ��Ĉ����悤�ɂȂ�
	liDesc.CPUAccessFlags = 0;
	liDesc.MiscFlags = 0;
	liDesc.StructureByteStride = 0;

	//�萔�o�b�t�@�쐬
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&liDesc, NULL, mpConstantBufferLight.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateBuffer5"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//�e�N�X�`���𒣂�ۂ̃T���v���[�̍쐬
	//�T���v���[ : �e�N�X�`����ǂݍ��ލۂ̐ݒ��o�^���邽�߂̂���
	D3D11_SAMPLER_DESC smpDesc;                         //D3D11_SAMPLER_DESC : �T���v���[�̐ݒ�l���i�[���邽�߂̍\����
	ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));   //���������̓[���N���A���Ȃ��Ƃ��܂������Ȃ�

	//�錾����D3D11_SAMPLER_DESC�ɐݒ�l�����Ă���
	smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;   //�e�N�X�`���摜�ɂ�������`��Ԃ̎�ށ@���Ԃ񂳂����̕����D�悳���
	smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;     //U���W�̐F�l��0�`1.0f�̊ԂɎ��܂�悤�ɂ���
	smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;     //V���W�̐F�l��0�`1.0f�̊ԂɎ��܂�悤�ɂ���
	smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;     //W���W�̐F�l��0�`1.0f�̊ԂɎ��܂�悤�ɂ���

	//(ID3D11Device*�^�ϐ�)->CreateSamplerState
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ���T���v���[���쐬����
	//��1����:�ݒ�l�̍\���́@��2����:�������ɃA�h���X��^����\����
	if (FAILED(manager->mpDevice.Get()->CreateSamplerState(&smpDesc, manager->mpSampler.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateSamplerState"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//���_�V�F�[�_�쐬
	//���_�V�F�[�_ : �|���S����`�悷��ۂɂ�����G�t�F�N�g�̑���

	//(ID3D11Device*�^�ϐ�)->CreateVertexShader
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ��璸�_�V�F�[�_���쐬����
	//��1����:���_�V�F�[�_�̖��́@��2����:���_�V�F�[�_�̃T�C�Y�@�@��3����:�쐬�������_�V�F�[�_���i�[����\����
	if (FAILED(manager->mpDevice.Get()->CreateVertexShader(&g_vs2_main, sizeof(g_vs2_main), NULL, mpVertexShader.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateVertexShader"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	//�s�N�Z���V�F�[�_�쐬
	//�s�N�Z���V�F�[�_ : �|���S�����s�N�Z�����������Ƃɂ�����G�t�F�N�g�̑���

	//(ID3D11Device*�^�ϐ�)->CreatePixelShader
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ���s�N�Z���V�F�[�_���쐬����
	//��1����:�s�N�Z���V�F�[�_�̖��́@��2����:�s�N�Z���V�F�[�_�̃T�C�Y�@�@��3����:�쐬�����s�N�Z���V�F�[�_���i�[����\����
	if (FAILED(manager->mpDevice.Get()->CreatePixelShader(&g_ps_main, sizeof(g_ps_main), NULL, mpPixelShader.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreatePixelShader"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}
	// �e�p
	//if (FAILED(manager->mpDevice.Get()->CreatePixelShader(&g_ps_shadow_main, sizeof(g_ps_shadow_main), NULL, mpPixelShader.GetAddressOf()))) {
	//	//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
	//	MessageBox(NULL, TEXT("CreatePixelShader"), TEXT("ERROR!!"), MB_ICONSTOP);
	//	PostQuitMessage(0);
	//}


	//���X�^���C�U����
	//���X�^���C�U :�@�|���S����`�悷��ۂɕK�v�ƂȂ�ݒ�l�̂��Ɓ@�G�t�F�N�g�I�Ȃ��̂������K�{�ł͂Ȃ�
	D3D11_RASTERIZER_DESC hRasterizerDesc = {           //���X�^���C�U�̐ݒ�l���i�[����\����
			D3D11_FILL_SOLID,                           //�`����@�̑I���@����͕��ʂ̖ʕ\���@D3D11_FILL_WIREFRAME�Ń��C���[�t���[���\��
			D3D11_CULL_BACK,                            //�ʕ`������̑I���@����͗���\�����Ȃ��@D3D11_CULL_FRONT�ŕ\��\�����Ȃ��@D3D11_CULL_NONE�ŗ��ʕ\��
			TRUE,                                       //TRUE�Ŕ����v����\�AFALSE�Ŏ��v���Ƃ���
			0,                                          //�|���S�����̂��̂ɐ[�x��^���邽�߂̐��l�@���ʂ�0
			0.0f,                                       //���̐ݒ�l�̍ő�l
			FALSE,                                      //Z�����̃N���b�s���O��L���ɂ��邩
			FALSE,                                      //�J�����O�ݒ�@�ڍׂ͕�����܂���ł���
			FALSE,                                      //��萸���ȃA���`�G�C���A�V���O�����邩�ǂ���
			FALSE,                                      //����FALSE�̍ہA����ȃA���`�G�C���A�V���O���s�����ǂ���
			FALSE                                       //???�����Ȃ�
	};

	//(ID3D11Device*�^�ϐ�)->CreateRasterizerState
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ��烉�X�^���C�U���쐬����
	//��1����:���X�^���C�U�̐ݒ�l�@��2����:�쐬�������X�^���C�U���i�[����\����
	if (FAILED(manager->mpDevice.Get()->CreateRasterizerState(&hRasterizerDesc, mpRasterState.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateRasterizerState"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	//�e�b�Z���[�V��������
	//�v���O����������|���S�����𐧌䂷�邽�߂̏����@�{���̓n�����e�b�Z���[�^���h���C�����W�I���g���̏��ōs�����A�e�b�Z���[�^�̓R�[�f�B���O�s�\�Ȃ̂ŃX���[
	//��肭�g���΋Ȗʂ����Ȃ߂炩�ɂ�����ł���Ǝv��

	//�n���V�F�[�_�[�͖���
	//�n���V�F�[�_ : �ʂ̕����̐ݒ���߂邽�߂̂���
	manager->mpImContext->HSSetShader(NULL, NULL, 0);

	//�h���C���V�F�[�_�[�͖���
	//�h���C���V�F�[�_ : �n���V�F�[�_�ŕ������ꂽ���̂ɑ΂��A�ǂ̂悤�ɔz�u���邩���߂�
	manager->mpImContext->DSSetShader(NULL, NULL, 0);

	//�W�I���g���V�F�[�_�[�͖���
	//�W�I���g���V�F�[�_ :�@�|���S���ł͂Ȃ����_�𑝉������邽�߂̂���
	manager->mpImContext->GSSetShader(NULL, NULL, 0);
}


//-------------------------------------
// �e�`��p
//-------------------------------------

void Shader::ShaderInitNoMotionShadow()
{
	DirectX11Manager* manager = GetDirectX11Manager();

	// ���_���C�A�E�g�ݒ�
	D3D11_INPUT_ELEMENT_DESC hInElementDesc[] = { //D3D11_INPUT_ELEMENT_DESC : ���_���C�A�E�g���i�[����\����
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "DEPTH",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//(ID3D11Device*�^�ϐ�)->CreateInputLayout
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ��璸�_���C�A�E�g���쐬����
	//��1����:�ݒ�l���i�[���ꂽ�o�b�t�@�@��2����:�ݒ�l�o�b�t�@�̃T�C�Y�@��3����:�����ƂȂ钸�_�V�F�[�_�̖��O�@��4����:�����ƂȂ钸�_�V�F�[�_�̃T�C�Y�@��5����:�쐬���ꂽ���C�A�E�g�̊i�[��
	if (FAILED(manager->mpDevice.Get()->CreateInputLayout(hInElementDesc, ARRAYSIZE(hInElementDesc), g_vs2_shadow_main, sizeof(g_vs2_shadow_main), mpShadowInputLayout.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateInputLayout_shadow"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//�萔�o�b�t�@�쐬
	//�萔�o�b�t�@ : ���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�Ƃ͕ʂ�GPU�ɕ`��w���𑗂邽�߂̂���
	//�J�����A���C�g�Ȃǂ̕`��ɗ��p����
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(CameraBuffer); //���W�ϊ��p�̍\���̂Ɗ֘A������̂ŁA�����̃T�C�Y��o�^
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //�o�b�t�@���ǂ��g�����@D3D11_BIND_CONSTANT_BUFFER�Œ萔�o�b�t�@�Ƃ��Ĉ����悤�ɂȂ�
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&cbDesc, NULL, mpShadowConstantBuffer.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateBuffer_shadow"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//���_�V�F�[�_�쐬
	//���_�V�F�[�_ : �|���S����`�悷��ۂɂ�����G�t�F�N�g�̑���
	//(ID3D11Device*�^�ϐ�)->CreateVertexShader
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ��璸�_�V�F�[�_���쐬����
	//��1����:���_�V�F�[�_�̖��́@��2����:���_�V�F�[�_�̃T�C�Y�@�@��3����:�쐬�������_�V�F�[�_���i�[����\����
	if (FAILED(manager->mpDevice.Get()->CreateVertexShader(&g_vs2_shadow_main, sizeof(g_vs2_shadow_main), NULL, mpShadowVertexShader.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateVertexShader_shadow"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//�s�N�Z���V�F�[�_�쐬
	//�s�N�Z���V�F�[�_ : �|���S�����s�N�Z�����������Ƃɂ�����G�t�F�N�g�̑���
	//(ID3D11Device*�^�ϐ�)->CreatePixelShader
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ���s�N�Z���V�F�[�_���쐬����
	//��1����:�s�N�Z���V�F�[�_�̖��́@��2����:�s�N�Z���V�F�[�_�̃T�C�Y�@�@��3����:�쐬�����s�N�Z���V�F�[�_���i�[����\����
	if (FAILED(manager->mpDevice.Get()->CreatePixelShader(&g_ps_shadow_main, sizeof(g_ps_shadow_main), NULL, mpShadowPixelShader.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreatePixelShader_shadow"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//�e�b�Z���[�V��������
	//�v���O����������|���S�����𐧌䂷�邽�߂̏����@�{���̓n�����e�b�Z���[�^���h���C�����W�I���g���̏��ōs�����A�e�b�Z���[�^�̓R�[�f�B���O�s�\�Ȃ̂ŃX���[

	//�n���V�F�[�_�[�͖���
	//�n���V�F�[�_ : �ʂ̕����̐ݒ���߂邽�߂̂���
	manager->mpImContext->HSSetShader(NULL, NULL, 0);

	//�h���C���V�F�[�_�[�͖���
	//�h���C���V�F�[�_ : �n���V�F�[�_�ŕ������ꂽ���̂ɑ΂��A�ǂ̂悤�ɔz�u���邩���߂�
	manager->mpImContext->DSSetShader(NULL, NULL, 0);

	//�W�I���g���V�F�[�_�[�͖���
	//�W�I���g���V�F�[�_ :�@�|���S���ł͂Ȃ����_�𑝉������邽�߂̂���
	manager->mpImContext->GSSetShader(NULL, NULL, 0);


	HRESULT hr;
	// �e�N�X�`���\
	D3D11_TEXTURE2D_DESC txDesc;
	ZeroMemory(&txDesc, sizeof(D3D11_TEXTURE2D_DESC));
	txDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	txDesc.MipLevels = 1;
	txDesc.ArraySize = 1;
	txDesc.SampleDesc.Count = 1;
	txDesc.SampleDesc.Quality = 0;
	txDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = 0;
	txDesc.Width = manager->GetScreenWidth() * 3;
	txDesc.Height = manager->GetScreenHeight() * 3;

	hr = manager->mpDevice.Get()->CreateTexture2D(&txDesc, nullptr, mpShadowDepthStencilTexture.GetAddressOf());

	if (SUCCEEDED(hr)) {
		// �[�x�r���[
		D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Texture2D.MipSlice = 0;
		hr = manager->mpDevice.Get()->CreateDepthStencilView(mpShadowDepthStencilTexture.Get(), &dsDesc, mpShadowDepthStencilView.GetAddressOf());

		// ���\�[�X�r���[
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = manager->mpDevice.Get()->CreateShaderResourceView(mpShadowDepthStencilTexture.Get(), &shaderResourceViewDesc, mpShadowShaderResourceView.GetAddressOf());
	}


	//�e�N�X�`���𒣂�ۂ̃T���v���[�̍쐬
	//�T���v���[ : �e�N�X�`����ǂݍ��ލۂ̐ݒ��o�^���邽�߂̂���
	D3D11_SAMPLER_DESC smpDesc; //D3D11_SAMPLER_DESC : �T���v���[�̐ݒ�l���i�[���邽�߂̍\����
	ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));
	//�錾����D3D11_SAMPLER_DESC�ɐݒ�l�����Ă���
	smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	smpDesc.BorderColor[0] = 1.0f;
	smpDesc.BorderColor[1] = 1.0f;
	smpDesc.BorderColor[2] = 1.0f;
	smpDesc.BorderColor[3] = 1.0f;
	smpDesc.MinLOD = 0.0f; // or 0.0f
	smpDesc.MaxLOD = D3D11_FLOAT32_MAX; // or D3D11_FLOAT32_MAX
	smpDesc.MipLODBias = 0.0f;
	smpDesc.MaxAnisotropy = 0; // or 0
	smpDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	smpDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT; // or D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT

	//(ID3D11Device*�^�ϐ�)->CreateSamplerState
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ���T���v���[���쐬����
	//��1����:�ݒ�l�̍\���́@��2����:�������ɃA�h���X��^����\����
	if (FAILED(manager->mpDevice.Get()->CreateSamplerState(&smpDesc, mpShadowSampler.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateSamplerState_shadow"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	// ���X�^���C�U����
	D3D11_RASTERIZER_DESC hRasterizerDesc;                       //���X�^���C�U�̐ݒ�l���i�[����\����
	ZeroMemory(&hRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC)); // �������[�m��
	hRasterizerDesc.CullMode = D3D11_CULL_FRONT;                 // �ʕ`������̑I���@����͗���\�����Ȃ��@D3D11_CULL_FRONT�ŕ\��\�����Ȃ��@D3D11_CULL_NONE�ŗ��ʕ\��
	hRasterizerDesc.FillMode = D3D11_FILL_SOLID;                 // �`����@�̑I���@����͕��ʂ̖ʕ\���@D3D11_FILL_WIREFRAME�Ń��C���[�t���[���\��
	hRasterizerDesc.DepthClipEnable = true;                      // �����ɂ��N���b�s���O���s�����̃t���O�ɂȂ�܂�
	hRasterizerDesc.FrontCounterClockwise = true;                // TRUE�Ŕ����v����\�AFALSE�Ŏ��v���Ƃ���
	hRasterizerDesc.DepthBias = 0;                               // �w��̃s�N�Z���ɉ��Z����[�x�l�ł�
	hRasterizerDesc.DepthBiasClamp = 0.0f;                       // �s�N�Z���̍ő�[�x�o�C�A�X�ł�
	hRasterizerDesc.SlopeScaledDepthBias = 0.0f;                 // �w��̃s�N�Z���̃X���[�v�ɑ΂���X�J���ł�
	hRasterizerDesc.ScissorEnable = false;                       // �V�U�[��`�J�����O��L���ɂ��܂��B�A�N�e�B�u�ȃV�U�[��`�̊O���̃s�N�Z���͂��ׂăJ�����O����܂�
	hRasterizerDesc.MultisampleEnable = false;                   // ture�Ŏl�ӌ`���C���A���`�G�C���A�X��false�ŃA���t�@���C���A���`�G�C���A�X���g�p���܂�
	hRasterizerDesc.AntialiasedLineEnable = false;               // MSAA�̃����_�[�^�[�Q�b�g���g�p���Ă��鎞�A�����`���MultisampleEnable��false�̂Ƃ��A�A���`�G�C���A�X��L���ɂ��܂�

	//(ID3D11Device*�^�ϐ�)->CreateRasterizerState
	//��Ԃ̐e�ł���f�o�C�X(ID3D11Device*�^�ϐ�)�̌����ŁA�ݒ荀�ڂ��烉�X�^���C�U���쐬����
	//��1����:���X�^���C�U�̐ݒ�l�@��2����:�쐬�������X�^���C�U���i�[����\����
	if (FAILED(manager->mpDevice.Get()->CreateRasterizerState(&hRasterizerDesc, mpShadowRasterState.GetAddressOf()))) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(NULL, TEXT("CreateRasterizerState"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	// �r���[�|�[�g�ݒ�
	ZeroMemory(&mShadowViewport, sizeof(D3D11_VIEWPORT));
	mShadowViewport.TopLeftX = 0;
	mShadowViewport.TopLeftY = 0;
	mShadowViewport.Width = (FLOAT)manager->GetScreenWidth() * 3.0f;
	mShadowViewport.Height = (FLOAT)manager->GetScreenHeight() * 3.0f;
	mShadowViewport.MinDepth = 0.0f;
	mShadowViewport.MaxDepth = 1.0f;

	manager->mpImContext->RSSetViewports(1, &mShadowViewport);
}


//*************************************
// �֐�
//*************************************

Shader* GetShader() {
	return gpShader;
}

Shader* GetShaderNoMotion() {
	return gpShaderNoMotion;
}

void ShaderInit(void) {

	// ���[�V��������
	gpShader = new Shader;
	gpShader->ShaderInit();
	
	// ���[�V�����Ȃ�
	gpShaderNoMotion = new Shader;
	gpShaderNoMotion->ShaderInitNoMotion();
	gpShaderNoMotion->ShaderInitNoMotionShadow(); // �e

}

void ShaderRelease(void) {

	delete gpShader;
	delete gpShaderNoMotion;

}