/*----------------------------------------------------------
	Direct3D 11�T���v��
		�EMicrosoft DirectX SDK (February 2010)
		�EVisual Studio 2010 Express
		�EWindows 7/Vista
		�E�V�F�[�_���f��4.0
		�Ή�

	D3D11Sample03.cpp
		�uDXGI�̋@�\�v
--------------------------------------------------------------*/

#define STRICT					// �^�`�F�b�N�������ɍs�Ȃ�
#define WIN32_LEAN_AND_MEAN		// �w�b�_�[���炠�܂�g���Ȃ��֐����Ȃ�
#define WINVER        0x0600	// Windows Vista�ȍ~�Ή��A�v�����w��(�Ȃ��Ă��悢)
#define _WIN32_WINNT  0x0600	// ����

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// ����}�N��

#include <windows.h>
#include <crtdbg.h>
#include <d3dx11.h>
#include <dxerr.h>

#include "resource.h"

// �K�v�ȃ��C�u�����������N����
#pragma comment( lib, "d3d11.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx11d.lib" )
#else
#pragma comment( lib, "d3dx11.lib" )
#endif
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "dxgi.lib" )

/*-------------------------------------------
	�O���[�o���ϐ�(�A�v���P�[�V�����֘A)
--------------------------------------------*/
HINSTANCE	g_hInstance		= NULL;	// �C���X�^���X �n���h��
HWND		g_hWindow		= NULL;	// �E�C���h�E �n���h��

WCHAR		g_szAppTitle[]	= L"Direct3D 11 Sample03";
WCHAR		g_szWndClass[]	= L"D3D11S03";

// �N�����̕`��̈�T�C�Y
SIZE		g_sizeWindow	= { 640, 480 };		// �E�C���h�E�̃N���C�A���g�̈�

/*-------------------------------------------
	�O���[�o���ϐ�(Direct3D�֘A)
--------------------------------------------*/

//�@�\���x���̔z��
D3D_FEATURE_LEVEL g_pFeatureLevels[] =  { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 }; 
UINT              g_FeatureLevels    = 3;   // �z��̗v�f��
D3D_FEATURE_LEVEL g_FeatureLevelsSupported; // �f�o�C�X�쐬���ɕԂ����@�\���x��

// �C���^�[�t�F�C�X
IDXGIFactory*           g_pFactory = NULL;			// DXGI 
ID3D11Device*           g_pD3DDevice = NULL;        // �f�o�C�X
ID3D11DeviceContext*    g_pImmediateContext = NULL; // �f�o�C�X�E�R���e�L�X�g
IDXGISwapChain*         g_pSwapChain = NULL;        // �X���b�v�E�`�F�C��

ID3D11RenderTargetView* g_pRenderTargetView = NULL; // �`��^�[�Q�b�g�E�r���[
D3D11_VIEWPORT          g_ViewPort[1];				// �r���[�|�[�g

// �X�^���o�C���[�h
bool g_bStandbyMode = false;

// �`��^�[�Q�b�g���N���A����l(RGBA)
float g_ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };

/*-------------------------------------------
	�֐���`
--------------------------------------------*/

LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);
HRESULT InitBackBuffer(void);

/*-------------------------------------------
	�A�v���P�[�V����������
--------------------------------------------*/
HRESULT InitApp(HINSTANCE hInst)
{
	// �A�v���P�[�V�����̃C���X�^���X �n���h����ۑ�
	g_hInstance = hInst;

	// �E�C���h�E �N���X�̓o�^
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

	// ���C�� �E�C���h�E�쐬
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

	// �E�C���h�E�\��
	ShowWindow(g_hWindow, SW_SHOWNORMAL);
	UpdateWindow(g_hWindow);

	return S_OK;
}

/*-------------------------------------------
	Direct3D������
--------------------------------------------*/
HRESULT InitDirect3D(void)
{
	// �E�C���h�E�̃N���C�A���g �G���A
	RECT rc;
	GetClientRect(g_hWindow, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// �f�o�C�X�ƃX���b�v �`�F�C���̍쐬
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));	// �\���̂̒l��������
	sd.BufferCount       = 1;		// �o�b�N �o�b�t�@��
	sd.BufferDesc.Width  = width;	// �o�b�N �o�b�t�@�̕�
	sd.BufferDesc.Height = height;	// �o�b�N �o�b�t�@�̍���
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // �t�H�[�}�b�g
	sd.BufferDesc.RefreshRate.Numerator = 60;  // ���t���b�V�� ���[�g(���q)
	sd.BufferDesc.RefreshRate.Denominator = 1; // ���t���b�V�� ���[�g(����)
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �o�b�N �o�b�t�@�̎g�p�@
	sd.OutputWindow = g_hWindow;	// �֘A�t����E�C���h�E
	sd.SampleDesc.Count = 1;		// �}���` �T���v���̐�
	sd.SampleDesc.Quality = 0;		// �}���` �T���v���̃N�I���e�B
	sd.Windowed = TRUE;				// �E�C���h�E ���[�h
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // ���[�h�����؂�ւ�

#if defined(DEBUG) || defined(_DEBUG)
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
    UINT createDeviceFlags = 0;
#endif

	// �n�[�h�E�F�A�E�f�o�C�X���쐬
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
            g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
            &g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
            &g_pImmediateContext);
    if(FAILED(hr)) {
        // WARP�f�o�C�X���쐬
        hr = D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags,
            g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
            &g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
            &g_pImmediateContext);
        if(FAILED(hr)) {
            // ���t�@�����X�E�f�o�C�X���쐬
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

	// �o�b�N �o�b�t�@�̏�����
	hr = InitBackBuffer();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D InitBackBuffer", hr);

	// IDXGIFactory�C���^�[�t�F�C�X�̎擾
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&g_pFactory));
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D CreateDXGIFactory", hr);

	// [Alt]+[Enter]�L�[�ɂ���ʃ��[�h�؂�ւ��@�\��ݒ肷��
	hr = g_pFactory->MakeWindowAssociation(
				g_hWindow, // ��ʃ��[�h�؂�ւ��@�\��ݒ肷��E�C���h�E �n���h��
				0);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pFactory->MakeWindowAssociation", hr);

	return S_OK;
}

/*-------------------------------------------
	�o�b�N �o�b�t�@�̏�����(�o�b�N �o�b�t�@��`��^�[�Q�b�g�ɐݒ�)
--------------------------------------------*/
HRESULT InitBackBuffer(void)
{
	HRESULT hr;

    // �X���b�v�E�`�F�C������ŏ��̃o�b�N�E�o�b�t�@���擾����
    ID3D11Texture2D *pBackBuffer;  // �o�b�t�@�̃A�N�Z�X�Ɏg���C���^�[�t�F�C�X
    hr = g_pSwapChain->GetBuffer(
            0,                         // �o�b�N�E�o�b�t�@�̔ԍ�
            __uuidof(ID3D11Texture2D), // �o�b�t�@�ɃA�N�Z�X����C���^�[�t�F�C�X
            (LPVOID*)&pBackBuffer);    // �o�b�t�@���󂯎��ϐ�
    if(FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pSwapChain->GetBuffer", hr);  // ���s

    // �o�b�N�E�o�b�t�@�̕`��^�[�Q�b�g�E�r���[�����
    hr = g_pD3DDevice->CreateRenderTargetView(
            pBackBuffer,           // �r���[�ŃA�N�Z�X���郊�\�[�X
            NULL,                  // �`��^�[�Q�b�g�E�r���[�̒�`
            &g_pRenderTargetView); // �`��^�[�Q�b�g�E�r���[���󂯎��ϐ�
    SAFE_RELEASE(pBackBuffer);  // �ȍ~�A�o�b�N�E�o�b�t�@�͒��ڎg��Ȃ��̂ŉ��
    if(FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateRenderTargetView", hr);  // ���s

    // �r���[�|�[�g�̐ݒ�
    g_ViewPort[0].TopLeftX = 0.0f;    // �r���[�|�[�g�̈�̍���X���W�B
    g_ViewPort[0].TopLeftY = 0.0f;    // �r���[�|�[�g�̈�̍���Y���W�B
    g_ViewPort[0].Width    = 640.0f;  // �r���[�|�[�g�̈�̕�
    g_ViewPort[0].Height   = 480.0f;  // �r���[�|�[�g�̈�̍���
    g_ViewPort[0].MinDepth = 0.0f; // �r���[�|�[�g�̈�̐[�x�l�̍ŏ��l
    g_ViewPort[0].MaxDepth = 1.0f; // �r���[�|�[�g�̈�̐[�x�l�̍ő�l

	return S_OK;
}

/*--------------------------------------------
	��ʂ̕`�揈��
--------------------------------------------*/
HRESULT Render(void)
{
	HRESULT hr;

    // �`��^�[�Q�b�g�̃N���A
    g_pImmediateContext->ClearRenderTargetView(
                       g_pRenderTargetView, // �N���A����`��^�[�Q�b�g
                       g_ClearColor);       // �N���A����l

    // ���X�^���C�U�Ƀr���[�|�[�g��ݒ�
    g_pImmediateContext->RSSetViewports(1, g_ViewPort);

    // �`��^�[�Q�b�g�E�r���[���o�̓}�[�W���[�̕`��^�[�Q�b�g�Ƃ��Đݒ�
    g_pImmediateContext->OMSetRenderTargets(
            1,                    // �`��^�[�Q�b�g�̐�
            &g_pRenderTargetView, // �`��^�[�Q�b�g�E�r���[�̔z��
            NULL);                // �[�x/�X�e���V���E�r���[��ݒ肵�Ȃ�

	// �`��(�ȗ�)

	// �o�b�N �o�b�t�@�̕\��
	hr = g_pSwapChain->Present(	0,	// ��ʂ𒼂��ɍX�V����
								0);	// ��ʂ����ۂɍX�V����

	return hr;
}

/*-------------------------------------------
	Direct3D�̏I������
--------------------------------------------*/
bool CleanupDirect3D(void)
{
    // �f�o�C�X�E�X�e�[�g�̃N���A
    if(g_pImmediateContext)
        g_pImmediateContext->ClearState();

	// �X���b�v �`�F�C�����E�C���h�E ���[�h�ɂ���
	if (g_pSwapChain)
		g_pSwapChain->SetFullscreenState(FALSE, NULL);

	// �擾�����C���^�[�t�F�C�X�̊J��
	SAFE_RELEASE(g_pRenderTargetView);
	SAFE_RELEASE(g_pSwapChain);
    SAFE_RELEASE(g_pImmediateContext);
	SAFE_RELEASE(g_pD3DDevice);
	SAFE_RELEASE(g_pFactory);

	return true;
}

/*-------------------------------------------
	�A�v���P�[�V�����̏I������
--------------------------------------------*/
bool CleanupApp(void)
{
	// �E�C���h�E �N���X�̓o�^����
	UnregisterClass(g_szWndClass, g_hInstance);
	return true;
}

/*-------------------------------------------
	�E�B���h�E����
--------------------------------------------*/
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HRESULT hr = S_OK;
	BOOL fullscreen;

	switch(msg)
	{
	case WM_DESTROY:
		// Direct3D�̏I������
		CleanupDirect3D();
		// �E�C���h�E�����
		PostQuitMessage(0);
		g_hWindow = NULL;
		return 0;

	// �E�C���h�E �T�C�Y�̕ύX����
	case WM_SIZE:
		if (!g_pD3DDevice || wParam == SIZE_MINIMIZED)
			break;
		// �`��^�[�Q�b�g����������
		g_pImmediateContext->OMSetRenderTargets(0, NULL, NULL);	// �`��^�[�Q�b�g�̉���
		SAFE_RELEASE(g_pRenderTargetView);					    // �`��^�[�Q�b�g �r���[�̉��

		// �o�b�t�@�̕ύX
		g_pSwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		// �o�b�N �o�b�t�@�̏�����
		InitBackBuffer();
		break;

	case WM_KEYDOWN:
		// �L�[���͂̏���
		switch(wParam)
		{
		case VK_ESCAPE:	// [ESC]�L�[�ŃE�C���h�E�����
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case VK_F5:		// [F5]�L�[�ŉ�ʃ��[�h��؂�ւ���
			if (g_pSwapChain != NULL) {
				g_pSwapChain->GetFullscreenState(&fullscreen, NULL);
				g_pSwapChain->SetFullscreenState(!fullscreen, NULL);
			}
			break;

		case VK_F6:		// [F6]�L�[�Ńf�B�X�v���C ���[�h��ݒ肷��
			if (g_pSwapChain != NULL) {
				// �f�B�X�v���C ���[�h�̕ύX
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
					DXTRACE_ERR(L"MainWndProc g_pSwapChain->ResizeTarget", hr);  // ���s
			}
			break;
		}
		break;
	}

	// �f�t�H���g����
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*--------------------------------------------
	�f�o�C�X�̏�������
--------------------------------------------*/
HRESULT IsDeviceRemoved(void)
{
	HRESULT hr;

	// �f�o�C�X�̏����m�F
	hr = g_pD3DDevice->GetDeviceRemovedReason();
	switch (hr) {
	case S_OK:
		break;         // ����

	case DXGI_ERROR_DEVICE_HUNG:
	case DXGI_ERROR_DEVICE_RESET:
		DXTRACE_ERR(L"IsDeviceRemoved g_pD3DDevice->GetDeviceRemovedReason", hr);
		CleanupDirect3D();   // Direct3D�̉��(�A�v���P�[�V������`)
		hr = InitDirect3D();  // Direct3D�̏�����(�A�v���P�[�V������`)
		if (FAILED(hr))
			return hr; // ���s�B�A�v���P�[�V�������I��
		break;

	case DXGI_ERROR_DEVICE_REMOVED:
	case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
	case DXGI_ERROR_INVALID_CALL:
	default:
		DXTRACE_ERR(L"IsDeviceRemoved g_pD3DDevice->GetDeviceRemovedReason", hr);
		return hr;   // �ǂ����悤���Ȃ��̂ŁA�A�v���P�[�V�������I���B
	};

	return S_OK;         // ����
}

/*--------------------------------------------
	�A�C�h�����̏���
--------------------------------------------*/
bool AppIdle(void)
{
	if (!g_pD3DDevice)
		return false;

	HRESULT hr;
	// �f�o�C�X�̏�������
	hr = IsDeviceRemoved();
	if (FAILED(hr))
		return false;

	// �X�^���o�C ���[�h
	if (g_bStandbyMode) {
		hr = g_pSwapChain->Present(0, DXGI_PRESENT_TEST);
		if (hr != S_OK) {
			Sleep(100);	// 0.1�b�҂�
			return true;
		}
		g_bStandbyMode = false; // �X�^���o�C ���[�h����������
		DXTRACE_MSG(L"�X�^���o�C ���[�h�������B");
	}

	// ��ʂ̍X�V
	hr = Render();
	if (hr == DXGI_STATUS_OCCLUDED) {
		g_bStandbyMode = true;  // �X�^���o�C ���[�h�ɓ���
		DXTRACE_MSG(L"�X�^���o�C ���[�h��ݒ�B");

		// �`��^�[�Q�b�g���N���A����F�̕ύX
		float c = g_ClearColor[0];
		g_ClearColor[0] = g_ClearColor[1];
		g_ClearColor[1] = g_ClearColor[2];
		g_ClearColor[2] = g_ClearColor[3];
		g_ClearColor[3] = c;
	}

	return true;
}

/*--------------------------------------------
	���C��
---------------------------------------------*/
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
	// �f�o�b�O �q�[�v �}�l�[�W���ɂ�郁�������蓖�Ă̒ǐՕ��@��ݒ�
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// �A�v���P�[�V�����Ɋւ��鏉����
	HRESULT hr = InitApp(hInst);
	if (FAILED(hr))
	{
		DXTRACE_ERR(L"WinMain InitApp", hr);
		return 0;
	}

	// Direct3D�̏�����
	hr = InitDirect3D();
	if (FAILED(hr)) {
		DXTRACE_ERR(L"WinMain InitDirect3D", hr);
		CleanupDirect3D();
		CleanupApp();
		return 0;
	}

	// ���b�Z�[�W ���[�v
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
			// �A�C�h������
			if (!AppIdle())
				// �G���[������ꍇ�C�A�v���P�[�V�������I������
				DestroyWindow(g_hWindow);
		}
	} while (msg.message != WM_QUIT);

	// �A�v���P�[�V�����̏I������
	CleanupApp();

	return (int)msg.wParam;
}
