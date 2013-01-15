/*----------------------------------------------------------
	Direct3D 11サンプル
		・Microsoft DirectX SDK (February 2010)
		・Visual Studio 2010 Express
		・Windows 7/Vista
		・シェーダモデル4.0
		対応

	D3D11Sample05.cpp
		「基本的な描画手順」
--------------------------------------------------------------*/

#define STRICT					// 型チェックを厳密に行なう
#define WIN32_LEAN_AND_MEAN		// ヘッダーからあまり使われない関数を省く
#define WINVER        0x0600	// Windows Vista以降対応アプリを指定(なくてもよい)
#define _WIN32_WINNT  0x0600	// 同上

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// 解放マクロ

#include <windows.h>
#include <mmsystem.h>
#include <crtdbg.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <dxerr.h>

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

WCHAR		g_szAppTitle[]	= L"Direct3D 11 Sample05";
WCHAR		g_szWndClass[]	= L"D3D11S05";

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
ID3D11Buffer*             g_pVerBuffer[2] = { NULL, NULL }; // 頂点バッファのインターフェイス
ID3D11Buffer*             g_pIdxBuffer = NULL;              // インデックス・バッファのインターフェイス

ID3D11VertexShader*       g_pVertexShader = NULL;   // 頂点シェーダ
ID3D11GeometryShader*     g_pGeometryShader = NULL; // ジオメトリ・シェーダ
ID3D11PixelShader*        g_pPixelShader = NULL;    // ピクセル・シェーダ
ID3D11Buffer*             g_pCBuffer[3] = { NULL, NULL, NULL }; // 定数バッファ

ID3D11BlendState*         g_pBlendState = NULL;			// ブレンド・ステート・オブジェクト
ID3D11RasterizerState*	  g_pRasterizerState = NULL;	// ラスタライザ・ステート・オブジェクト
ID3D11DepthStencilState*  g_pDepthStencilState = NULL;	// 深度/ステンシル・ステート・オブジェクト

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

XMFLOAT3 g_vLightPos(3.0f, 3.0f, -3.0f);   // 光源座標(ワールド座標系)

// 頂点バッファ①に納めるデータ形式
struct XYZBuffer
{
	XMFLOAT3 Position;   // 座標値
};

// 頂点バッファ②に納めるデータ形式
struct ColBuffer
{
	XMFLOAT3 Color;     // 色
};

// 定数バッファのデータ定義①　変更しないデータ
struct cbNeverChanges {
	XMFLOAT4X4 Projection;   // 透視変換行列
};

// 定数バッファのデータ定義②　変更頻度の低いデータ
struct cbChangesEveryFrame {
	XMFLOAT4X4  View;  // ビュー変換行列
	XMFLOAT3    Light; // 光源座標(ワールド座標系)
	FLOAT       dummy; // ダミー
};

// 定数バッファのデータ定義③　変更頻度の高いデータ
struct cbChangesEveryObject {
	XMFLOAT4X4 World;      // ワールド変換行列
};

// 定数バッファのデータ
struct cbNeverChanges       g_cbNeverChanges;       // 透視変換行列
struct cbChangesEveryFrame  g_cbChangesEveryFrame;  // ビュー変換行列　光源座標
struct cbChangesEveryObject g_cbChangesEveryObject; // ワールド変換行列

/*-------------------------------------------
	関数定義
--------------------------------------------*/

LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);
HRESULT InitBackBuffer(void);

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
	// 頂点バッファ①の定義
	D3D11_BUFFER_DESC xyzBufferDesc;
	xyzBufferDesc.Usage          = D3D11_USAGE_DEFAULT;      // デフォルト使用法
	xyzBufferDesc.ByteWidth      = sizeof(XYZBuffer) * 8;    // 8頂点
	xyzBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER; // 頂点バッファ
	xyzBufferDesc.CPUAccessFlags = 0;
	xyzBufferDesc.MiscFlags      = 0;
	xyzBufferDesc.StructureByteStride = 0;

	// 頂点バッファ①のサブリソースの初期値(頂点座標)
	struct XYZBuffer posVertex[] = {
		XMFLOAT3(-1.0f,1.0f,-1.0f), XMFLOAT3(1.0f,1.0f,-1.0f),
		XMFLOAT3(1.0f,-1.0f,-1.0f), XMFLOAT3(-1.0f,-1.0f,-1.0f),
		XMFLOAT3(-1.0f,1.0f,1.0f), XMFLOAT3(1.0f,1.0f,1.0f),
		XMFLOAT3(1.0f,-1.0f,1.0f), XMFLOAT3(-1.0f,-1.0f,1.0f)
	};

	// 頂点バッファ①のサブリソースの定義
	D3D11_SUBRESOURCE_DATA xyzSubData;
	xyzSubData.pSysMem          = posVertex;  // バッファ・データの初期値
	xyzSubData.SysMemPitch      = 0;
	xyzSubData.SysMemSlicePitch = 0;

	// 頂点バッファ①の作成
	hr = g_pD3DDevice->CreateBuffer(&xyzBufferDesc, &xyzSubData, &g_pVerBuffer[0]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// **********************************************************
	// 頂点バッファ②の定義
	D3D11_BUFFER_DESC colBufferDesc;
	colBufferDesc.Usage          = D3D11_USAGE_DEFAULT;      // デフォルト使用法
	colBufferDesc.ByteWidth      = sizeof(ColBuffer) * 8;    // 8頂点
	colBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER; // 頂点バッファ
	colBufferDesc.CPUAccessFlags = 0;
	colBufferDesc.MiscFlags      = 0;
	colBufferDesc.StructureByteStride = 0;

	// 頂点バッファ②のサブリソースの初期値(頂点色)
	struct ColBuffer colVertex[] = {
		XMFLOAT3(0.0f,0.0f,0.0f), XMFLOAT3(0.0f,0.0f,1.0f),
		XMFLOAT3(0.0f,1.0f,0.0f), XMFLOAT3(0.0f,1.0f,1.0f),
		XMFLOAT3(1.0f,0.0f,0.0f), XMFLOAT3(1.0f,0.0f,1.0f),
		XMFLOAT3(1.0f,1.0f,0.0f), XMFLOAT3(1.0f,1.0f,1.0f)
	};

	// 頂点バッファ②のサブリソースの定義
	D3D11_SUBRESOURCE_DATA colSubData;
	colSubData.pSysMem          = colVertex;  // バッファの初期値
	colSubData.SysMemPitch      = 0;
	colSubData.SysMemSlicePitch = 0;

	// 頂点バッファ②のサブリソースの作成
	hr = g_pD3DDevice->CreateBuffer(&colBufferDesc, &colSubData, &g_pVerBuffer[1]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// **********************************************************
	// インデックス・バッファの定義
	D3D11_BUFFER_DESC idxBufferDesc;
	idxBufferDesc.Usage          = D3D11_USAGE_DEFAULT;     // デフォルト使用法
	idxBufferDesc.ByteWidth      = sizeof(UINT) * 36;       // 12×3頂点
	idxBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER; // インデックス・バッファ
	idxBufferDesc.CPUAccessFlags = 0;
	idxBufferDesc.MiscFlags      = 0;
	idxBufferDesc.StructureByteStride = 0;

	// インデックス・バッファのサブリソースの初期値(頂点ID)
	UINT idxVertexID[] = {
		0,1,3,  1,2,3,  1,5,2,  5,6,2,  5,4,6,  4,7,6,
		4,5,0,  5,1,0,  4,0,7,  0,3,7,  3,2,7,  2,6,7
	};

	// インデックス・バッファのサブリソースの定義
	D3D11_SUBRESOURCE_DATA idxSubData;
	idxSubData.pSysMem          = idxVertexID;  // バッファ・データの初期値
	idxSubData.SysMemPitch      = 0;
	idxSubData.SysMemSlicePitch = 0;

	// インデックス・バッファの作成
	hr = g_pD3DDevice->CreateBuffer(&idxBufferDesc, &idxSubData, &g_pIdxBuffer);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// **********************************************************
	// 頂点シェーダのコードをコンパイル
	ID3DBlob* pBlobVS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample05.sh",  // ファイル名
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
		return DXTRACE_ERR(L"InitDirect3D D3DX11CompileShaderFromFile", hr);

	// 頂点シェーダの作成
	hr = g_pD3DDevice->CreateVertexShader(
			pBlobVS->GetBufferPointer(), // バイト・コードへのポインタ
			pBlobVS->GetBufferSize(),    // バイト・コードの長さ
			NULL,
			&g_pVertexShader); // 頂点シェーダを受け取る変数
//	SAFE_RELEASE(pBlobVS);  // バイト・コードを解放
	if (FAILED(hr)) {
		SAFE_RELEASE(pBlobVS);
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateVertexShader", hr);
	}

	// **********************************************************
	// 入力要素
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
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
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateInputLayout", hr);

	// **********************************************************
	// ジオメトリ・シェーダのコードをコンパイル
	ID3DBlob* pBlobGS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample05.sh",  // ファイル名
			NULL,          // マクロ定義(なし)
			NULL,          // インクルード・ファイル定義(なし)
			"GS",          // 「GS関数」がシェーダから実行される
			"gs_4_0",      // ジオメトリ・シェーダ
			g_flagCompile, // コンパイル・オプション
			0,             // エフェクトのコンパイル・オプション(なし)
			NULL,          // 直ぐにコンパイルしてから関数を抜ける。
			&pBlobGS,      // コンパイルされたバイト・コード
			NULL,          // エラーメッセージは不要
			NULL);         // 戻り値
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D D3DX11CompileShaderFromFile", hr);

	// ジオメトリ・シェーダの作成
	hr = g_pD3DDevice->CreateGeometryShader(
			pBlobGS->GetBufferPointer(), // バイト・コードへのポインタ
			pBlobGS->GetBufferSize(),    // バイト・コードの長さ
			NULL,
			&g_pGeometryShader); // ジオメトリ・シェーダを受け取る変数
	SAFE_RELEASE(pBlobGS);  // バイト・コードを解放
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreatePixelShader", hr);

	// **********************************************************
	// ピクセル・シェーダのコードをコンパイル
	ID3DBlob* pBlobPS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample05.sh",  // ファイル名
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
		return DXTRACE_ERR(L"InitDirect3D D3DX11CompileShaderFromFile", hr);

	// ピクセル・シェーダの作成
	hr = g_pD3DDevice->CreatePixelShader(
			pBlobPS->GetBufferPointer(), // バイト・コードへのポインタ
			pBlobPS->GetBufferSize(),    // バイト・コードの長さ
			NULL,
			&g_pPixelShader); // ピクセル・シェーダを受け取る変数
	SAFE_RELEASE(pBlobPS);  // バイト・コードを解放
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreatePixelShader", hr);

	// **********************************************************
	// 定数バッファの定義
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;    // 動的(ダイナミック)使用法
	cBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER; // 定数バッファ
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPUから書き込む
	cBufferDesc.MiscFlags      = 0;
	cBufferDesc.StructureByteStride = 0;

	// 定数バッファ①の作成
	cBufferDesc.ByteWidth      = sizeof(cbNeverChanges); // バッファ・サイズ
	hr = g_pD3DDevice->CreateBuffer(&cBufferDesc, NULL, &g_pCBuffer[0]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// 定数バッファ②の作成
	cBufferDesc.ByteWidth      = sizeof(cbChangesEveryFrame); // バッファ・サイズ
	hr = g_pD3DDevice->CreateBuffer(&cBufferDesc, NULL, &g_pCBuffer[1]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// 定数バッファ③の作成
	cBufferDesc.ByteWidth      = sizeof(cbChangesEveryObject); // バッファ・サイズ
	hr = g_pD3DDevice->CreateBuffer(&cBufferDesc, NULL, &g_pCBuffer[2]);
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

	// ラスタライザ・ステート・オブジェクトの作成
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;   // 普通に描画する
	RSDesc.CullMode = D3D11_CULL_NONE;    // 両面を描画する
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
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateRasterizerState", hr);

	// **********************************************************
	// 深度/ステンシル・ステート・オブジェクトの作成
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable      = TRUE; // 深度テストあり
	DepthStencil.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ALL; // 書き込む
	DepthStencil.DepthFunc        = D3D11_COMPARISON_LESS; // 手前の物体を描画
	DepthStencil.StencilEnable    = FALSE; // ステンシル・テストなし
	DepthStencil.StencilReadMask  = 0;     // ステンシル読み込みマスク。
	DepthStencil.StencilWriteMask = 0;     // ステンシル書き込みマスク。
			// 面が表を向いている場合のステンシル・テストの設定
	DepthStencil.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilFunc        = D3D11_COMPARISON_NEVER; // 常に失敗
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
	// バック バッファの初期化
	hr = InitBackBuffer();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D InitBackBuffer", hr);

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

	// 定数バッファ①を更新
	// 射影変換行列(パースペクティブ(透視法)射影)
	XMMATRIX mat = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(30.0f),		// 視野角30°
			(float)descBackBuffer.Width / (float)descBackBuffer.Height,	// アスペクト比
			1.0f,							// 前方投影面までの距離
			20.0f);							// 後方投影面までの距離
	mat = XMMatrixTranspose(mat);
	XMStoreFloat4x4(&g_cbNeverChanges.Projection, mat);
	// 定数バッファ①のマップ取得
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	hr = g_pImmediateContext->Map(
	                  g_pCBuffer[0],           // マップするリソース
	                  0,                       // サブリソースのインデックス番号
	                  D3D11_MAP_WRITE_DISCARD, // 書き込みアクセス
	                  0,                       //
	                  &MappedResource);        // データの書き込み先ポインタ
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer  g_pImmediateContext->Map", hr);  // 失敗
	// データ書き込み
	CopyMemory(MappedResource.pData, &g_cbNeverChanges, sizeof(cbNeverChanges));
	// マップ解除
	g_pImmediateContext->Unmap(g_pCBuffer[0], 0);

	//サイズを保存
	g_sizeWindow.cx = descBackBuffer.Width;
	g_sizeWindow.cy = descBackBuffer.Height;

	return S_OK;
}

/*--------------------------------------------
	画面の描画処理
--------------------------------------------*/
HRESULT Render(void)
{
	HRESULT hr;

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
	// 立方体の描画

	// 定数バッファ②を更新
	// ビュー変換行列
	XMVECTORF32 eyePosition   = { 0.0f, 5.0f, -5.0f, 1.0f };  // 視点(カメラの位置)
	XMVECTORF32 focusPosition = { 0.0f, 0.0f,  0.0f, 1.0f };  // 注視点
	XMVECTORF32 upDirection   = { 0.0f, 1.0f,  0.0f, 1.0f };  // カメラの上方向
	XMMATRIX mat = XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	XMStoreFloat4x4(&g_cbChangesEveryFrame.View, XMMatrixTranspose(mat));
	// 点光源座標
	XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&g_vLightPos), mat);
	XMStoreFloat3(&g_cbChangesEveryFrame.Light, vec);
	// 定数バッファ②のマップ取得
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	hr = g_pImmediateContext->Map(
	                  g_pCBuffer[1],           // マップするリソース
	                  0,                       // サブリソースのインデックス番号
	                  D3D11_MAP_WRITE_DISCARD, // 書き込みアクセス
	                  0,                       //
	                  &MappedResource);        // データの書き込み先ポインタ
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer  g_pImmediateContext->Map", hr);  // 失敗
	// データ書き込み
	CopyMemory(MappedResource.pData, &g_cbChangesEveryFrame, sizeof(cbChangesEveryFrame));
	// マップ解除
	g_pImmediateContext->Unmap(g_pCBuffer[1], 0);

	// 定数バッファ③を更新
	// ワールド変換行列
	XMMATRIX matY, matX;
	FLOAT rotate = (FLOAT)(XM_PI * (timeGetTime() % 3000)) / 1500.0f;
	matY = XMMatrixRotationY(rotate);
	rotate = (FLOAT)(XM_PI * (timeGetTime() % 1500)) / 750.0f;
	matX = XMMatrixRotationX(rotate);
	XMStoreFloat4x4(&g_cbChangesEveryObject.World, XMMatrixTranspose(matY * matX));
	// 定数バッファ③のマップ取得
	hr = g_pImmediateContext->Map(
	                  g_pCBuffer[2],           // マップするリソース
	                  0,                       // サブリソースのインデックス番号
	                  D3D11_MAP_WRITE_DISCARD, // 書き込みアクセス
	                  0,                       //
	                  &MappedResource);        // データの書き込み先ポインタ
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer  g_pImmediateContext->Map", hr);  // 失敗
	// データ書き込み
	CopyMemory(MappedResource.pData, &g_cbChangesEveryObject, sizeof(cbChangesEveryObject));
	// マップ解除
	g_pImmediateContext->Unmap(g_pCBuffer[2], 0);

	// ***************************************
	// IAに頂点バッファを設定
	UINT strides[2] = { sizeof(XYZBuffer), sizeof(ColBuffer) };
	UINT offsets[2] = { 0, 0 };
	g_pImmediateContext->IASetVertexBuffers(0, 2, g_pVerBuffer, strides, offsets);
	// IAにインデックス・バッファを設定
	g_pImmediateContext->IASetIndexBuffer(g_pIdxBuffer, DXGI_FORMAT_R32_UINT, 0);
	// IAに入力レイアウト・オブジェクトを設定
	g_pImmediateContext->IASetInputLayout(g_pInputLayout);
	// IAにプリミティブの種類を設定
	g_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// VSに頂点シェーダを設定
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	// VSに定数バッファを設定
	g_pImmediateContext->VSSetConstantBuffers(0, 3, g_pCBuffer);

	// GSにジオメトリ・シェーダを設定
	g_pImmediateContext->GSSetShader(g_pGeometryShader, NULL, 0);
	// GSに定数バッファを設定
	g_pImmediateContext->GSSetConstantBuffers(0, 3, g_pCBuffer);

	// RSにビューポートを設定
	g_pImmediateContext->RSSetViewports(1, g_ViewPort);
	// RSにラスタライザ・ステート・オブジェクトを設定
	g_pImmediateContext->RSSetState(g_pRasterizerState);

	// PSにピクセル・シェーダを設定
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	// PSに定数バッファを設定
	g_pImmediateContext->PSSetConstantBuffers(0, 3, g_pCBuffer);

	// OMに描画ターゲット ビューと深度/ステンシル・ビューを設定
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_bDepthMode ? g_pDepthStencilView : NULL);
	// OMにブレンド・ステート・オブジェクトを設定
	FLOAT BlendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	g_pImmediateContext->OMSetBlendState(g_pBlendState, BlendFactor, 0xffffffff);
	// OMに深度/ステンシル・ステート・オブジェクトを設定
	g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState, 0);

	// ***************************************
	// 頂点バッファのデータをインデックス・バッファを使って描画する
	g_pImmediateContext->DrawIndexed(
			36, // 描画するインデックス数(頂点数)
			0,  // インデックス・バッファの最初のインデックスから描画開始
			0); // 頂点バッファ内の最初の頂点データから使用開始

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

	// 取得したインターフェイスの開放
	SAFE_RELEASE(g_pDepthStencilState);
	SAFE_RELEASE(g_pBlendState);
	SAFE_RELEASE(g_pRasterizerState);

	SAFE_RELEASE(g_pCBuffer[2]);
	SAFE_RELEASE(g_pCBuffer[1]);
	SAFE_RELEASE(g_pCBuffer[0]);

	SAFE_RELEASE(g_pInputLayout);

	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pGeometryShader);
	SAFE_RELEASE(g_pVertexShader);

	SAFE_RELEASE(g_pIdxBuffer);
	SAFE_RELEASE(g_pVerBuffer[1]);
	SAFE_RELEASE(g_pVerBuffer[0]);

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

		case VK_F2:		// [F2]キーで深度バッファのモードを切り替える
			g_bDepthMode = !g_bDepthMode;
			break;

		case VK_F5:		// [F5]キーで画面モードを切り替える
			if (g_pSwapChain != NULL) {
				g_pSwapChain->GetFullscreenState(&fullscreen, NULL);
				g_pSwapChain->SetFullscreenState(!fullscreen, NULL);
			}
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
