#include "Sprite2D.h"


//*************************************
// �O���[�o���ϐ�
//*************************************
VertexShader    Sprite2D::mpVertexShader;
PixelShader     Sprite2D::mpPixelShader;
InputLayout     Sprite2D::mpInputLayout;
RasterizerState Sprite2D::mpRasterizerState;


//*************************************
// �V�F�[�_�[
//*************************************
#include "shader\vssprite2d.h"
#include "shader\pssprite2d.h"


Sprite2D::Sprite2D()
{

}

Sprite2D::~Sprite2D()
{

}

void Sprite2D::Init(int texID)
{
	DirectX11Manager* manager = GetDirectX11Manager();


	// �C���v�b�g���C�A�E�g
	if (mpInputLayout == NULL) {
		D3D11_INPUT_ELEMENT_DESC desc[] = {
			{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",		 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		if (FAILED(manager->mpDevice.Get()->CreateInputLayout(desc, 3, g_vmain, sizeof(g_vmain), mpInputLayout.GetAddressOf()))) {
			//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
			MessageBox(NULL, TEXT("CreateInputLayout"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// ���_
	if (mpVertexShader == NULL) {
		if (FAILED(manager->mpDevice.Get()->CreateVertexShader(g_vmain, sizeof(g_vmain), NULL, mpVertexShader.GetAddressOf()))) {
			//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
			MessageBox(NULL, TEXT("CreateVertexShader"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// �s�N�Z��
	if (mpPixelShader == NULL) {
		if (FAILED(manager->mpDevice.Get()->CreatePixelShader(g_pmain, sizeof(g_pmain), NULL, mpPixelShader.GetAddressOf()))) {
			//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
			MessageBox(NULL, TEXT("CreatePixelShader"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// ���X�^���C�U�[
	if (mpRasterizerState == NULL) {
		D3D11_RASTERIZER_DESC rasterDesc;
		// setup the raster description which will determine how and what polygons will be drawn
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		//	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;		// ���C���t���[���ɂ������Ƃ�
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		if (FAILED(manager->mpDevice.Get()->CreateRasterizerState(&rasterDesc, mpRasterizerState.GetAddressOf()))) {
			//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
			MessageBox(NULL, TEXT("CreateRasterizerState"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// ���W�A�F
	for (int i = 0; i < 4; i++) {
		mVertices[i].pos.z = 0.0f;
		mVertices[i].color.r = 1.0f;
		mVertices[i].color.g = 1.0f;
		mVertices[i].color.b = 1.0f;
		mVertices[i].color.a = 1.0f;
	}
	SetRect(0, 0, 50, 50);  // �f�t�H���g�ʒu�T�C�Y

	// UV
	mVertices[0].uv.u = 0.0f;
	mVertices[0].uv.v = 0.0f;
	mVertices[1].uv.u = 1.0f;
	mVertices[1].uv.v = 0.0f;
	mVertices[2].uv.u = 0.0f;
	mVertices[2].uv.v = 1.0f;
	mVertices[3].uv.u = 1.0f;
	mVertices[3].uv.v = 1.0f;

	// �e�N�X�`���̃��[�h
	if (mpTexture == NULL) {
		
		mpTexture = DX11TEXTURE_CreateAndLoadFile(gpTextureFileNames[texID]);  // �e�N�X�`���̃t�@�C�������w��

		if (mpTexture == NULL) {
			//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
			MessageBox(NULL, TEXT("CreateAndLoadFile"), TEXT("ERROR!!"), MB_ICONSTOP);
			PostQuitMessage(0);
		}
	}

	// �o�b�t�@�[�쐬
	CreateVertexBuffer();

}

void Sprite2D::Draw()
{
	DirectX11Manager* manager = GetDirectX11Manager();

	// ���_�o�b�t�@�̒��_�̊e�p�����[�^������������A�@���_�f�[�^�̍X�V
	D3D11_MAPPED_SUBRESOURCE mapRes;
	HRESULT hr = manager->mpImContext.Get()->Map(mpVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapRes);

	if (SUCCEEDED(hr)) {
		memcpy_s(mapRes.pData, mapRes.RowPitch, mVertices, sizeof(mVertices));
		manager->mpImContext.Get()->Unmap(mpVertexBuffer.Get(), 0);
	}

	const UINT stride = sizeof(VERTEX_SPRITE2D);
	const UINT offset = 0;

	manager->mpImContext.Get()->IASetVertexBuffers(0, 1, mpVertexBuffer.GetAddressOf(), &stride, &offset);
	manager->mpImContext.Get()->IASetInputLayout(mpInputLayout.Get());
	manager->mpImContext.Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	manager->mpImContext.Get()->RSSetState(mpRasterizerState.Get());

	manager->mpImContext.Get()->VSSetShader(mpVertexShader.Get(), NULL, 0);
	manager->mpImContext.Get()->PSSetShader(mpPixelShader.Get(), NULL, 0);
	manager->mpImContext.Get()->PSSetShaderResources(0, 1, DX11TEXTURE_GetShaderResourceViewAddr(mpTexture));
	manager->mpImContext.Get()->PSSetSamplers(0, 1, manager->mpSampler.GetAddressOf()); // �T���v���[���Z�b�g����

	manager->RenderTargetsSetDisableZbuffer(); // Z�o�b�t�@�̖�����
	manager->mpImContext.Get()->Draw(4, 0);
	manager->RenderTargetsSet();  // Z�o�b�t�@�̗L����
}

void Sprite2D::SetTexture(DX11TEXTURE* pTex)
{
	mpTexture = pTex;
}

void Sprite2D::SetRect(float left, float top, float right, float bottom)
{
	DirectX11Manager* manager = GetDirectX11Manager();

	// ��΍��W�𑊑΍��W�ɕϊ�
	left   = left          / manager->GetScreenWidth() * 2.0f - 1.0f;
	top    = 1.0f - top    / manager->GetScreenHeight() * 2.0f;
	right  = right         / manager->GetScreenWidth() * 2.0f - 1.0f;
	bottom = 1.0f - bottom / manager->GetScreenHeight() * 2.0f;

	// �e���_�ɍ��W��ݒ�
	mVertices[0].pos.x = left;
	mVertices[0].pos.y = top;
	mVertices[1].pos.x = right;
	mVertices[1].pos.y = top;
	mVertices[2].pos.x = left;
	mVertices[2].pos.y = bottom;
	mVertices[3].pos.x = right;
	mVertices[3].pos.y = bottom;
}

void Sprite2D::SetColor(FRGBA v0, FRGBA v1, FRGBA v2, FRGBA v3)
{
	mVertices[0].color = v0;
	mVertices[1].color = v1;
	mVertices[2].color = v2;
	mVertices[3].color = v3;
}

bool Sprite2D::CreateVertexBuffer(void)
{
	DirectX11Manager* manager = GetDirectX11Manager();

	//���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(mVertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResourceData;
	subResourceData.pSysMem = mVertices;
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;

	HRESULT hr = manager->mpDevice.Get()->CreateBuffer(&bufferDesc, &subResourceData, mpVertexBuffer.GetAddressOf());

	return SUCCEEDED(hr);
}
