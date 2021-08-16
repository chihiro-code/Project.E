#pragma once
#include "GameObject.h"


//---------------------------------------------------------
// GameObjectに持たせるコンポーネントを表す基底クラス
// 全てのコンポーネントはこれを継承して作る
//---------------------------------------------------------


//*************************************
// ComponentBase class
//*************************************
class ComponentBase //: public GameObject
{
	//*******************************************
	// 変数
	//*******************************************
protected:

	GameObject* mpOwner; // GameObjectインスタンスのアドレス保存


	//*******************************************
	// 関数
	//*******************************************
public:
	ComponentBase();
	~ComponentBase();

	// GameObjectインスタンスのアドレスを保存する
	void SetOwner(GameObject*);

	// オーバーライドで内容を上書きする
	virtual void Init();
	virtual void Update();
	virtual void Draw();
	virtual void Release();

};

