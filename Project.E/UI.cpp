#include "UI.h"
#include "Sprite2D.h"


//*************************************
// �O���[�o���ϐ�
//*************************************
Sprite2D* gpImage;


void UIInit()
{
	DirectX11Manager* manager = GetDirectX11Manager();

	// �摜
	FRGBA color = { 1.0f, 1.0f, 1.0f, 1.0f };
	gpImage = new Sprite2D;
	gpImage->Init(TITLE);
	gpImage->SetColor(color, color, color, color);
	//gpImage->SetRect(0, 0, (float)manager->GetScreenWidth(), (float)manager->GetScreenHeight());
	gpImage->SetRect(0, 0, 300.0f, 200.0f);

}

void UIUpdate()
{

}

void UIDraw()
{
	gpImage->Draw();
}

void UIRelease()
{

}
