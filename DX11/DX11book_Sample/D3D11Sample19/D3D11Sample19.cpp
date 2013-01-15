/*----------------------------------------------------------
	Direct3D 11サンプル
		・Microsoft DirectX SDK (February 2010)
		・Visual Studio 2010 Express
		・Windows 7/Vista
		・シェーダモデル4.0
		対応

	D3D11Sample19.cpp
		「コンピュート・シェーダ」
--------------------------------------------------------------*/

#define STRICT					// 型チェックを厳密に行なう
#define WIN32_LEAN_AND_MEAN		// ヘッダーからあまり使われない関数を省く
#define WINVER        0x0600	// Windows Vista以降対応アプリを指定(なくてもよい)
#define _WIN32_WINNT  0x0600	// 同上

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// 解放マクロ

#define DATASIZE (1920*1080*3)		// 計算するデータ数
//#define COPYRESOURCE				// CopyResourceメソッドを使う

#include <windows.h>
#include <mmsystem.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include <locale.h>

// 必要なライブラリをリンクする
#pragma comment( lib, "d3d11.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx11d.lib" )
#else
#pragma comment( lib, "d3dx11.lib" )
#endif
#pragma comment( lib, "dxerr.lib" )

#pragma comment( lib, "winmm.lib" )

/*-------------------------------------------
	グローバル変数(アプリケーション関連)
--------------------------------------------*/
ID3D11Device*        g_pD3DDevice        = NULL;
ID3D11DeviceContext* g_pImmediateContext = NULL;
D3D_FEATURE_LEVEL    g_pFeatureLevels[] =  { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 }; // 機能レベルの配列
D3D_FEATURE_LEVEL    g_FeatureLevelsSupported; // デバイス作成時に返される機能レベル

ID3D11ComputeShader* g_pComputeShader = NULL;  // コンピュート・シェーダ
ID3D11Buffer*        g_pCBuffer = NULL;        // 定数バッファ

ID3D11Buffer* g_pBuffer[2] = { NULL, NULL }; // バッファ リソース
ID3D11Buffer* g_pReadBackBuffer = NULL;      // リードバック用バッファ リソース
ID3D11ShaderResourceView*  g_pSRV[2] = { NULL, NULL }; // シェーダ リソース ビュー
ID3D11UnorderedAccessView* g_pUAV[2] = { NULL, NULL }; // アンオーダード アクセス ビュー

// シェーダのコンパイル オプション
#if defined(DEBUG) || defined(_DEBUG)
UINT g_flagCompile = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION
					| D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
UINT g_flagCompile = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#endif

// 定数バッファのデータ
struct cbCBuffer {
    unsigned int g_iCount;  // 演算するデータ数
	unsigned int dummy[3];
};
cbCBuffer g_cbCBuffer;

/*-------------------------------------------
	デバイスの作成
--------------------------------------------*/
HRESULT CreateDevice(void)
{
	HRESULT hr;

	// ハードウェア デバイスを試す
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
			g_pFeatureLevels, _countof(g_pFeatureLevels), D3D11_SDK_VERSION, &g_pD3DDevice,
			&g_FeatureLevelsSupported, &g_pImmediateContext);
	if (SUCCEEDED(hr)) {
		//「コンピュート シェーダ」「未処理バッファー」「構造化バッファ」のサポート調査
	    D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
	    g_pD3DDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
	    if(hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x) {
			wprintf_s(L"[D3D_DRIVER_TYPE_HARDWARE]\n");
			return hr;
		}
		SAFE_RELEASE(g_pImmediateContext);
		SAFE_RELEASE(g_pD3DDevice);
	}

	// WARPデバイスを試す
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
			g_pFeatureLevels, _countof(g_pFeatureLevels), D3D11_SDK_VERSION, &g_pD3DDevice,
			&g_FeatureLevelsSupported, &g_pImmediateContext);
	if (SUCCEEDED(hr)) {
		//「コンピュート シェーダ」「未処理バッファー」「構造化バッファ」のサポート調査
	    D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
	    g_pD3DDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
	    if(hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x) {
			wprintf_s(L"[D3D_DRIVER_TYPE_WARP]\n");
			return hr;
		}
		SAFE_RELEASE(g_pImmediateContext);
		SAFE_RELEASE(g_pD3DDevice);
	}

	// リファレンス デバイスを試す
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, 0,
			g_pFeatureLevels, _countof(g_pFeatureLevels), D3D11_SDK_VERSION, &g_pD3DDevice,
			&g_FeatureLevelsSupported, &g_pImmediateContext);
	if (SUCCEEDED(hr)) {
		//「コンピュート シェーダ」「未処理バッファー」「構造化バッファ」のサポート調査
	    D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
	    g_pD3DDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
	    if(hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x) {
			wprintf_s(L"[D3D_DRIVER_TYPE_REFERENCE]\n");
			return hr;
		}
		SAFE_RELEASE(g_pImmediateContext);
		SAFE_RELEASE(g_pD3DDevice);
	}

	// 失敗
	return DXTRACE_ERR(L"D3D11CreateDevice", hr);;
}

/*-------------------------------------------
	シェーダの作成
--------------------------------------------*/
HRESULT CreateShader(void)
{
	HRESULT hr;

	// コンピュート・シェーダのコードをコンパイル
	ID3DBlob* pBlobVS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample19.sh",  // ファイル名
			NULL,          // マクロ定義(なし)
			NULL,          // インクルード・ファイル定義(なし)
			"CS",          // 「CS関数」がシェーダから実行される
			"cs_4_0",      // コンピュート・シェーダ
			g_flagCompile, // コンパイル・オプション
			0,             // エフェクトのコンパイル・オプション(なし)
			NULL,          // 直ぐにコンパイルしてから関数を抜ける。
			&pBlobVS,      // コンパイルされたバイト・コード
			NULL,          // エラーメッセージは不要
			NULL);         // 戻り値
	if (FAILED(hr))
		return DXTRACE_ERR(L"D3DX11CompileShaderFromFile", hr);

	// コンピュート・シェーダの作成
	hr = g_pD3DDevice->CreateComputeShader(
			pBlobVS->GetBufferPointer(),// バイト・コードへのポインタ
			pBlobVS->GetBufferSize(),	// バイト・コードの長さ
			NULL,
			&g_pComputeShader);			//
	SAFE_RELEASE(pBlobVS);  // バイト・コードを解放
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateComputeShader", hr);

	return hr;
}

/*-------------------------------------------
	定数バッファの作成
--------------------------------------------*/
HRESULT CreateCBuffer(void)
{
	HRESULT hr;

	// 定数バッファの定義
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;    // 動的(ダイナミック)使用法
	cBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER; // 定数バッファ
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPUから書き込む
	cBufferDesc.MiscFlags      = 0;
	cBufferDesc.StructureByteStride = 0;
	cBufferDesc.ByteWidth      = sizeof(cbCBuffer); // バッファ・サイズ

	// 定数バッファの作成
	hr = g_pD3DDevice->CreateBuffer(&cBufferDesc, NULL, &g_pCBuffer);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateBuffer", hr);

	return hr;
}

/*-------------------------------------------
	リソースの作成
--------------------------------------------*/
HRESULT CreateResource(void)
{
	HRESULT hr;

	//初期化用データ
	FLOAT* initdata = new FLOAT[DATASIZE];
	for (int i=0; i<DATASIZE; ++i)
	    initdata[i] = 1;

	// CPUによる合計計算
	DWORD time = timeGetTime(); // 時間の計測開始

	FLOAT sum = 0.0f;
	for (int i=0; i<DATASIZE; ++i)
	    sum += initdata[i];

	time = timeGetTime() - time; // 時間の計測終了
	wprintf_s(L"[CPU] %u ms [SUM = %f]\n", time, sum);

	// リソースの設定
    D3D11_BUFFER_DESC Desc;
    ZeroMemory(&Desc, sizeof(Desc));
    Desc.ByteWidth = DATASIZE * sizeof(FLOAT); // バッファ サイズ
    Desc.Usage     = D3D11_USAGE_DEFAULT;
    Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
    Desc.StructureByteStride = sizeof(FLOAT);

	D3D11_SUBRESOURCE_DATA SubResource;
	SubResource.pSysMem          = initdata;
	SubResource.SysMemPitch      = 0;
	SubResource.SysMemSlicePitch = 0;

	// 最初の入力リソース(データを初期化する)
    hr = g_pD3DDevice->CreateBuffer(&Desc, &SubResource, &g_pBuffer[0]);
	delete[] initdata;
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateBuffer", hr);

	// 最初の出力リソース
    hr = g_pD3DDevice->CreateBuffer(&Desc, NULL, &g_pBuffer[1]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateBuffer", hr);

	// リードバック用バッファ リソースの作成
    ZeroMemory(&Desc, sizeof(Desc));
#ifdef COPYRESOURCE
	Desc.ByteWidth = DATASIZE * sizeof(FLOAT);	// バッファ サイズ
#else
    Desc.ByteWidth = sizeof(FLOAT);				// バッファ サイズ
#endif
    Desc.Usage     = D3D11_USAGE_STAGING;  // CPUから読み書き可能なリソース
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // CPUから読み込む
    Desc.StructureByteStride = sizeof(FLOAT);

    hr = g_pD3DDevice->CreateBuffer(&Desc, NULL, &g_pReadBackBuffer);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateBuffer", hr);

	return hr;
}
/*-------------------------------------------
	シェーダ リソース ビューの作成
--------------------------------------------*/
HRESULT CreateSRV(void)
{
	HRESULT hr;

	// シェーダ リソース ビューの設定
	D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV;
    ZeroMemory(&DescSRV, sizeof(DescSRV));
	DescSRV.Format        = DXGI_FORMAT_UNKNOWN;
	DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    DescSRV.Buffer.ElementWidth = DATASIZE; // データ数

	// シェーダ リソース ビューの作成
	hr = g_pD3DDevice->CreateShaderResourceView(g_pBuffer[0], &DescSRV, &g_pSRV[0]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateShaderResourceView", hr);

	hr = g_pD3DDevice->CreateShaderResourceView(g_pBuffer[1], &DescSRV, &g_pSRV[1]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateShaderResourceView", hr);

	return hr;
}

/*-------------------------------------------
	アンオーダード・アクセス・ビューの作成
--------------------------------------------*/
HRESULT CreateUAV(void)
{
	HRESULT hr;

	// アンオーダード・アクセス・ビューの設定
	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
	ZeroMemory(&DescUAV, sizeof(DescUAV));
	DescUAV.Format        = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    DescUAV.Buffer.NumElements = DATASIZE; // データ数

	// アンオーダード・アクセス・ビューの作成
	hr = g_pD3DDevice->CreateUnorderedAccessView(g_pBuffer[0], &DescUAV, &g_pUAV[0]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateUnorderedAccessView", hr);

	hr = g_pD3DDevice->CreateUnorderedAccessView(g_pBuffer[1], &DescUAV, &g_pUAV[1]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateUnorderedAccessView", hr);

	return hr;
}

/*-------------------------------------------
	コンピュート シェーダを使った演算
--------------------------------------------*/
HRESULT ComputeShader(void)
{
	HRESULT hr = S_OK;
	DWORD time1 = timeGetTime(); // 時間の計測開始

	// 定数バッファをバインド
	g_pImmediateContext->CSSetConstantBuffers(0, 1, &g_pCBuffer);
	// シェーダを設定
	g_pImmediateContext->CSSetShader(g_pComputeShader, NULL, 0);

	// 演算を実行
	unsigned int datacount = DATASIZE;
	bool flag = false;
	ID3D11ShaderResourceView* pViewNULL = NULL;
	do {
		// リソースを入れ替える
		flag = !flag;

		// アンオーダード・アクセス・ビューに設定するため、
		// 現在のシェーダ リソース ビューを解除
		g_pImmediateContext->CSSetShaderResources(0, 1, &pViewNULL);
		// アンオーダード・アクセス・ビューの設定
		g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &g_pUAV[flag ? 1 : 0], NULL);
		// シェーダ リソース ビューの設定
		g_pImmediateContext->CSSetShaderResources(0, 1, &g_pSRV[flag ? 0 : 1]);

		// 定数バッファを更新
		g_cbCBuffer.g_iCount = datacount;
		D3D11_MAPPED_SUBRESOURCE MappedResource = {0}; 
		hr = g_pImmediateContext->Map(g_pCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		if (FAILED(hr))
			return DXTRACE_ERR(L"g_pImmediateContext->Map", hr);
	    memcpy(MappedResource.pData, &g_cbCBuffer, sizeof(g_cbCBuffer));
	    g_pImmediateContext->Unmap(g_pCBuffer, 0);

		// ディスパッチするスレッド グループの数
		unsigned int threadgroup = (datacount + 127) / 128;
		// コンピュート・シェーダの実行
		g_pImmediateContext->Dispatch(threadgroup, 1, 1);

		// 次に計算するデータ数
		datacount = threadgroup;
	} while (datacount > 1);

	DWORD time2 = timeGetTime(); // 計算終了時刻

	// 計算結果をリードバック用バッファ リソースにダウンロード
#ifdef COPYRESOURCE
	g_pImmediateContext->CopyResource(g_pReadBackBuffer, g_pBuffer[flag ? 1 : 0]);
#else
	D3D11_BOX box;
	box.left  = 0; box.right  = sizeof(FLOAT);
	box.top   = 0; box.bottom = 1;
	box.front = 0; box.back   = 1;
	g_pImmediateContext->CopySubresourceRegion(g_pReadBackBuffer, 0, 0, 0, 0, g_pBuffer[flag ? 1 : 0], 0, &box);
#endif

	DWORD time3 = timeGetTime(); // ダウンロード終了時刻

	// 結果を読み込む
	D3D11_MAPPED_SUBRESOURCE MappedResource = {0};
    hr = g_pImmediateContext->Map(g_pReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResource);       
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pImmediateContext->Map", hr);
	FLOAT sum = *(FLOAT*)MappedResource.pData;
    g_pImmediateContext->Unmap(g_pReadBackBuffer, 0);
	
	DWORD time4 = timeGetTime(); // ダウンロード終了時刻
	wprintf_s(L"[GPU] %u ms - %u ms - %u ms [SUM = %f]\n", time2 - time1, time3 - time2, time4 - time3, sum);

	return hr;
}

/*-------------------------------------------
	main関数
--------------------------------------------*/
int wmain(int argc, WCHAR* argv[])
{
	HRESULT hr;

	// ロケールを設定
	_wsetlocale(LC_ALL, L"Japanese");

	// **********************************************************
	// Direct3D11デバイスの作成
	hr = CreateDevice();

	// **********************************************************
	// コンピュート・シェーダの作成
	if (SUCCEEDED(hr))
		hr = CreateShader();

	// **********************************************************
	// 定数バッファの作成
	if (SUCCEEDED(hr))
		hr = CreateCBuffer();

	// **********************************************************
	// リソースの作成
	if (SUCCEEDED(hr))
		hr = CreateResource();

	// **********************************************************
	// シェーダ リソース ビューの作成
	if (SUCCEEDED(hr))
		hr = CreateSRV();

	// **********************************************************
	// アンオーダード・アクセス・ビューの作成
	if (SUCCEEDED(hr))
		hr = CreateUAV();

	// **********************************************************
	// コンピュート・シェーダを使った演算
	if (SUCCEEDED(hr))
		ComputeShader();

	// **********************************************************
	// 開放
	SAFE_RELEASE(g_pUAV[1]);
	SAFE_RELEASE(g_pUAV[0]);
	SAFE_RELEASE(g_pSRV[1]);
	SAFE_RELEASE(g_pSRV[0]);
	SAFE_RELEASE(g_pReadBackBuffer);
	SAFE_RELEASE(g_pBuffer[1]);
	SAFE_RELEASE(g_pBuffer[0]);
	SAFE_RELEASE(g_pCBuffer);
	SAFE_RELEASE(g_pComputeShader);
	SAFE_RELEASE(g_pImmediateContext);
	SAFE_RELEASE(g_pD3DDevice);

	return 0;
}
