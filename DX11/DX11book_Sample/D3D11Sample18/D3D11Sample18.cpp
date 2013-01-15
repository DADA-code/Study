/*----------------------------------------------------------
	Direct3D 11サンプル
		・Microsoft DirectX SDK (February 2010)
		・Visual Studio 2010 Express
		・Windows 7/Vista
		・シェーダモデル4.0
		対応

	D3D11Sample18.cpp
		「シャドウ・マッピング」
--------------------------------------------------------------*/

#define STRICT					// 型チェックを厳密に行なう
#define WIN32_LEAN_AND_MEAN		// ヘッダーからあまり使われない関数を省く
#define WINVER        0x0600	// Windows Vista以降対応アプリを指定(なくてもよい)
#define _WIN32_WINNT  0x0600	// 同上

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// 解放マクロ

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <crtdbg.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <dxerr.h>

#include "..\\WavefrontOBJ\\wavefrontobj.h"
#include "resource.h"

// 必要なライブラリをリンクする
#pragma comment( lib, "d3d11.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx11d.lib" )
#else
#pragma comment( lib, "d3dx11.lib" )
#endif
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "dxgi.lib" )

#pragma comment( lib, "winmm.lib" )

/*-------------------------------------------
	グローバル変数(アプリケーション関連)
--------------------------------------------*/
HINSTANCE	g_hInstance		= NULL;	// インスタンス ハンドル
HWND		g_hWindow		= NULL;	// ウインドウ ハンドル

WCHAR		g_szAppTitle[]	= L"Direct3D 11 Sample18";
WCHAR		g_szWndClass[]	= L"D3D11S18";

// 起動時の描画領域サイズ
SIZE		g_sizeWindow	= { 640, 480 };		// ウインドウのクライアント領域

/*-------------------------------------------
	グローバル変数(Direct3D関連)
--------------------------------------------*/

//機能レベルの配列
D3D_FEATURE_LEVEL g_pFeatureLevels[] =  { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 }; 
UINT              g_FeatureLevels    = 3;   // 配列の要素数
D3D_FEATURE_LEVEL g_FeatureLevelsSupported; // デバイス作成時に返される機能レベル

// インターフェイス
ID3D11Device*           g_pD3DDevice = NULL;        // デバイス
ID3D11DeviceContext*    g_pImmediateContext = NULL; // デバイス・コンテキスト
IDXGISwapChain*         g_pSwapChain = NULL;        // スワップ・チェイン

ID3D11RenderTargetView* g_pRenderTargetView = NULL; // 描画ターゲット・ビュー
D3D11_VIEWPORT          g_ViewPort[1];				// ビューポート

ID3D11Texture2D*          g_pDepthStencil = NULL;		// 深度/ステンシル
ID3D11DepthStencilView*   g_pDepthStencilView = NULL;	// 深度/ステンシル・ビュー

ID3D11InputLayout*        g_pInputLayout = NULL;            // 入力レイアウト・オブジェクト

// 3Dオブジェクト描画用
ID3D11VertexShader*       g_pVertexShader = NULL;		// 頂点シェーダ
ID3D11PixelShader*        g_pPixelShader = NULL;		// ピクセル・シェーダ
ID3D11PixelShader*        g_pPixelShaderNoSM = NULL;	// ピクセル・シェーダ(シャドウ マップなし)
ID3D11BlendState*         g_pBlendState = NULL;			// ブレンド・ステート・オブジェクト
ID3D11RasterizerState*	  g_pRasterizerState = NULL;	// ラスタライザ・ステート・オブジェクト
ID3D11DepthStencilState*  g_pDepthStencilState = NULL;	// 深度/ステンシル・ステート・オブジェクト

// シャドウ マップ描画用
ID3D11VertexShader*       g_pVertexShaderShadow = NULL;		// 頂点シェーダ
ID3D11RasterizerState*	  g_pRasterizerStateShadow = NULL;	// ラスタライザ・ステート・オブジェクト

ID3D11Buffer*             g_pCBuffer = NULL;		// 定数バッファ

ID3D11SamplerState*       g_pTextureSampler[2] = { NULL, NULL };		// サンプラー

// シャドウ・マッピング用
bool g_bShadowMappingMode = true;
ID3D11Texture2D*          g_pShadowMap = NULL;			// シャドウ・マップ
ID3D11DepthStencilView*   g_pShadowMapDSView = NULL;	// 深度/ステンシル・ビュー
ID3D11ShaderResourceView* g_pShadowMapSRView = NULL;	// シェーダ・リソース・ビュー
D3D11_VIEWPORT            g_ViewPortShadowMap[1];       // ビューポート

// シェーダのコンパイル オプション
#if defined(DEBUG) || defined(_DEBUG)
UINT g_flagCompile = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION
					| D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
UINT g_flagCompile = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#endif

// 深度バッファのモード
bool g_bDepthMode = true;

// スタンバイモード
bool g_bStandbyMode = false;

// 描画ターゲットをクリアする値(RGBA)
float g_ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };

float g_fEye = 2.5f;                       // 視点までの距離
XMFLOAT3 g_vLightPos(3.0f, 3.0f, -3.0f);   // 光源座標(ワールド座標系)

// 定数バッファのデータ定義
struct cbCBuffer {
	XMFLOAT4X4 World;		// ワールド変換行列
	XMFLOAT4X4 View;		// ビュー変換行列
	XMFLOAT4X4 Projection;	// 透視変換行列
	XMFLOAT4X4 SMWorldViewProj; // ワールド×ビュー×透視変換行列(シャドウマップ用)
	XMFLOAT4   Diffuse;		// ディフューズ色
	XMFLOAT3   Light;		// 光源座標(ワールド座標系)
	FLOAT      dummy;		// ダミー
};

// 定数バッファのデータ
struct cbCBuffer g_cbCBuffer;

// メッシュ情報
CWavefrontObj       g_wfObjKuma;
CWavefrontMtl       g_wfMtl;     // マテリアル

/*-------------------------------------------
	関数定義
--------------------------------------------*/

LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);
HRESULT CreateShaderObj(void);
HRESULT CreateShaderShadow(void);
HRESULT InitBackBuffer(void);
HRESULT InitShadowMap(void);

/*-------------------------------------------
	アプリケーション初期化
--------------------------------------------*/
HRESULT InitApp(HINSTANCE hInst)
{
	// アプリケーションのインスタンス ハンドルを保存
	g_hInstance = hInst;

	// ウインドウ クラスの登録
	WNDCLASS wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)MainWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= LoadIcon(hInst, (LPCTSTR)IDI_ICON1);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= g_szWndClass;

	if (!RegisterClass(&wc))
		return DXTRACE_ERR(L"InitApp", GetLastError());

	// メイン ウインドウ作成
	RECT rect;
	rect.top	= 0;
	rect.left	= 0;
	rect.right	= g_sizeWindow.cx;
	rect.bottom	= g_sizeWindow.cy;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

	g_hWindow = CreateWindow(g_szWndClass, g_szAppTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left, rect.bottom - rect.top,
			NULL, NULL, hInst, NULL);
	if (g_hWindow == NULL)
		return DXTRACE_ERR(L"InitApp", GetLastError());

	// ウインドウ表示
	ShowWindow(g_hWindow, SW_SHOWNORMAL);
	UpdateWindow(g_hWindow);

	return S_OK;
}

/*-------------------------------------------
	Direct3D初期化
--------------------------------------------*/
HRESULT InitDirect3D(void)
{
	// ウインドウのクライアント エリア
	RECT rc;
	GetClientRect(g_hWindow, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// デバイスとスワップ チェインの作成
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));	// 構造体の値を初期化
	sd.BufferCount       = 3;		// バック バッファ数
	sd.BufferDesc.Width  = width;	// バック バッファの幅
	sd.BufferDesc.Height = height;	// バック バッファの高さ
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // フォーマット
	sd.BufferDesc.RefreshRate.Numerator = 60;  // リフレッシュ レート(分子)
	sd.BufferDesc.RefreshRate.Denominator = 1; // リフレッシュ レート(分母)
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;	// スキャンライン
	sd.BufferDesc.Scaling          = DXGI_MODE_SCALING_CENTERED;			// スケーリング
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // バック バッファの使用法
	sd.OutputWindow = g_hWindow;	// 関連付けるウインドウ
	sd.SampleDesc.Count = 1;		// マルチ サンプルの数
	sd.SampleDesc.Quality = 0;		// マルチ サンプルのクオリティ
	sd.Windowed = TRUE;				// ウインドウ モード
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // モード自動切り替え

#if defined(DEBUG) || defined(_DEBUG)
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
    UINT createDeviceFlags = 0;
#endif

	// ハードウェア・デバイスを作成
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
            g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
            &g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
            &g_pImmediateContext);
    if(FAILED(hr)) {
        // WARPデバイスを作成
        hr = D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags,
            g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
            &g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
            &g_pImmediateContext);
        if(FAILED(hr)) {
            // リファレンス・デバイスを作成
            hr = D3D11CreateDeviceAndSwapChain(
                NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, createDeviceFlags,
                g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
                &g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
                &g_pImmediateContext);
            if(FAILED(hr)) {
                return DXTRACE_ERR(L"InitDirect3D D3D11CreateDeviceAndSwapChain", hr);
            }
        }
    }

	// **********************************************************
	// シェーダのコンパイル
	hr = CreateShaderObj();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D CreateShaderObj", hr);
	hr = CreateShaderShadow();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D CreateShaderShadow", hr);

	// **********************************************************
	// 定数バッファの定義
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;    // 動的(ダイナミック)使用法
	cBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER; // 定数バッファ
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPUから書き込む
	cBufferDesc.MiscFlags      = 0;
	cBufferDesc.StructureByteStride = 0;

	// 定数バッファの作成
	cBufferDesc.ByteWidth      = sizeof(cbCBuffer); // バッファ・サイズ
	hr = g_pD3DDevice->CreateBuffer(&cBufferDesc, NULL, &g_pCBuffer);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// **********************************************************
	// ブレンド・ステート・オブジェクトの作成
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable  = FALSE;
	BlendState.IndependentBlendEnable = FALSE;
	BlendState.RenderTarget[0].BlendEnable           = FALSE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pD3DDevice->CreateBlendState(&BlendState, &g_pBlendState);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBlendState", hr);

	// **********************************************************
	// 深度/ステンシル・ステート・オブジェクトの作成
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable      = TRUE; // 深度テストあり
	DepthStencil.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ALL; // 書き込む
	DepthStencil.DepthFunc        = D3D11_COMPARISON_LESS; // 手前の物体を描画
	DepthStencil.StencilEnable    = FALSE; // ステンシル・テストなし
	DepthStencil.StencilReadMask  = 0xff;     // ステンシル読み込みマスク。
	DepthStencil.StencilWriteMask = 0xff;     // ステンシル書き込みマスク。
			// 面が表を向いている場合のステンシル・テストの設定
	DepthStencil.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS; // 常に成功
			// 面が裏を向いている場合のステンシル・テストの設定
	DepthStencil.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS; // 常に成功
	hr = g_pD3DDevice->CreateDepthStencilState(&DepthStencil,
											   &g_pDepthStencilState);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateDepthStencilState", hr);

	// **********************************************************
	// Waveform OBJファイルの読み込み
	char mtlFileName[80];
	if (g_wfObjKuma.Load(g_pD3DDevice, g_pImmediateContext, "..\\misc\\kuma.obj", mtlFileName, sizeof(mtlFileName)) == false)
		return DXTRACE_ERR(L"InitDirect3D g_wfObjKuma.Load", E_FAIL);
	// MTLファイルの読み込み
	if (g_wfMtl.Load(g_pD3DDevice, mtlFileName, "..\\misc\\default.bmp") == false)
		return DXTRACE_ERR(L"InitDirect3D g_wfMtl.Load", E_FAIL);

	// **********************************************************
	// サンプラーの作成
	D3D11_SAMPLER_DESC descSampler;
	descSampler.Filter          = D3D11_FILTER_ANISOTROPIC;
    descSampler.AddressU        = D3D11_TEXTURE_ADDRESS_WRAP;
    descSampler.AddressV        = D3D11_TEXTURE_ADDRESS_WRAP;
    descSampler.AddressW        = D3D11_TEXTURE_ADDRESS_WRAP;
    descSampler.MipLODBias      = 0.0f;
    descSampler.MaxAnisotropy   = 2;
    descSampler.ComparisonFunc  = D3D11_COMPARISON_NEVER;
    descSampler.BorderColor[0]  = 0.0f;
    descSampler.BorderColor[1]  = 0.0f;
    descSampler.BorderColor[2]  = 0.0f;
    descSampler.BorderColor[3]  = 0.0f;
    descSampler.MinLOD          = -FLT_MAX;
    descSampler.MaxLOD          = FLT_MAX;
	hr = g_pD3DDevice->CreateSamplerState(&descSampler, &g_pTextureSampler[0]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateSamplerState", hr);

    descSampler.AddressU        = D3D11_TEXTURE_ADDRESS_BORDER;
    descSampler.AddressV        = D3D11_TEXTURE_ADDRESS_BORDER;
    descSampler.AddressW        = D3D11_TEXTURE_ADDRESS_BORDER;
    descSampler.BorderColor[0]  = 1.0f;
    descSampler.BorderColor[1]  = 1.0f;
    descSampler.BorderColor[2]  = 1.0f;
    descSampler.BorderColor[3]  = 1.0f;
	hr = g_pD3DDevice->CreateSamplerState(&descSampler, &g_pTextureSampler[1]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateSamplerState", hr);

	// **********************************************************
	// バック バッファの初期化
	hr = InitBackBuffer();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D InitBackBuffer", hr);

	// **********************************************************
	// シャドウ・マップの作成
	hr = InitShadowMap();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D InitShadowMap", hr);

	return hr;
}

/*-------------------------------------------
	シェーダのコンパイル(3Dオブジェクト)
--------------------------------------------*/
HRESULT CreateShaderObj(void)
{
	HRESULT hr;

	// **********************************************************
	// 頂点シェーダのコードをコンパイル
	ID3DBlob* pBlobVS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample18.sh",  // ファイル名
			NULL,          // マクロ定義(なし)
			NULL,          // インクルード・ファイル定義(なし)
			"VS",          // 「VS関数」がシェーダから実行される
			"vs_4_0",      // 頂点シェーダ
			g_flagCompile, // コンパイル・オプション
			0,             // エフェクトのコンパイル・オプション(なし)
			NULL,          // 直ぐにコンパイルしてから関数を抜ける。
			&pBlobVS,      // コンパイルされたバイト・コード
			NULL,          // エラーメッセージは不要
			NULL);         // 戻り値
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj D3DX11CompileShaderFromFile", hr);

	// 頂点シェーダの作成
	hr = g_pD3DDevice->CreateVertexShader(
			pBlobVS->GetBufferPointer(), // バイト・コードへのポインタ
			pBlobVS->GetBufferSize(),    // バイト・コードの長さ
			NULL,
			&g_pVertexShader); // 頂点シェーダを受け取る変数
//	SAFE_RELEASE(pBlobVS);  // バイト・コードを解放
	if (FAILED(hr)) {
		SAFE_RELEASE(pBlobVS);
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreateVertexShader", hr);
	}

	// **********************************************************
	// 入力要素
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                    D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3),     D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,    0, sizeof(XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// 入力レイアウト・オブジェクトの作成
	hr = g_pD3DDevice->CreateInputLayout(
			layout,                            // 定義の配列
			_countof(layout),                  // 定義の要素数
			pBlobVS->GetBufferPointer(),       // バイト・コードへのポインタ
			pBlobVS->GetBufferSize(),          // バイト・コードのサイズ
			&g_pInputLayout);                  // 受け取る変数のポインタ
	SAFE_RELEASE(pBlobVS);
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreateInputLayout", hr);

	// **********************************************************
	// ピクセル・シェーダのコードをコンパイル
	ID3DBlob* pBlobPS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample18.sh",  // ファイル名
			NULL,          // マクロ定義(なし)
			NULL,          // インクルード・ファイル定義(なし)
			"PS",          // 「PS関数」がシェーダから実行される
			"ps_4_0",      // ピクセル・シェーダ
			g_flagCompile, // コンパイル・オプション
			0,             // エフェクトのコンパイル・オプション(なし)
			NULL,          // 直ぐにコンパイルしてから関数を抜ける。
			&pBlobPS,      // コンパイルされたバイト・コード
			NULL,          // エラーメッセージは不要
			NULL);         // 戻り値
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj D3DX11CompileShaderFromFile", hr);

	// ピクセル・シェーダの作成
	hr = g_pD3DDevice->CreatePixelShader(
			pBlobPS->GetBufferPointer(), // バイト・コードへのポインタ
			pBlobPS->GetBufferSize(),    // バイト・コードの長さ
			NULL,
			&g_pPixelShader); // ピクセル・シェーダを受け取る変数
	SAFE_RELEASE(pBlobPS);  // バイト・コードを解放
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreatePixelShader", hr);

	// ピクセル・シェーダのコードをコンパイル
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample18.sh",  // ファイル名
			NULL,          // マクロ定義(なし)
			NULL,          // インクルード・ファイル定義(なし)
			"PS_NOSM",     // 「PS_NOSM関数」がシェーダから実行される
			"ps_4_0",      // ピクセル・シェーダ
			g_flagCompile, // コンパイル・オプション
			0,             // エフェクトのコンパイル・オプション(なし)
			NULL,          // 直ぐにコンパイルしてから関数を抜ける。
			&pBlobPS,      // コンパイルされたバイト・コード
			NULL,          // エラーメッセージは不要
			NULL);         // 戻り値
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj D3DX11CompileShaderFromFile", hr);

	// ピクセル・シェーダの作成
	hr = g_pD3DDevice->CreatePixelShader(
			pBlobPS->GetBufferPointer(), // バイト・コードへのポインタ
			pBlobPS->GetBufferSize(),    // バイト・コードの長さ
			NULL,
			&g_pPixelShaderNoSM); // ピクセル・シェーダを受け取る変数
	SAFE_RELEASE(pBlobPS);  // バイト・コードを解放
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreatePixelShader", hr);

	// **********************************************************
	// ラスタライザ・ステート・オブジェクトの作成
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;   // 普通に描画する
	RSDesc.CullMode = D3D11_CULL_BACK;    // 表面を描画する
	RSDesc.FrontCounterClockwise = FALSE; // 時計回りが表面
	RSDesc.DepthBias             = 0;
	RSDesc.DepthBiasClamp        = 0;
	RSDesc.SlopeScaledDepthBias  = 0;
	RSDesc.DepthClipEnable       = TRUE;
	RSDesc.ScissorEnable         = FALSE;
	RSDesc.MultisampleEnable     = FALSE;
	RSDesc.AntialiasedLineEnable = FALSE;
	hr = g_pD3DDevice->CreateRasterizerState(&RSDesc, &g_pRasterizerState);
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreateRasterizerState", hr);

	return hr;
}

/*-------------------------------------------
	シェーダのコンパイル(シャドウ マップ描画)
--------------------------------------------*/
HRESULT CreateShaderShadow(void)
{
	HRESULT hr;

	// **********************************************************
	// 頂点シェーダのコードをコンパイル
	ID3DBlob* pBlobVS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample18.sh",  // ファイル名
			NULL,          // マクロ定義(なし)
			NULL,          // インクルード・ファイル定義(なし)
			"VS_SM",       // 「VS_SM関数」がシェーダから実行される
			"vs_4_0",      // 頂点シェーダ
			g_flagCompile, // コンパイル・オプション
			0,             // エフェクトのコンパイル・オプション(なし)
			NULL,          // 直ぐにコンパイルしてから関数を抜ける。
			&pBlobVS,      // コンパイルされたバイト・コード
			NULL,          // エラーメッセージは不要
			NULL);         // 戻り値
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj D3DX11CompileShaderFromFile", hr);

	// 頂点シェーダの作成
	hr = g_pD3DDevice->CreateVertexShader(
			pBlobVS->GetBufferPointer(), // バイト・コードへのポインタ
			pBlobVS->GetBufferSize(),    // バイト・コードの長さ
			NULL,
			&g_pVertexShaderShadow); // 頂点シェーダを受け取る変数
	SAFE_RELEASE(pBlobVS);  // バイト・コードを解放
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreateVertexShader", hr);

	// **********************************************************
	// ラスタライザ・ステート・オブジェクトの作成
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;   // 普通に描画する
	RSDesc.CullMode = D3D11_CULL_FRONT;   // 表面を描画する
	RSDesc.FrontCounterClockwise = FALSE; // 時計回りが表面
	RSDesc.DepthBias             = 0;
	RSDesc.DepthBiasClamp        = 0;
	RSDesc.SlopeScaledDepthBias  = 0;
	RSDesc.DepthClipEnable       = TRUE;
	RSDesc.ScissorEnable         = FALSE;
	RSDesc.MultisampleEnable     = FALSE;
	RSDesc.AntialiasedLineEnable = FALSE;
	hr = g_pD3DDevice->CreateRasterizerState(&RSDesc, &g_pRasterizerStateShadow);
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreateRasterizerState", hr);

	return hr;
}

/*-------------------------------------------
	バック バッファの初期化(バック バッファを描画ターゲットに設定)
--------------------------------------------*/
HRESULT InitBackBuffer(void)
{
	HRESULT hr;

    // スワップ・チェインから最初のバック・バッファを取得する
    ID3D11Texture2D *pBackBuffer;  // バッファのアクセスに使うインターフェイス
    hr = g_pSwapChain->GetBuffer(
            0,                         // バック・バッファの番号
            __uuidof(ID3D11Texture2D), // バッファにアクセスするインターフェイス
            (LPVOID*)&pBackBuffer);    // バッファを受け取る変数
    if(FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pSwapChain->GetBuffer", hr);  // 失敗

	// バック・バッファの情報
	D3D11_TEXTURE2D_DESC descBackBuffer;
	pBackBuffer->GetDesc(&descBackBuffer);

    // バック・バッファの描画ターゲット・ビューを作る
    hr = g_pD3DDevice->CreateRenderTargetView(
            pBackBuffer,           // ビューでアクセスするリソース
            NULL,                  // 描画ターゲット・ビューの定義
            &g_pRenderTargetView); // 描画ターゲット・ビューを受け取る変数
    SAFE_RELEASE(pBackBuffer);  // 以降、バック・バッファは直接使わないので解放
    if(FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateRenderTargetView", hr);  // 失敗

	// 深度/ステンシル・テクスチャの作成
	D3D11_TEXTURE2D_DESC descDepth = descBackBuffer;
//	descDepth.Width     = descBackBuffer.Width;   // 幅
//	descDepth.Height    = descBackBuffer.Height;  // 高さ
	descDepth.MipLevels = 1;       // ミップマップ・レベル数
	descDepth.ArraySize = 1;       // 配列サイズ
	descDepth.Format    = DXGI_FORMAT_D32_FLOAT;  // フォーマット(深度のみ)
//	descDepth.SampleDesc.Count   = 1;  // マルチサンプリングの設定
//	descDepth.SampleDesc.Quality = 0;  // マルチサンプリングの品質
	descDepth.Usage          = D3D11_USAGE_DEFAULT;      // デフォルト使用法
	descDepth.BindFlags      = D3D11_BIND_DEPTH_STENCIL; // 深度/ステンシルとして使用
	descDepth.CPUAccessFlags = 0;   // CPUからはアクセスしない
	descDepth.MiscFlags      = 0;   // その他の設定なし
	hr = g_pD3DDevice->CreateTexture2D(
			&descDepth,         // 作成する2Dテクスチャの設定
			NULL,               // 
			&g_pDepthStencil);  // 作成したテクスチャを受け取る変数
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateTexture2D", hr);  // 失敗

	// 深度/ステンシル ビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format             = descDepth.Format;            // ビューのフォーマット
	descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags              = 0;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pD3DDevice->CreateDepthStencilView(
			g_pDepthStencil,       // 深度/ステンシル・ビューを作るテクスチャ
			&descDSV,              // 深度/ステンシル・ビューの設定
			&g_pDepthStencilView); // 作成したビューを受け取る変数
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateDepthStencilView", hr);  // 失敗

    // ビューポートの設定
    g_ViewPort[0].TopLeftX = 0.0f;    // ビューポート領域の左上X座標。
    g_ViewPort[0].TopLeftY = 0.0f;    // ビューポート領域の左上Y座標。
    g_ViewPort[0].Width    = (FLOAT)descBackBuffer.Width;   // ビューポート領域の幅
    g_ViewPort[0].Height   = (FLOAT)descBackBuffer.Height;  // ビューポート領域の高さ
    g_ViewPort[0].MinDepth = 0.0f; // ビューポート領域の深度値の最小値
    g_ViewPort[0].MaxDepth = 1.0f; // ビューポート領域の深度値の最大値

	// 定数バッファを更新
	// 射影変換行列(パースペクティブ(透視法)射影)
	XMMATRIX mat = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(30.0f),		// 視野角30°
			(float)descBackBuffer.Width / (float)descBackBuffer.Height,	// アスペクト比
			1.0f,							// 前方投影面までの距離
			400.0f);						// 後方投影面までの距離
	mat = XMMatrixTranspose(mat);
	XMStoreFloat4x4(&g_cbCBuffer.Projection, mat);

	//サイズを保存
	g_sizeWindow.cx = descBackBuffer.Width;
	g_sizeWindow.cy = descBackBuffer.Height;

	return S_OK;
}

/*-------------------------------------------
	シャドウ マップの初期化
--------------------------------------------*/
HRESULT InitShadowMap(void) {
	HRESULT hr;

	// シャドウ マップの作成
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width     = 512;   // 幅
	descDepth.Height    = 512;  // 高さ
	descDepth.MipLevels = 1;       // ミップマップ レベル数
	descDepth.ArraySize = 1;       // 配列サイズ
	descDepth.Format    = DXGI_FORMAT_R32_TYPELESS;  // フォーマット
	descDepth.SampleDesc.Count   = 1;  // マルチサンプリングの設定
	descDepth.SampleDesc.Quality = 0;  // マルチサンプリングの品質
	descDepth.Usage          = D3D11_USAGE_DEFAULT;      // デフォルト使用法
	descDepth.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE ; // 深度/ステンシル、シェーダ リソース ビューとして使用
	descDepth.CPUAccessFlags = 0;   // CPUからはアクセスしない
	descDepth.MiscFlags      = 0;   // その他の設定なし
	hr = g_pD3DDevice->CreateTexture2D(
			&descDepth,         // 作成する2Dテクスチャの設定
			NULL,               // 
			&g_pShadowMap);     // 作成したテクスチャを受け取る変数
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitShadowMap g_pD3DDevice->CreateTexture2D", hr);  // 失敗

	// 深度/ステンシル ビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format             = DXGI_FORMAT_D32_FLOAT;            // ビューのフォーマット
	descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags              = 0;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pD3DDevice->CreateDepthStencilView(
			g_pShadowMap,         // 深度/ステンシル・ビューを作るテクスチャ
			&descDSV,             // 深度/ステンシル・ビューの設定
			&g_pShadowMapDSView); // 作成したビューを受け取る変数
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitShadowMap g_pD3DDevice->CreateDepthStencilView", hr);  // 失敗

	// シェーダ リソース ビューの作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format        = DXGI_FORMAT_R32_FLOAT; // フォーマット
	srDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;  // 2Dテクスチャ
	srDesc.Texture2D.MostDetailedMip = 0;   // 最初のミップマップ レベル
	srDesc.Texture2D.MipLevels       = -1;  // すべてのミップマップ レベル

	// シェーダ リソース ビューの作成
	hr = g_pD3DDevice->CreateShaderResourceView(
				g_pShadowMap,          // アクセスするテクスチャ リソース
				&srDesc,               // シェーダ リソース ビューの設定
				&g_pShadowMapSRView);  // 受け取る変数
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitShadowMap g_pD3DDevice->CreateShaderResourceView", hr);

	// ビューポートの設定
	g_ViewPortShadowMap[0].TopLeftX = 0.0f;		// ビューポート領域の左上X座標。
	g_ViewPortShadowMap[0].TopLeftY = 0.0f;		// ビューポート領域の左上Y座標。
	g_ViewPortShadowMap[0].Width    = 512.0f;	// ビューポート領域の幅
	g_ViewPortShadowMap[0].Height   = 512.0f;	// ビューポート領域の高さ
	g_ViewPortShadowMap[0].MinDepth = 0.0f;		// ビューポート領域の深度値の最小値
	g_ViewPortShadowMap[0].MaxDepth = 1.0f;		// ビューポート領域の深度値の最大値

	return S_OK;
}

/*--------------------------------------------
	Objの描画処理
--------------------------------------------*/
void DrawObj(bool drawShadowMap)
{
	WFOBJ_GROUP* pGroup = g_wfObjKuma.GetGroup();
	int countGroup = g_wfObjKuma.GetCountGroup();
	for (int g = 0; g < countGroup; ++g) {
		if (pGroup[g].countIndex <= 0)
			continue;

		// マテリアル設定
		WFOBJ_MTL* pMtl = g_wfMtl.GetMaterial(pGroup[g].mtl);
		ID3D11ShaderResourceView* srv[2];
		srv[0] = g_wfMtl.GetTextureDefault(pMtl->map_Kd);
		srv[1] = drawShadowMap ? NULL : g_pShadowMapSRView;

		g_cbCBuffer.Diffuse.x = pMtl->Kd[0];
		g_cbCBuffer.Diffuse.y = pMtl->Kd[1];
		g_cbCBuffer.Diffuse.z = pMtl->Kd[2];
		g_cbCBuffer.Diffuse.w = pMtl->d;

		// 定数バッファのマップ取得
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		HRESULT hr = g_pImmediateContext->Map(
                  g_pCBuffer,              // マップするリソース
                  0,                       // サブリソースのインデックス番号
                  D3D11_MAP_WRITE_DISCARD, // 書き込みアクセス
                  0,                       //
                  &MappedResource);        // データの書き込み先ポインタ
		if (FAILED(hr)) {
			DXTRACE_ERR(L"DrawObj  g_pImmediateContext->Map", hr);  // 失敗
			return;
		}
		// データ書き込み
		CopyMemory(MappedResource.pData, &g_cbCBuffer, sizeof(cbCBuffer));
		// マップ解除
		g_pImmediateContext->Unmap(g_pCBuffer, 0);

		// PSにシェーダ・リソース・ビューを設定
		g_pImmediateContext->PSSetShaderResources(
	        0,              // 設定する最初のスロット番号
	        2,              // 設定するシェーダ・リソース・ビューの数
	        srv);			// 設定するシェーダ・リソース・ビューの配列

		// メッシュを描画
		g_wfObjKuma.Draw(g);
	}
}

/*--------------------------------------------
	物体の描画処理
--------------------------------------------*/
void RenderObj(bool drawShadowMap) {
	// **********************************************************
	// IAにOBJファイルの頂点バッファ/インデックス・バッファを設定
	g_wfObjKuma.SetIA();
	// IAに入力レイアウト・オブジェクトを設定
	g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	// VSに頂点シェーダを設定
	g_pImmediateContext->VSSetShader(drawShadowMap ? g_pVertexShaderShadow : g_pVertexShader, NULL, 0);

	// RSにラスタライザ・ステート・オブジェクトを設定
	g_pImmediateContext->RSSetState(drawShadowMap ? g_pRasterizerStateShadow : g_pRasterizerState);

	// PSにピクセル・シェーダを設定
	g_pImmediateContext->PSSetShader(drawShadowMap ? NULL : (g_bShadowMappingMode ? g_pPixelShader : g_pPixelShaderNoSM), NULL, 0);

	// OMにブレンド・ステート・オブジェクトを設定
	FLOAT BlendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	g_pImmediateContext->OMSetBlendState(g_pBlendState, BlendFactor, 0xffffffff);
	// OMに深度/ステンシル・ステート・オブジェクトを設定
	g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState, 0);

	// ***************************************
	// 3Dオブジェクトを描画
	DrawObj(drawShadowMap);
}

/*--------------------------------------------
	画面の描画処理
--------------------------------------------*/
HRESULT Render(void)
{
	HRESULT hr;

	// 定数バッファを更新
	// ワールド変換行列
	XMFLOAT3 center = g_wfObjKuma.GetBoundingSphereCenter();
	XMMATRIX matTrans = XMMatrixTranslation(-center.x, -center.y, -center.z);

	float scale = 1.0f / g_wfObjKuma.GetBoundingSphereRadius();
	XMMATRIX matScale = XMMatrixScaling(scale, scale, scale);

	FLOAT rotate = (FLOAT)(XM_PI * (timeGetTime() % 3000)) / 1500.0f;
	XMMATRIX matY = XMMatrixRotationY(rotate);

	XMMATRIX matWorld = matTrans * matScale * matY;
	XMStoreFloat4x4(&g_cbCBuffer.World, XMMatrixTranspose(matWorld));

	// ***************************************
	// シャドウ マップの描画
	if (g_bShadowMappingMode) {
		g_pImmediateContext->ClearState();

		// ビュー変換行列(光源から見る)
		XMVECTORF32 focusPosition = { 0.0f, 0.0f,  0.0f };  // 注視点
		XMVECTORF32 upDirection   = { 0.0f, 1.0f,  0.0f };  // カメラの上方向
		XMMATRIX matShadowMapView = XMMatrixLookAtLH(XMLoadFloat3(&g_vLightPos), focusPosition, upDirection);
		XMStoreFloat4x4(&g_cbCBuffer.View, XMMatrixTranspose(matShadowMapView));

		// 射影変換行列(パースペクティブ(透視法)射影)
		XMMATRIX matShadowMapProj = XMMatrixPerspectiveFovLH(
				XMConvertToRadians(45.0f),		// 視野角45°
				g_ViewPortShadowMap[0].Width / g_ViewPortShadowMap[0].Height,	// アスペクト比
				1.0f,							// 前方投影面までの距離
				400.0f);						// 後方投影面までの距離
		XMStoreFloat4x4(&g_cbCBuffer.Projection, XMMatrixTranspose(matShadowMapProj));

		// 深度/ステンシルのクリア
		g_pImmediateContext->ClearDepthStencilView(g_pShadowMapDSView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		// VSに定数バッファを設定
		g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);

		// PSに定数バッファを設定
		g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
		// PSにサンプラーを設定
		g_pImmediateContext->PSSetSamplers(0, 2, g_pTextureSampler);

		// RSにビューポートを設定
		g_pImmediateContext->RSSetViewports(1, g_ViewPortShadowMap);

		// OMに描画ターゲット ビューと深度/ステンシル・ビューを設定
		ID3D11RenderTargetView* pRender[1] = { NULL };
		g_pImmediateContext->OMSetRenderTargets(1, pRender, g_pShadowMapDSView);

		// 物体の描画
		RenderObj(true);

		// シャドウマップの設定
		XMMATRIX mat = XMMatrixTranspose(matWorld * matShadowMapView * matShadowMapProj);
		XMStoreFloat4x4(&g_cbCBuffer.SMWorldViewProj, mat);
	}

	// ***************************************
	// ビュー変換行列
	XMVECTORF32 eyePosition   = { 0.0f, g_fEye, -g_fEye, 1.0f };  // 視点(カメラの位置)
	XMVECTORF32 focusPosition = { 0.0f, 0.0f,  0.0f, 1.0f };  // 注視点
	XMVECTORF32 upDirection   = { 0.0f, 1.0f,  0.0f, 1.0f };  // カメラの上方向
	XMMATRIX matView = XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	XMStoreFloat4x4(&g_cbCBuffer.View, XMMatrixTranspose(matView));
	// 射影変換行列(パースペクティブ(透視法)射影)
	XMMATRIX matProj = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(30.0f),		// 視野角30°
			g_ViewPort[0].Width / g_ViewPort[0].Height,	// アスペクト比
			1.0f,							// 前方投影面までの距離
			20.0f);						// 後方投影面までの距離
	XMStoreFloat4x4(&g_cbCBuffer.Projection, XMMatrixTranspose(matProj));
	// 点光源座標
	XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&g_vLightPos), matView);
	XMStoreFloat3(&g_cbCBuffer.Light, vec);

	// ***************************************
    // 描画ターゲットのクリア
    g_pImmediateContext->ClearRenderTargetView(
                       g_pRenderTargetView, // クリアする描画ターゲット
                       g_ClearColor);         // クリアする値

	// 深度/ステンシルのクリア
	g_pImmediateContext->ClearDepthStencilView(
			g_pDepthStencilView, // クリアする深度/ステンシル・ビュー
			D3D11_CLEAR_DEPTH,   // 深度値だけをクリアする
			1.0f,                // 深度バッファをクリアする値
			0);                  // ステンシル・バッファをクリアする値(この場合、無関係)

	// ***************************************
	g_pImmediateContext->ClearState();

	// VSに定数バッファを設定
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);

	// PSに定数バッファを設定
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
	// PSにサンプラーを設定
	g_pImmediateContext->PSSetSamplers(0, 2, g_pTextureSampler);

	// RSにビューポートを設定
	g_pImmediateContext->RSSetViewports(1, g_ViewPort);

	// OMに描画ターゲット ビューと深度/ステンシル・ビューを設定
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_bDepthMode ? g_pDepthStencilView : NULL);

	// 物体の描画
	RenderObj(false);

	// ***************************************
	// バック バッファの表示
	hr = g_pSwapChain->Present(	0,	// 画面を直ぐに更新する
								0);	// 画面を実際に更新する

	return hr;
}

/*-------------------------------------------
	Direct3Dの終了処理
--------------------------------------------*/
bool CleanupDirect3D(void)
{
    // デバイス・ステートのクリア
    if(g_pImmediateContext)
        g_pImmediateContext->ClearState();

	// スワップ チェインをウインドウ モードにする
	if (g_pSwapChain)
		g_pSwapChain->SetFullscreenState(FALSE, NULL);

	// メッシュの解放
	g_wfMtl.Cleanup();
	g_wfObjKuma.Cleanup();

	// 取得したインターフェイスの開放
	SAFE_RELEASE(g_pShadowMap);
	SAFE_RELEASE(g_pShadowMapDSView);
	SAFE_RELEASE(g_pShadowMapSRView);

	SAFE_RELEASE(g_pTextureSampler[0]);
	SAFE_RELEASE(g_pTextureSampler[1]);

	SAFE_RELEASE(g_pDepthStencilState);
	SAFE_RELEASE(g_pBlendState);
	SAFE_RELEASE(g_pRasterizerStateShadow);
	SAFE_RELEASE(g_pRasterizerState);

	SAFE_RELEASE(g_pCBuffer);

	SAFE_RELEASE(g_pPixelShaderNoSM);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pVertexShaderShadow);
	SAFE_RELEASE(g_pVertexShader);

	SAFE_RELEASE(g_pInputLayout);

	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pDepthStencil);

	SAFE_RELEASE(g_pRenderTargetView);
	SAFE_RELEASE(g_pSwapChain);

    SAFE_RELEASE(g_pImmediateContext);
	SAFE_RELEASE(g_pD3DDevice);

	return true;
}

/*-------------------------------------------
	アプリケーションの終了処理
--------------------------------------------*/
bool CleanupApp(void)
{
	// ウインドウ クラスの登録解除
	UnregisterClass(g_szWndClass, g_hInstance);
	return true;
}

/*-------------------------------------------
	ウィンドウ処理
--------------------------------------------*/
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HRESULT hr = S_OK;
	BOOL fullscreen;

	switch(msg)
	{
	case WM_DESTROY:
		// Direct3Dの終了処理
		CleanupDirect3D();
		// ウインドウを閉じる
		PostQuitMessage(0);
		g_hWindow = NULL;
		return 0;

	// ウインドウ サイズの変更処理
	case WM_SIZE:
		if (!g_pD3DDevice || wParam == SIZE_MINIMIZED)
			break;

		// 描画ターゲットを解除する
		g_pImmediateContext->OMSetRenderTargets(0, NULL, NULL);	// 描画ターゲットの解除
		SAFE_RELEASE(g_pRenderTargetView);					    // 描画ターゲット ビューの解放
		SAFE_RELEASE(g_pDepthStencilView);					// 深度/ステンシル ビューの解放
		SAFE_RELEASE(g_pDepthStencil);						// 深度/ステンシル テクスチャの解放

		// バッファの変更
		g_pSwapChain->ResizeBuffers(3, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		// バック バッファの初期化
		InitBackBuffer();
		break;

	case WM_KEYDOWN:
		// キー入力の処理
		switch(wParam)
		{
		case VK_ESCAPE:	// [ESC]キーでウインドウを閉じる
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

//		case VK_F2:		// [F2]キーで深度バッファのモードを切り替える
//			g_bDepthMode = !g_bDepthMode;
//			break;

		case VK_F2:		// [F2]キーでシャドウ マッピングのモードを切り替える
			g_bShadowMappingMode = !g_bShadowMappingMode;
			break;

		case VK_F5:		// [F5]キーで画面モードを切り替える
			if (g_pSwapChain != NULL) {
				g_pSwapChain->GetFullscreenState(&fullscreen, NULL);
				g_pSwapChain->SetFullscreenState(!fullscreen, NULL);
			}
			break;

		case VK_UP:	//視点を近づける
			g_fEye -= 0.1f;
			if (g_fEye < 2.5f)
				g_fEye = 2.5f;
			break;

		case VK_DOWN:	// 視点を遠ざける
			g_fEye += 0.1f;
			if (g_fEye > 20.0f)
				g_fEye = 20.0f;
			break;
		}
		break;
	}

	// デフォルト処理
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*--------------------------------------------
	デバイスの消失処理
--------------------------------------------*/
HRESULT IsDeviceRemoved(void)
{
	HRESULT hr;

	// デバイスの消失確認
	hr = g_pD3DDevice->GetDeviceRemovedReason();
	switch (hr) {
	case S_OK:
		break;         // 正常

	case DXGI_ERROR_DEVICE_HUNG:
	case DXGI_ERROR_DEVICE_RESET:
		DXTRACE_ERR(L"IsDeviceRemoved g_pD3DDevice->GetDeviceRemovedReason", hr);
		CleanupDirect3D();   // Direct3Dの解放(アプリケーション定義)
		hr = InitDirect3D();  // Direct3Dの初期化(アプリケーション定義)
		if (FAILED(hr))
			return hr; // 失敗。アプリケーションを終了
		break;

	case DXGI_ERROR_DEVICE_REMOVED:
	case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
	case DXGI_ERROR_INVALID_CALL:
	default:
		DXTRACE_ERR(L"IsDeviceRemoved g_pD3DDevice->GetDeviceRemovedReason", hr);
		return hr;   // どうしようもないので、アプリケーションを終了。
	};

	return S_OK;         // 正常
}

/*--------------------------------------------
	アイドル時の処理
--------------------------------------------*/
bool AppIdle(void)
{
	if (!g_pD3DDevice)
		return false;

	HRESULT hr;
	// デバイスの消失処理
	hr = IsDeviceRemoved();
	if (FAILED(hr))
		return false;

	// スタンバイ モード
	if (g_bStandbyMode) {
		hr = g_pSwapChain->Present(0, DXGI_PRESENT_TEST);
		if (hr != S_OK) {
			Sleep(100);	// 0.1秒待つ
			return true;
		}
		g_bStandbyMode = false; // スタンバイ モードを解除する
	}

	// 画面の更新
	hr = Render();
	if (hr == DXGI_STATUS_OCCLUDED) {
		g_bStandbyMode = true;  // スタンバイ モードに入る
	}

	return true;
}

/*--------------------------------------------
	メイン
---------------------------------------------*/
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
	// デバッグ ヒープ マネージャによるメモリ割り当ての追跡方法を設定
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// XNA Mathライブラリのサポート チェック
    if (XMVerifyCPUSupport() != TRUE)
	{
		DXTRACE_MSG(L"WinMain XMVerifyCPUSupport");
		return 0;
	}

	// アプリケーションに関する初期化
	HRESULT hr = InitApp(hInst);
	if (FAILED(hr))
	{
		DXTRACE_ERR(L"WinMain InitApp", hr);
		return 0;
	}

	// Direct3Dの初期化
	hr = InitDirect3D();
	if (FAILED(hr)) {
		DXTRACE_ERR(L"WinMain InitDirect3D", hr);
		CleanupDirect3D();
		CleanupApp();
		return 0;
	}

	// メッセージ ループ
	MSG msg;
	do
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// アイドル処理
			if (!AppIdle())
				// エラーがある場合，アプリケーションを終了する
				DestroyWindow(g_hWindow);
		}
	} while (msg.message != WM_QUIT);

	// アプリケーションの終了処理
	CleanupApp();

	return (int)msg.wParam;
}
