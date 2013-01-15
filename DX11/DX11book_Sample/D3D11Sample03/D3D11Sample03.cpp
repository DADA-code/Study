/*----------------------------------------------------------
	Direct3D 11サンプル
		・Microsoft DirectX SDK (February 2010)
		・Visual Studio 2010 Express
		・Windows 7/Vista
		・シェーダモデル4.0
		対応

	D3D11Sample03.cpp
		「DXGIの機能」
--------------------------------------------------------------*/

#define STRICT					// 型チェックを厳密に行なう
#define WIN32_LEAN_AND_MEAN		// ヘッダーからあまり使われない関数を省く
#define WINVER        0x0600	// Windows Vista以降対応アプリを指定(なくてもよい)
#define _WIN32_WINNT  0x0600	// 同上

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// 解放マクロ

#include <windows.h>
#include <crtdbg.h>
#include <d3dx11.h>
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

/*-------------------------------------------
	グローバル変数(アプリケーション関連)
--------------------------------------------*/
HINSTANCE	g_hInstance		= NULL;	// インスタンス ハンドル
HWND		g_hWindow		= NULL;	// ウインドウ ハンドル

WCHAR		g_szAppTitle[]	= L"Direct3D 11 Sample03";
WCHAR		g_szWndClass[]	= L"D3D11S03";

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
IDXGIFactory*           g_pFactory = NULL;			// DXGI 
ID3D11Device*           g_pD3DDevice = NULL;        // デバイス
ID3D11DeviceContext*    g_pImmediateContext = NULL; // デバイス・コンテキスト
IDXGISwapChain*         g_pSwapChain = NULL;        // スワップ・チェイン

ID3D11RenderTargetView* g_pRenderTargetView = NULL; // 描画ターゲット・ビュー
D3D11_VIEWPORT          g_ViewPort[1];				// ビューポート

// スタンバイモード
bool g_bStandbyMode = false;

// 描画ターゲットをクリアする値(RGBA)
float g_ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };

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
	sd.BufferCount       = 1;		// バック バッファ数
	sd.BufferDesc.Width  = width;	// バック バッファの幅
	sd.BufferDesc.Height = height;	// バック バッファの高さ
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // フォーマット
	sd.BufferDesc.RefreshRate.Numerator = 60;  // リフレッシュ レート(分子)
	sd.BufferDesc.RefreshRate.Denominator = 1; // リフレッシュ レート(分母)
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

	// バック バッファの初期化
	hr = InitBackBuffer();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D InitBackBuffer", hr);

	// IDXGIFactoryインターフェイスの取得
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&g_pFactory));
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D CreateDXGIFactory", hr);

	// [Alt]+[Enter]キーによる画面モード切り替え機能を設定する
	hr = g_pFactory->MakeWindowAssociation(
				g_hWindow, // 画面モード切り替え機能を設定するウインドウ ハンドル
				0);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pFactory->MakeWindowAssociation", hr);

	return S_OK;
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

    // バック・バッファの描画ターゲット・ビューを作る
    hr = g_pD3DDevice->CreateRenderTargetView(
            pBackBuffer,           // ビューでアクセスするリソース
            NULL,                  // 描画ターゲット・ビューの定義
            &g_pRenderTargetView); // 描画ターゲット・ビューを受け取る変数
    SAFE_RELEASE(pBackBuffer);  // 以降、バック・バッファは直接使わないので解放
    if(FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateRenderTargetView", hr);  // 失敗

    // ビューポートの設定
    g_ViewPort[0].TopLeftX = 0.0f;    // ビューポート領域の左上X座標。
    g_ViewPort[0].TopLeftY = 0.0f;    // ビューポート領域の左上Y座標。
    g_ViewPort[0].Width    = 640.0f;  // ビューポート領域の幅
    g_ViewPort[0].Height   = 480.0f;  // ビューポート領域の高さ
    g_ViewPort[0].MinDepth = 0.0f; // ビューポート領域の深度値の最小値
    g_ViewPort[0].MaxDepth = 1.0f; // ビューポート領域の深度値の最大値

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
                       g_ClearColor);       // クリアする値

    // ラスタライザにビューポートを設定
    g_pImmediateContext->RSSetViewports(1, g_ViewPort);

    // 描画ターゲット・ビューを出力マージャーの描画ターゲットとして設定
    g_pImmediateContext->OMSetRenderTargets(
            1,                    // 描画ターゲットの数
            &g_pRenderTargetView, // 描画ターゲット・ビューの配列
            NULL);                // 深度/ステンシル・ビューを設定しない

	// 描画(省略)

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
	SAFE_RELEASE(g_pRenderTargetView);
	SAFE_RELEASE(g_pSwapChain);
    SAFE_RELEASE(g_pImmediateContext);
	SAFE_RELEASE(g_pD3DDevice);
	SAFE_RELEASE(g_pFactory);

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

		// バッファの変更
		g_pSwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

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

		case VK_F5:		// [F5]キーで画面モードを切り替える
			if (g_pSwapChain != NULL) {
				g_pSwapChain->GetFullscreenState(&fullscreen, NULL);
				g_pSwapChain->SetFullscreenState(!fullscreen, NULL);
			}
			break;

		case VK_F6:		// [F6]キーでディスプレイ モードを設定する
			if (g_pSwapChain != NULL) {
				// ディスプレイ モードの変更
				DXGI_MODE_DESC desc;
				desc.Width  = 800;
				desc.Height = 600;
				desc.RefreshRate.Numerator   = 60;
				desc.RefreshRate.Denominator = 1;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				hr = g_pSwapChain->ResizeTarget(&desc);
				if (FAILED(hr))
					DXTRACE_ERR(L"MainWndProc g_pSwapChain->ResizeTarget", hr);  // 失敗
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
		DXTRACE_MSG(L"スタンバイ モードを解除。");
	}

	// 画面の更新
	hr = Render();
	if (hr == DXGI_STATUS_OCCLUDED) {
		g_bStandbyMode = true;  // スタンバイ モードに入る
		DXTRACE_MSG(L"スタンバイ モードを設定。");

		// 描画ターゲットをクリアする色の変更
		float c = g_ClearColor[0];
		g_ClearColor[0] = g_ClearColor[1];
		g_ClearColor[1] = g_ClearColor[2];
		g_ClearColor[2] = g_ClearColor[3];
		g_ClearColor[3] = c;
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
