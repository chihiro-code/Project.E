#pragma once





//***********************************************
// 構造体
//***********************************************
struct FRGBA {
	float r, g, b, a;
};

struct FUV {
	float u, v;
};

class UI
{

};


//*******************************************
// 関数
//*******************************************
void UIInit();
void UIUpdate();
void UIDraw();
void UIRelease();
