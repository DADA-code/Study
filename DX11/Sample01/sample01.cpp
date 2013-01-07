#define STRICT                     // 型チェックを厳密に行う
#define WIN32_LEAN_AND_MEAN        // ヘッダーからまり使われない関数を省く。
#define WINVER        0x0600       // Windows Vista移行対応アプリ
#define _WIN32_WINNT  0x0600       // Windows Vista移行対応アプリ


#include <Windows.h>

#include <D3D11.h>
#include <D3DX11.h>
#include <DxErr.h>

#define SAFE_RELEASE(x) { if(x) { (x)->Release(); (x)=NULL; } } //解放マクロ


// グローバル変数
HINSTANCE g_Instance_Handle = NULL;
HWND      g_Window_Handle   = NULL;

WCHAR     g_App_Title[]         = L"Direct3D 11 Sample";
WCHAR     g_Window_Class_Name[] = L"D3D11S01";

//起動時の描画領域サイズ
SIZE      g_Initial_Client_Erea_Size = { 640, 480 };  // ウィンドウのクライアント領域

// グローバル変数(Direct3D関連)
// 機能レベルの配列
D3D_FEATURE_LEVEL g_Feature_Levels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
UINT              g_Num_Of_Feature_Levels    = 3; // 配列の要素数
D3D_FEATURE_LEVEL g_Feature_Levels_Supported;     // デバイス作成時に返される機能レベル

// インターフェイス
ID3D11Device*        g_D3DDevice                = NULL;
ID3D11DeviceContext* g_Immediate_Device_Context = NULL;
IDXGISwapChain*      g_Swap_Chain               = NULL;

ID3D11RenderTargetView* g_Render_Target_View    = NULL;
D3D11_VIEWPORT          g_Viewport[1] ;

ID3D11Texture2D*        g_Depth_Stencil_Texture;
ID3D11DepthStencilView* g_Depth_Stencil_View; 


// 関数定義
LRESULT CALLBACK MainWndProc(HWND window_handle, UINT message, UINT parameter, LONG long_paramter);
HRESULT          InitBackBuffer(void);


HRESULT InitApp(HINSTANCE instance_handle) {
	// アプリケーションのインスタンスハンドルを保存
	g_Instance_Handle = instance_handle;

	// ウインドウクラスの登録
	WNDCLASS window_class;
	window_class.style         = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc   = (WNDPROC)MainWndProc;
	window_class.cbClsExtra    = 0;
	window_class.cbWndExtra    = 0;
	window_class.hInstance     = instance_handle;
//	window_class.hIcon         = LoadIcon(instance_handle, (LPCTSTR)IDI_ICON1);
	window_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
	window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	window_class.lpszMenuName  = NULL;
	window_class.lpszClassName = g_Window_Class_Name;

	if(!RegisterClass( &window_class)) {
		return DXTRACE_ERR(L"InitApp", GetLastError());
	}

	// メインウィンドウ作成
	RECT rect;
	rect.top    = 0;
	rect.left   = 0;
	rect.right  = g_Initial_Client_Erea_Size.cx;
	rect.bottom =  g_Initial_Client_Erea_Size.cy;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

	g_Window_Handle = CreateWindow(g_Window_Class_Name, g_App_Title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, instance_handle, NULL);
	if(g_Window_Handle == NULL) {
		return DXTRACE_ERR(L"InitApp", GetLastError());
	}

	// ウィンドウ表示
	ShowWindow(g_Window_Handle, SW_SHOWNORMAL);
	UpdateWindow(g_Window_Handle);

	return S_OK;
}

//
// Direct3D 初期化
HRESULT InitDirect3D(void)
{
	// ウィンドウのクライアント　エリア
	RECT rect;
	GetClientRect(g_Window_Handle, &rect);
	UINT width  = rect.right  - rect.left;
	UINT height = rect.bottom - rect.top;

	// デバイスとスワップチェインの作成
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
	swap_chain_desc.BufferCount                        = 1;
	swap_chain_desc.BufferDesc.Width                   = 640;
	swap_chain_desc.BufferDesc.Height                  = 480;
	swap_chain_desc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator   = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow                       = g_Window_Handle;
	swap_chain_desc.SampleDesc.Count                   = 1;
	swap_chain_desc.SampleDesc.Quality                 = 0;
	swap_chain_desc.Windowed                           = TRUE;
	swap_chain_desc.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

#if defined(DEBUG) || defined(_DEBUG)
	UINT create_device_flags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT create_device_flags = 0;
#endif

	// ハードウェアデバイスを作成
	HRESULT result = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		create_device_flags,
		g_Feature_Levels,
		g_Num_Of_Feature_Levels,
		D3D11_SDK_VERSION,
		&swap_chain_desc,
		&g_Swap_Chain,
		&g_D3DDevice,
		&g_Feature_Levels_Supported,
		&g_Immediate_Device_Context );

	if(FAILED(result)) {
		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_WARP,
			NULL,
			create_device_flags,
			g_Feature_Levels,
			g_Num_Of_Feature_Levels,
			D3D11_SDK_VERSION,
			&swap_chain_desc,
			&g_Swap_Chain,
			&g_D3DDevice,
			&g_Feature_Levels_Supported,
			&g_Immediate_Device_Context );

		if( FAILED(result)) {
			HRESULT hr = D3D11CreateDeviceAndSwapChain(
				NULL,
				D3D_DRIVER_TYPE_REFERENCE,
				NULL,
				create_device_flags,
				g_Feature_Levels,
				g_Num_Of_Feature_Levels,
				D3D11_SDK_VERSION,
				&swap_chain_desc,
				&g_Swap_Chain,
				&g_D3DDevice,
				&g_Feature_Levels_Supported,
				&g_Immediate_Device_Context );

			if(FAILED(result)) {
                return DXTRACE_ERR(L"InitDirect3D D3D11CreateDeviceAndSwapChain", result);
            }
		}
	}

	// バック　バッファの初期化
	result = InitBackBuffer();
	if( FAILED(result)){
		return DXTRACE_ERR(L"InitDirect3D InitBackBuffer", result);
	}

	return S_OK;
}

//
// バックバッファの初期化
HRESULT InitBackBuffer() {
	HRESULT result;

	//スワップチェインから最初のバックバッファを取得する。
	ID3D11Texture2D *p_back_buffer; //バッファのアクセスに使うインターフェース
	result = g_Swap_Chain->GetBuffer(0,	__uuidof(ID3D11Texture2D),(LPVOID*)&p_back_buffer );
	if(FAILED(result)) {
		return DXTRACE_ERR(L"InitBackBuffer g_Swap_Cahin->Getbuffer", result);
	}

	//バックバッファの情報
	D3D11_TEXTURE2D_DESC back_buffer_desc;
	p_back_buffer->GetDesc(&back_buffer_desc);

	//バックバッファの描画ターゲットビューを作る
	result = g_D3DDevice->CreateRenderTargetView( p_back_buffer, NULL, &g_Render_Target_View );
	SAFE_RELEASE(p_back_buffer);
	if(FAILED(result)) {
		return DXTRACE_ERR(L"InitBackBuffer g_DRDevice->CreateRenderTargetView", result); //失敗
	}

	// 深度/ステンシル・テクスチャの作成
	D3D11_TEXTURE2D_DESC depth_stencil_desc = back_buffer_desc;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format    = DXGI_FORMAT_D32_FLOAT;
	depth_stencil_desc.Usage     = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags      = 0;
	result = g_D3DDevice->CreateTexture2D(
		&depth_stencil_desc,
		NULL,
		&g_Depth_Stencil_Texture);
	if( FAILED(result)) {
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateTexture2D", result); // 失敗
	}

	// 深度/ステンシルビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	depth_stencil_view_desc.Format = depth_stencil_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Flags  = 0;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	result = g_D3DDevice->CreateDepthStencilView( g_Depth_Stencil_Texture, &depth_stencil_view_desc, &g_Depth_Stencil_View);
	if(FAILED(result)) {
		return DXTRACE_ERR(L"InitBackBuffer g_D3DDevice->CreateDepthStencilView", result);
	}

	// ビューポートの設定
	g_Viewport[0].TopLeftX = 0.0f;
	g_Viewport[0].TopLeftY = 0.0f;
	g_Viewport[0].Width    = 640.0f;
	g_Viewport[0].Height   = 480.0f;
	g_Viewport[0].MinDepth = 0.0f;
	g_Viewport[0].MaxDepth = 1.0f;

	return S_OK;
}