#include "ShaderClass.h"


//*************************************
//シェーダのインクルード
//*************************************
// 通常描画用
#include "shader/vs.h"  //モーションあり
#include "shader/vs2.h" //モーションなし
#include "shader/ps.h"
// 影描画用
#include "shader/vs2_shadow.h" //モーションなし
#include "shader/ps_shadow.h"


//*************************************
// グローバル変数
//*************************************
Shader* gpShader;
Shader* gpShaderNoMotion;


//*************************************
// ShaderClass関数
//*************************************

//-------------------------------------
// 通常描画用
//-------------------------------------

void Shader::ShaderInit()
{
	DirectX11Manager* manager = GetDirectX11Manager();


	D3D11_INPUT_ELEMENT_DESC hInElementDesc[] = {     //D3D11_INPUT_ELEMENT_DESC : 頂点レイアウトを格納する構造体
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	//(ID3D11Device*型変数)->CreateInputLayout
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目から頂点レイアウトを作成する
	//第1引数:設定値が格納されたバッファ　第2引数:設定値バッファのサイズ　第3引数:送り先となる頂点シェーダの名前　第4引数:送り先となる頂点シェーダのサイズ　第5引数:作成されたレイアウトの格納先
	if (FAILED(manager->mpDevice.Get()->CreateInputLayout(hInElementDesc, ARRAYSIZE(hInElementDesc), g_vs_main, sizeof(g_vs_main), mpInputLayout.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateInputLayout"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	// 定数バッファ作成
	// 定数バッファ : 頂点バッファ、インデックスバッファとは別にGPUに描画指示を送るためのもの
	// 座標
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(CameraBuffer);        //座標変換用の構造体と関連させるので、そいつのサイズを登録
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //バッファをどう使うか　D3D11_BIND_CONSTANT_BUFFERで定数バッファとして扱うようになる
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	//定数バッファ作成
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&cbDesc, NULL, mpConstantBufferWorld.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateBuffer3"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	// アニメーション
	D3D11_BUFFER_DESC abDesc;
	abDesc.ByteWidth = sizeof(AnimeBuffer);
	abDesc.Usage = D3D11_USAGE_DEFAULT;
	abDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //バッファをどう使うか　D3D11_BIND_CONSTANT_BUFFERで定数バッファとして扱うようになる
	abDesc.CPUAccessFlags = 0;
	abDesc.MiscFlags = 0;
	abDesc.StructureByteStride = 0;

	//定数バッファ作成
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&abDesc, NULL, mpConstantBufferAnime.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateBuffer4"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	// ライト
	D3D11_BUFFER_DESC liDesc;
	liDesc.ByteWidth = sizeof(LightBuffer);
	liDesc.Usage = D3D11_USAGE_DEFAULT;
	liDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //バッファをどう使うか　D3D11_BIND_CONSTANT_BUFFERで定数バッファとして扱うようになる
	liDesc.CPUAccessFlags = 0;
	liDesc.MiscFlags = 0;
	liDesc.StructureByteStride = 0;

	//定数バッファ作成
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&liDesc, NULL, mpConstantBufferLight.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateBuffer5"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//テクスチャを張る際のサンプラーの作成
	//サンプラー : テクスチャを読み込む際の設定を登録するためのもの
	D3D11_SAMPLER_DESC smpDesc;                         //D3D11_SAMPLER_DESC : サンプラーの設定値を格納するための構造体
	ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));   //ここだけはゼロクリアしないとうまく動かない

	//宣言したD3D11_SAMPLER_DESCに設定値を入れていく
	smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;   //テクスチャ画像にかける線形補間の種類　たぶんさっきの方が優先される
	smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;     //U座標の色値は0～1.0fの間に収まるようにする
	smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;     //V座標の色値は0～1.0fの間に収まるようにする
	smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;     //W座標の色値は0～1.0fの間に収まるようにする

	//(ID3D11Device*型変数)->CreateSamplerState
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からサンプラーを作成する
	//第1引数:設定値の構造体　第2引数:成功時にアドレスを与える構造体
	if (FAILED(manager->mpDevice.Get()->CreateSamplerState(&smpDesc, manager->mpSampler.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateSamplerState"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//頂点シェーダ作成
	//頂点シェーダ : ポリゴンを描画する際にかけるエフェクトの総称

	//(ID3D11Device*型変数)->CreateVertexShader
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目から頂点シェーダを作成する
	//第1引数:頂点シェーダの名称　第2引数:頂点シェーダのサイズ　　第3引数:作成した頂点シェーダを格納する構造体
	if (FAILED(manager->mpDevice.Get()->CreateVertexShader(&g_vs_main, sizeof(g_vs_main), NULL, mpVertexShader.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateVertexShader"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//ピクセルシェーダ作成
	//ピクセルシェーダ : ポリゴンをピクセル化したあとにかけるエフェクトの総称

	//(ID3D11Device*型変数)->CreatePixelShader
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からピクセルシェーダを作成する
	//第1引数:ピクセルシェーダの名称　第2引数:ピクセルシェーダのサイズ　　第3引数:作成したピクセルシェーダを格納する構造体
	if (FAILED(manager->mpDevice.Get()->CreatePixelShader(&g_ps_main, sizeof(g_ps_main), NULL, mpPixelShader.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreatePixelShader"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//ラスタライザ生成
	//ラスタライザ :　ポリゴンを描画する際に必要となる設定値のこと　エフェクト的なものが多く必須ではない
	D3D11_RASTERIZER_DESC hRasterizerDesc = {           //ラスタライザの設定値を格納する構造体
			D3D11_FILL_SOLID,                           //描画方法の選択　今回は普通の面表示　D3D11_FILL_WIREFRAMEでワイヤーフレーム表示
			D3D11_CULL_BACK,                            //面描画方式の選択　今回は裏を表示しない　D3D11_CULL_FRONTで表を表示しない　D3D11_CULL_NONEで両面表示
			TRUE,                                      //TRUEで反時計回りを表、FALSEで時計回りとする
			0,                                          //ポリゴンそのものに深度を与えるための数値　普通は0
			0.0f,                                       //↑の設定値の最大値
			FALSE,                                      //Z方向のクリッピングを有効にするか
			FALSE,                                      //カリング設定　詳細は分かりませんでした
			FALSE,                                      //より精密なアンチエイリアシングをするかどうか
			FALSE,                                      //↑がFALSEの際、特殊なアンチエイリアシングを行うかどうか
			FALSE                                       //???資料なし
	};

	//(ID3D11Device*型変数)->CreateRasterizerState
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からラスタライザを作成する
	//第1引数:ラスタライザの設定値　第2引数:作成したラスタライザを格納する構造体
	if (FAILED(manager->mpDevice.Get()->CreateRasterizerState(&hRasterizerDesc, mpRasterState.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateRasterizerState"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	//テッセレーション処理
	//プログラム側からポリゴン割を制御するための処理　本来はハル→テッセレータ→ドメイン→ジオメトリの順で行うが、テッセレータはコーディング不可能なのでスルー
	//上手く使えば曲面をよりなめらかにしたりできると思う

	//ハルシェーダーは無し
	//ハルシェーダ : 面の分割の設定を定めるためのもの
	manager->mpImContext->HSSetShader(NULL, NULL, 0);

	//ドメインシェーダーは無し
	//ドメインシェーダ : ハルシェーダで分割されたものに対し、どのように配置するかを定める
	manager->mpImContext->DSSetShader(NULL, NULL, 0);

	//ジオメトリシェーダーは無し
	//ジオメトリシェーダ :　ポリゴンではなく頂点を増加させるためのもの
	manager->mpImContext->GSSetShader(NULL, NULL, 0);
}

void Shader::ShaderInitNoMotion()
{
	DirectX11Manager* manager = GetDirectX11Manager();


	D3D11_INPUT_ELEMENT_DESC hInElementDesc[] = {     //D3D11_INPUT_ELEMENT_DESC : 頂点レイアウトを格納する構造体
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"DEPTH",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};


	//(ID3D11Device*型変数)->CreateInputLayout
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目から頂点レイアウトを作成する
	//第1引数:設定値が格納されたバッファ　第2引数:設定値バッファのサイズ　第3引数:送り先となる頂点シェーダの名前　第4引数:送り先となる頂点シェーダのサイズ　第5引数:作成されたレイアウトの格納先
	if (FAILED(manager->mpDevice.Get()->CreateInputLayout(hInElementDesc, ARRAYSIZE(hInElementDesc), g_vs2_main, sizeof(g_vs2_main), mpInputLayout.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateInputLayout"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	// 定数バッファ作成
	// 定数バッファ : 頂点バッファ、インデックスバッファとは別にGPUに描画指示を送るためのもの
	// 座標
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(CameraBuffer);        //座標変換用の構造体と関連させるので、そいつのサイズを登録
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //バッファをどう使うか　D3D11_BIND_CONSTANT_BUFFERで定数バッファとして扱うようになる
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	//定数バッファ作成
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&cbDesc, NULL, mpConstantBufferWorld.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateBuffer3"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	// ライト
	D3D11_BUFFER_DESC liDesc;
	liDesc.ByteWidth = sizeof(LightBuffer);
	liDesc.Usage = D3D11_USAGE_DEFAULT;
	liDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //バッファをどう使うか　D3D11_BIND_CONSTANT_BUFFERで定数バッファとして扱うようになる
	liDesc.CPUAccessFlags = 0;
	liDesc.MiscFlags = 0;
	liDesc.StructureByteStride = 0;

	//定数バッファ作成
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&liDesc, NULL, mpConstantBufferLight.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateBuffer5"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//テクスチャを張る際のサンプラーの作成
	//サンプラー : テクスチャを読み込む際の設定を登録するためのもの
	D3D11_SAMPLER_DESC smpDesc;                         //D3D11_SAMPLER_DESC : サンプラーの設定値を格納するための構造体
	ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));   //ここだけはゼロクリアしないとうまく動かない

	//宣言したD3D11_SAMPLER_DESCに設定値を入れていく
	smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;   //テクスチャ画像にかける線形補間の種類　たぶんさっきの方が優先される
	smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;     //U座標の色値は0～1.0fの間に収まるようにする
	smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;     //V座標の色値は0～1.0fの間に収まるようにする
	smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;     //W座標の色値は0～1.0fの間に収まるようにする

	//(ID3D11Device*型変数)->CreateSamplerState
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からサンプラーを作成する
	//第1引数:設定値の構造体　第2引数:成功時にアドレスを与える構造体
	if (FAILED(manager->mpDevice.Get()->CreateSamplerState(&smpDesc, manager->mpSampler.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateSamplerState"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//頂点シェーダ作成
	//頂点シェーダ : ポリゴンを描画する際にかけるエフェクトの総称

	//(ID3D11Device*型変数)->CreateVertexShader
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目から頂点シェーダを作成する
	//第1引数:頂点シェーダの名称　第2引数:頂点シェーダのサイズ　　第3引数:作成した頂点シェーダを格納する構造体
	if (FAILED(manager->mpDevice.Get()->CreateVertexShader(&g_vs2_main, sizeof(g_vs2_main), NULL, mpVertexShader.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateVertexShader"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	//ピクセルシェーダ作成
	//ピクセルシェーダ : ポリゴンをピクセル化したあとにかけるエフェクトの総称

	//(ID3D11Device*型変数)->CreatePixelShader
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からピクセルシェーダを作成する
	//第1引数:ピクセルシェーダの名称　第2引数:ピクセルシェーダのサイズ　　第3引数:作成したピクセルシェーダを格納する構造体
	if (FAILED(manager->mpDevice.Get()->CreatePixelShader(&g_ps_main, sizeof(g_ps_main), NULL, mpPixelShader.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreatePixelShader"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}
	// 影用
	//if (FAILED(manager->mpDevice.Get()->CreatePixelShader(&g_ps_shadow_main, sizeof(g_ps_shadow_main), NULL, mpPixelShader.GetAddressOf()))) {
	//	//作成に失敗したら警告を出してプログラムを強制終了
	//	MessageBox(NULL, TEXT("CreatePixelShader"), TEXT("ERROR!!"), MB_ICONSTOP);
	//	PostQuitMessage(0);
	//}


	//ラスタライザ生成
	//ラスタライザ :　ポリゴンを描画する際に必要となる設定値のこと　エフェクト的なものが多く必須ではない
	D3D11_RASTERIZER_DESC hRasterizerDesc = {           //ラスタライザの設定値を格納する構造体
			D3D11_FILL_SOLID,                           //描画方法の選択　今回は普通の面表示　D3D11_FILL_WIREFRAMEでワイヤーフレーム表示
			D3D11_CULL_BACK,                            //面描画方式の選択　今回は裏を表示しない　D3D11_CULL_FRONTで表を表示しない　D3D11_CULL_NONEで両面表示
			TRUE,                                       //TRUEで反時計回りを表、FALSEで時計回りとする
			0,                                          //ポリゴンそのものに深度を与えるための数値　普通は0
			0.0f,                                       //↑の設定値の最大値
			FALSE,                                      //Z方向のクリッピングを有効にするか
			FALSE,                                      //カリング設定　詳細は分かりませんでした
			FALSE,                                      //より精密なアンチエイリアシングをするかどうか
			FALSE,                                      //↑がFALSEの際、特殊なアンチエイリアシングを行うかどうか
			FALSE                                       //???資料なし
	};

	//(ID3D11Device*型変数)->CreateRasterizerState
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からラスタライザを作成する
	//第1引数:ラスタライザの設定値　第2引数:作成したラスタライザを格納する構造体
	if (FAILED(manager->mpDevice.Get()->CreateRasterizerState(&hRasterizerDesc, mpRasterState.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateRasterizerState"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

	//テッセレーション処理
	//プログラム側からポリゴン割を制御するための処理　本来はハル→テッセレータ→ドメイン→ジオメトリの順で行うが、テッセレータはコーディング不可能なのでスルー
	//上手く使えば曲面をよりなめらかにしたりできると思う

	//ハルシェーダーは無し
	//ハルシェーダ : 面の分割の設定を定めるためのもの
	manager->mpImContext->HSSetShader(NULL, NULL, 0);

	//ドメインシェーダーは無し
	//ドメインシェーダ : ハルシェーダで分割されたものに対し、どのように配置するかを定める
	manager->mpImContext->DSSetShader(NULL, NULL, 0);

	//ジオメトリシェーダーは無し
	//ジオメトリシェーダ :　ポリゴンではなく頂点を増加させるためのもの
	manager->mpImContext->GSSetShader(NULL, NULL, 0);
}


//-------------------------------------
// 影描画用
//-------------------------------------

void Shader::ShaderInitNoMotionShadow()
{
	DirectX11Manager* manager = GetDirectX11Manager();

	// 頂点レイアウト設定
	D3D11_INPUT_ELEMENT_DESC hInElementDesc[] = { //D3D11_INPUT_ELEMENT_DESC : 頂点レイアウトを格納する構造体
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "DEPTH",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//(ID3D11Device*型変数)->CreateInputLayout
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目から頂点レイアウトを作成する
	//第1引数:設定値が格納されたバッファ　第2引数:設定値バッファのサイズ　第3引数:送り先となる頂点シェーダの名前　第4引数:送り先となる頂点シェーダのサイズ　第5引数:作成されたレイアウトの格納先
	if (FAILED(manager->mpDevice.Get()->CreateInputLayout(hInElementDesc, ARRAYSIZE(hInElementDesc), g_vs2_shadow_main, sizeof(g_vs2_shadow_main), mpShadowInputLayout.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateInputLayout_shadow"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//定数バッファ作成
	//定数バッファ : 頂点バッファ、インデックスバッファとは別にGPUに描画指示を送るためのもの
	//カメラ、ライトなどの描画に利用する
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(CameraBuffer); //座標変換用の構造体と関連させるので、そいつのサイズを登録
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    //バッファをどう使うか　D3D11_BIND_CONSTANT_BUFFERで定数バッファとして扱うようになる
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;
	if (FAILED(manager->mpDevice.Get()->CreateBuffer(&cbDesc, NULL, mpShadowConstantBuffer.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateBuffer_shadow"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//頂点シェーダ作成
	//頂点シェーダ : ポリゴンを描画する際にかけるエフェクトの総称
	//(ID3D11Device*型変数)->CreateVertexShader
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目から頂点シェーダを作成する
	//第1引数:頂点シェーダの名称　第2引数:頂点シェーダのサイズ　　第3引数:作成した頂点シェーダを格納する構造体
	if (FAILED(manager->mpDevice.Get()->CreateVertexShader(&g_vs2_shadow_main, sizeof(g_vs2_shadow_main), NULL, mpShadowVertexShader.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateVertexShader_shadow"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//ピクセルシェーダ作成
	//ピクセルシェーダ : ポリゴンをピクセル化したあとにかけるエフェクトの総称
	//(ID3D11Device*型変数)->CreatePixelShader
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からピクセルシェーダを作成する
	//第1引数:ピクセルシェーダの名称　第2引数:ピクセルシェーダのサイズ　　第3引数:作成したピクセルシェーダを格納する構造体
	if (FAILED(manager->mpDevice.Get()->CreatePixelShader(&g_ps_shadow_main, sizeof(g_ps_shadow_main), NULL, mpShadowPixelShader.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreatePixelShader_shadow"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	//テッセレーション処理
	//プログラム側からポリゴン割を制御するための処理　本来はハル→テッセレータ→ドメイン→ジオメトリの順で行うが、テッセレータはコーディング不可能なのでスルー

	//ハルシェーダーは無し
	//ハルシェーダ : 面の分割の設定を定めるためのもの
	manager->mpImContext->HSSetShader(NULL, NULL, 0);

	//ドメインシェーダーは無し
	//ドメインシェーダ : ハルシェーダで分割されたものに対し、どのように配置するかを定める
	manager->mpImContext->DSSetShader(NULL, NULL, 0);

	//ジオメトリシェーダーは無し
	//ジオメトリシェーダ :　ポリゴンではなく頂点を増加させるためのもの
	manager->mpImContext->GSSetShader(NULL, NULL, 0);


	HRESULT hr;
	// テクスチャ―
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
		// 深度ビュー
		D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Texture2D.MipSlice = 0;
		hr = manager->mpDevice.Get()->CreateDepthStencilView(mpShadowDepthStencilTexture.Get(), &dsDesc, mpShadowDepthStencilView.GetAddressOf());

		// リソースビュー
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = manager->mpDevice.Get()->CreateShaderResourceView(mpShadowDepthStencilTexture.Get(), &shaderResourceViewDesc, mpShadowShaderResourceView.GetAddressOf());
	}


	//テクスチャを張る際のサンプラーの作成
	//サンプラー : テクスチャを読み込む際の設定を登録するためのもの
	D3D11_SAMPLER_DESC smpDesc; //D3D11_SAMPLER_DESC : サンプラーの設定値を格納するための構造体
	ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));
	//宣言したD3D11_SAMPLER_DESCに設定値を入れていく
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

	//(ID3D11Device*型変数)->CreateSamplerState
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からサンプラーを作成する
	//第1引数:設定値の構造体　第2引数:成功時にアドレスを与える構造体
	if (FAILED(manager->mpDevice.Get()->CreateSamplerState(&smpDesc, mpShadowSampler.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateSamplerState_shadow"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	// ラスタライザ生成
	D3D11_RASTERIZER_DESC hRasterizerDesc;                       //ラスタライザの設定値を格納する構造体
	ZeroMemory(&hRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC)); // メモリー確保
	hRasterizerDesc.CullMode = D3D11_CULL_FRONT;                 // 面描画方式の選択　今回は裏を表示しない　D3D11_CULL_FRONTで表を表示しない　D3D11_CULL_NONEで両面表示
	hRasterizerDesc.FillMode = D3D11_FILL_SOLID;                 // 描画方法の選択　今回は普通の面表示　D3D11_FILL_WIREFRAMEでワイヤーフレーム表示
	hRasterizerDesc.DepthClipEnable = true;                      // 距離によるクリッピングを行うかのフラグになります
	hRasterizerDesc.FrontCounterClockwise = true;                // TRUEで反時計回りを表、FALSEで時計回りとする
	hRasterizerDesc.DepthBias = 0;                               // 指定のピクセルに加算する深度値です
	hRasterizerDesc.DepthBiasClamp = 0.0f;                       // ピクセルの最大深度バイアスです
	hRasterizerDesc.SlopeScaledDepthBias = 0.0f;                 // 指定のピクセルのスロープに対するスカラです
	hRasterizerDesc.ScissorEnable = false;                       // シザー矩形カリングを有効にします。アクティブなシザー矩形の外側のピクセルはすべてカリングされます
	hRasterizerDesc.MultisampleEnable = false;                   // tureで四辺形ラインアンチエイリアスをfalseでアルファラインアンチエイリアスを使用します
	hRasterizerDesc.AntialiasedLineEnable = false;               // MSAAのレンダーターゲットを使用している時、線分描画でMultisampleEnableがfalseのとき、アンチエイリアスを有効にします

	//(ID3D11Device*型変数)->CreateRasterizerState
	//一番の親であるデバイス(ID3D11Device*型変数)の権利で、設定項目からラスタライザを作成する
	//第1引数:ラスタライザの設定値　第2引数:作成したラスタライザを格納する構造体
	if (FAILED(manager->mpDevice.Get()->CreateRasterizerState(&hRasterizerDesc, mpShadowRasterState.GetAddressOf()))) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(NULL, TEXT("CreateRasterizerState"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}


	// ビューポート設定
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
// 関数
//*************************************

Shader* GetShader() {
	return gpShader;
}

Shader* GetShaderNoMotion() {
	return gpShaderNoMotion;
}

void ShaderInit(void) {

	// モーションあり
	gpShader = new Shader;
	gpShader->ShaderInit();
	
	// モーションなし
	gpShaderNoMotion = new Shader;
	gpShaderNoMotion->ShaderInitNoMotion();
	gpShaderNoMotion->ShaderInitNoMotionShadow(); // 影

}

void ShaderRelease(void) {

	delete gpShader;
	delete gpShaderNoMotion;

}