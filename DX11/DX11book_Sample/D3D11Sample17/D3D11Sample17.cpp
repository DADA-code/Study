/*----------------------------------------------------------
	Direct3D 11�T���v��
		�EMicrosoft DirectX SDK (February 2010)
		�EVisual Studio 2010 Express
		�EWindows 7/Vista
		�E�V�F�[�_���f��4.0
		�Ή�

	D3D11Sample17.cpp
		�u�V���h�E�E�{�����[���v
--------------------------------------------------------------*/

#define STRICT					// �^�`�F�b�N�������ɍs�Ȃ�
#define WIN32_LEAN_AND_MEAN		// �w�b�_�[���炠�܂�g���Ȃ��֐����Ȃ�
#define WINVER        0x0600	// Windows Vista�ȍ~�Ή��A�v�����w��(�Ȃ��Ă��悢)
#define _WIN32_WINNT  0x0600	// ����

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// ����}�N��

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <crtdbg.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <dxerr.h>

#include "..\\WavefrontOBJ\\wavefrontobj.h"
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

#pragma comment( lib, "winmm.lib" )

/*-------------------------------------------
	�O���[�o���ϐ�(�A�v���P�[�V�����֘A)
--------------------------------------------*/
HINSTANCE	g_hInstance		= NULL;	// �C���X�^���X �n���h��
HWND		g_hWindow		= NULL;	// �E�C���h�E �n���h��

WCHAR		g_szAppTitle[]	= L"Direct3D 11 Sample17";
WCHAR		g_szWndClass[]	= L"D3D11S17";

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
ID3D11Device*           g_pD3DDevice = NULL;        // �f�o�C�X
ID3D11DeviceContext*    g_pImmediateContext = NULL; // �f�o�C�X�E�R���e�L�X�g
IDXGISwapChain*         g_pSwapChain = NULL;        // �X���b�v�E�`�F�C��

ID3D11RenderTargetView* g_pRenderTargetView = NULL; // �`��^�[�Q�b�g�E�r���[
D3D11_VIEWPORT          g_ViewPort[1];				// �r���[�|�[�g

ID3D11Texture2D*          g_pDepthStencil = NULL;		// �[�x/�X�e���V��
ID3D11DepthStencilView*   g_pDepthStencilView = NULL;	// �[�x/�X�e���V���E�r���[

ID3D11InputLayout*        g_pInputLayout = NULL;            // ���̓��C�A�E�g�E�I�u�W�F�N�g

// 3D�I�u�W�F�N�g�`��p
ID3D11VertexShader*       g_pVertexShader = NULL;		// ���_�V�F�[�_
ID3D11PixelShader*        g_pPixelShader = NULL;		// �s�N�Z���E�V�F�[�_
ID3D11BlendState*         g_pBlendState = NULL;			// �u�����h�E�X�e�[�g�E�I�u�W�F�N�g
ID3D11RasterizerState*	  g_pRasterizerState = NULL;	// ���X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g
ID3D11DepthStencilState*  g_pDepthStencilState = NULL;	// �[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g

// �V���h�E �{�����[���`��p
ID3D11VertexShader*       g_pVertexShaderSV = NULL;		// ���_�V�F�[�_
ID3D11GeometryShader*     g_pGeometryShaderSV1 = NULL;	// �W�I���g���E�V�F�[�_
ID3D11GeometryShader*     g_pGeometryShaderSV2 = NULL;	// �W�I���g���E�V�F�[�_
ID3D11BlendState*         g_pBlendStateSV = NULL;		// �u�����h�E�X�e�[�g�E�I�u�W�F�N�g
ID3D11RasterizerState*	  g_pRasterizerStateSV = NULL;	// ���X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g
ID3D11DepthStencilState*  g_pDepthStencilStateSV = NULL;	// �[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g

// �V���h�E�`��p
ID3D11VertexShader*       g_pVertexShaderShadow = NULL;		// ���_�V�F�[�_
ID3D11PixelShader*        g_pPixelShaderShadow = NULL;		// �s�N�Z���E�V�F�[�_
ID3D11BlendState*         g_pBlendStateShadow = NULL;		// �u�����h�E�X�e�[�g�E�I�u�W�F�N�g
ID3D11DepthStencilState*  g_pDepthStencilStateShadow = NULL;	// �[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g

ID3D11Buffer*             g_pCBuffer = NULL;		// �萔�o�b�t�@

// �V�F�[�_�̃R���p�C�� �I�v�V����
#if defined(DEBUG) || defined(_DEBUG)
UINT g_flagCompile = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION
					| D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
UINT g_flagCompile = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#endif

// �[�x�o�b�t�@�̃��[�h
bool g_bDepthMode = true;

// �V���h�E�E�{�����[���̐������[�h
bool g_bShadowVolumeMode1 = true;

// �X�^���o�C���[�h
bool g_bStandbyMode = false;

// �`��^�[�Q�b�g���N���A����l(RGBA)
float g_ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };

float g_fEye = 2.5f;                       // ���_�܂ł̋���
XMFLOAT3 g_vLightPos(3.0f, 3.0f, -3.0f);   // �������W(���[���h���W�n)

// �萔�o�b�t�@�̃f�[�^��`
struct cbCBuffer {
	XMFLOAT4X4 World;		// ���[���h�ϊ��s��
	XMFLOAT4X4 View;		// �r���[�ϊ��s��
	XMFLOAT4X4 Projection;	// �����ϊ��s��
	XMFLOAT4   Diffuse;		// �f�B�t���[�Y�F
	XMFLOAT3   Light;		// �������W(���[���h���W�n)
	FLOAT      dummy;		// �_�~�[
};

// �萔�o�b�t�@�̃f�[�^
struct cbCBuffer g_cbCBuffer;

// ���b�V�����
CWavefrontObj       g_wfObjKuma;
CWavefrontMtl       g_wfMtl;     // �}�e���A��
ID3D11SamplerState* g_pTextureSampler = NULL; // �T���v���[

/*-------------------------------------------
	�֐���`
--------------------------------------------*/

LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);
HRESULT CreateShaderObj(void);
HRESULT CreateShaderShadowVolume(void);
HRESULT CreateShaderShadow(void);
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
	sd.BufferCount       = 3;		// �o�b�N �o�b�t�@��
	sd.BufferDesc.Width  = width;	// �o�b�N �o�b�t�@�̕�
	sd.BufferDesc.Height = height;	// �o�b�N �o�b�t�@�̍���
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // �t�H�[�}�b�g
	sd.BufferDesc.RefreshRate.Numerator = 60;  // ���t���b�V�� ���[�g(���q)
	sd.BufferDesc.RefreshRate.Denominator = 1; // ���t���b�V�� ���[�g(����)
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;	// �X�L�������C��
	sd.BufferDesc.Scaling          = DXGI_MODE_SCALING_CENTERED;			// �X�P�[�����O
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

	// **********************************************************
	// �V�F�[�_�̃R���p�C��
	hr = CreateShaderObj();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D CreateShaderObj", hr);
	hr = CreateShaderShadowVolume();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D CreateShaderShadowVolume", hr);
	hr = CreateShaderShadow();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D CreateShaderShadow", hr);


	// **********************************************************
	// �萔�o�b�t�@�̒�`
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;    // ���I(�_�C�i�~�b�N)�g�p�@
	cBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER; // �萔�o�b�t�@
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPU���珑������
	cBufferDesc.MiscFlags      = 0;
	cBufferDesc.StructureByteStride = 0;

	// �萔�o�b�t�@�̍쐬
	cBufferDesc.ByteWidth      = sizeof(cbCBuffer); // �o�b�t�@�E�T�C�Y
	hr = g_pD3DDevice->CreateBuffer(&cBufferDesc, NULL, &g_pCBuffer);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateBuffer", hr);

	// **********************************************************
	// Waveform OBJ�t�@�C���̓ǂݍ���
	char mtlFileName[80];
	if (g_wfObjKuma.Load(g_pD3DDevice, g_pImmediateContext, "..\\misc\\kuma.obj",mtlFileName, sizeof(mtlFileName)) == false)
		return DXTRACE_ERR(L"InitDirect3D g_wfObjKuma.Load", E_FAIL);
	// MTL�t�@�C���̓ǂݍ���
	if (g_wfMtl.Load(g_pD3DDevice, mtlFileName, "..\\misc\\default.bmp") == false)
		return DXTRACE_ERR(L"InitDirect3D g_wfMtl.Load", E_FAIL);

	// **********************************************************
	// �T���v���[�̍쐬
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
	hr = g_pD3DDevice->CreateSamplerState(&descSampler, &g_pTextureSampler);
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D g_pD3DDevice->CreateSamplerState", hr);

	// **********************************************************
	// �o�b�N �o�b�t�@�̏�����
	hr = InitBackBuffer();
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitDirect3D InitBackBuffer", hr);

	return hr;
}

/*-------------------------------------------
	�V�F�[�_�̃R���p�C��(3D�I�u�W�F�N�g�`��p)
--------------------------------------------*/
HRESULT CreateShaderObj(void)
{
	HRESULT hr;

	// **********************************************************
	// ���_�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobVS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample17.sh",  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"VS",          // �uVS�֐��v���V�F�[�_������s�����
			"vs_4_0",      // ���_�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobVS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj D3DX11CompileShaderFromFile", hr);

	// ���_�V�F�[�_�̍쐬
	hr = g_pD3DDevice->CreateVertexShader(
			pBlobVS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobVS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
			NULL,
			&g_pVertexShader); // ���_�V�F�[�_���󂯎��ϐ�
//	SAFE_RELEASE(pBlobVS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr)) {
		SAFE_RELEASE(pBlobVS);
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreateVertexShader", hr);
	}

	// **********************************************************
	// ���͗v�f
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                    D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3),     D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,    0, sizeof(XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// ���̓��C�A�E�g�E�I�u�W�F�N�g�̍쐬
	hr = g_pD3DDevice->CreateInputLayout(
			layout,                            // ��`�̔z��
			_countof(layout),                  // ��`�̗v�f��
			pBlobVS->GetBufferPointer(),       // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobVS->GetBufferSize(),          // �o�C�g�E�R�[�h�̃T�C�Y
			&g_pInputLayout);                  // �󂯎��ϐ��̃|�C���^
	SAFE_RELEASE(pBlobVS);
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreateInputLayout", hr);

	// **********************************************************
	// �s�N�Z���E�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobPS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample17.sh",  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"PS",          // �uPS�֐��v���V�F�[�_������s�����
			"ps_4_0",      // �s�N�Z���E�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobPS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj D3DX11CompileShaderFromFile", hr);

	// �s�N�Z���E�V�F�[�_�̍쐬
	hr = g_pD3DDevice->CreatePixelShader(
			pBlobPS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobPS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
			NULL,
			&g_pPixelShader); // �s�N�Z���E�V�F�[�_���󂯎��ϐ�
	SAFE_RELEASE(pBlobPS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreatePixelShader", hr);

	// **********************************************************
	// �u�����h�E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable  = FALSE;
	BlendState.IndependentBlendEnable = FALSE;
	BlendState.RenderTarget[0].BlendEnable           = FALSE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pD3DDevice->CreateBlendState(&BlendState, &g_pBlendState);
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreateBlendState", hr);

	// **********************************************************
	// ���X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;   // ���ʂɕ`�悷��
	RSDesc.CullMode = D3D11_CULL_BACK;    // �\�ʂ�`�悷��
	RSDesc.FrontCounterClockwise = FALSE; // ���v��肪�\��
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

	// **********************************************************
	// �[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable      = TRUE; // �[�x�e�X�g����
	DepthStencil.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ALL; // ��������
	DepthStencil.DepthFunc        = D3D11_COMPARISON_LESS; // ��O�̕��̂�`��
	DepthStencil.StencilEnable    = FALSE; // �X�e���V���E�e�X�g�Ȃ�
	DepthStencil.StencilReadMask  = 0xff;     // �X�e���V���ǂݍ��݃}�X�N�B
	DepthStencil.StencilWriteMask = 0xff;     // �X�e���V���������݃}�X�N�B
			// �ʂ��\�������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS; // ��ɐ���
			// �ʂ����������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS; // ��ɐ���
	hr = g_pD3DDevice->CreateDepthStencilState(&DepthStencil,
											   &g_pDepthStencilState);
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderObj g_pD3DDevice->CreateDepthStencilState", hr);

	return hr;
}

/*-------------------------------------------
	�V�F�[�_�̃R���p�C��(�V���h�E �{�����[���`��p)
--------------------------------------------*/
HRESULT CreateShaderShadowVolume(void)
{
	HRESULT hr;

	// **********************************************************
	// ���_�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobVS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample17.sh",  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"VS_SV",       // �uVS_SV�֐��v���V�F�[�_������s�����
			"vs_4_0",      // ���_�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobVS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadowVolume1 D3DX11CompileShaderFromFile", hr);

	// ���_�V�F�[�_�̍쐬
	hr = g_pD3DDevice->CreateVertexShader(
			pBlobVS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobVS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
			NULL,
			&g_pVertexShaderSV); // ���_�V�F�[�_���󂯎��ϐ�
	SAFE_RELEASE(pBlobVS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadowVolume1 g_pD3DDevice->CreateVertexShader", hr);

	// **********************************************************
	// �W�I���g���E�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobGS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample17.sh",  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"GS_SV_1",     // �uGS_SV_1�֐��v���V�F�[�_������s�����
			"gs_4_0",      // �W�I���g���E�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobGS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadowVolume D3DX11CompileShaderFromFile", hr);

	// �W�I���g���E�V�F�[�_�̍쐬
	hr = g_pD3DDevice->CreateGeometryShader(
			pBlobGS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobGS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
			NULL,
			&g_pGeometryShaderSV1); // �W�I���g���E�V�F�[�_���󂯎��ϐ�
	SAFE_RELEASE(pBlobGS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadowVolume g_pD3DDevice->CreatePixelShader", hr);

	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample17.sh",  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"GS_SV_2",     // �uGS_SV_2�֐��v���V�F�[�_������s�����
			"gs_4_0",      // �W�I���g���E�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobGS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadowVolume D3DX11CompileShaderFromFile", hr);

	// �W�I���g���E�V�F�[�_�̍쐬
	hr = g_pD3DDevice->CreateGeometryShader(
			pBlobGS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobGS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
			NULL,
			&g_pGeometryShaderSV2); // �W�I���g���E�V�F�[�_���󂯎��ϐ�
	SAFE_RELEASE(pBlobGS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadowVolume g_pD3DDevice->CreatePixelShader", hr);

	// **********************************************************
	// �u�����h�E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable  = FALSE;
	BlendState.IndependentBlendEnable = FALSE;
	BlendState.RenderTarget[0].BlendEnable           = TRUE;
	BlendState.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pD3DDevice->CreateBlendState(&BlendState, &g_pBlendStateSV);
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadowVolume g_pD3DDevice->CreateBlendState", hr);

	// **********************************************************
	// ���X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;   // ���ʂɕ`�悷��
	RSDesc.CullMode = D3D11_CULL_NONE;    // ���ʂ�`�悷��
	RSDesc.FrontCounterClockwise = FALSE; // ���v��肪�\��
	RSDesc.DepthBias             = 0;
	RSDesc.DepthBiasClamp        = 0;
	RSDesc.SlopeScaledDepthBias  = 0;
	RSDesc.DepthClipEnable       = TRUE;
	RSDesc.ScissorEnable         = FALSE;
	RSDesc.MultisampleEnable     = FALSE;
	RSDesc.AntialiasedLineEnable = FALSE;
	hr = g_pD3DDevice->CreateRasterizerState(&RSDesc, &g_pRasterizerStateSV);
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadowVolume g_pD3DDevice->CreateRasterizerState", hr);

	// **********************************************************
	// �[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable      = TRUE; // �[�x�e�X�g����
	DepthStencil.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ZERO; // �������܂Ȃ�
	DepthStencil.DepthFunc        = D3D11_COMPARISON_LESS; // ��O�̕��̂�`��
	DepthStencil.StencilEnable    = TRUE; // �X�e���V���E�e�X�g����
	DepthStencil.StencilReadMask  = 0xff;     // �X�e���V���ǂݍ��݃}�X�N�B
	DepthStencil.StencilWriteMask = 0xff;     // �X�e���V���������݃}�X�N�B
			// �ʂ��\�������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_INCR;  // �{�P
	DepthStencil.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS; // ��ɐ���
			// �ʂ����������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilPassOp      = D3D11_STENCIL_OP_DECR;   // �|�P
	DepthStencil.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS; // ��ɐ���
	hr = g_pD3DDevice->CreateDepthStencilState(&DepthStencil,
											   &g_pDepthStencilStateSV);
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadowVolume g_pD3DDevice->CreateDepthStencilState", hr);

	return hr;
}

/*-------------------------------------------
	�V�F�[�_�̃R���p�C��(�V���h�E�`��p)
--------------------------------------------*/
HRESULT CreateShaderShadow(void)
{
	HRESULT hr;

	// **********************************************************
	// ���_�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobVS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample17.sh",  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"VS_S",        // �uVS_S�֐��v���V�F�[�_������s�����
			"vs_4_0",      // ���_�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobVS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadow D3DX11CompileShaderFromFile", hr);

	// ���_�V�F�[�_�̍쐬
	hr = g_pD3DDevice->CreateVertexShader(
			pBlobVS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobVS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
			NULL,
			&g_pVertexShaderShadow); // ���_�V�F�[�_���󂯎��ϐ�
	SAFE_RELEASE(pBlobVS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadow g_pD3DDevice->CreateVertexShader", hr);

	// **********************************************************
	// �s�N�Z���E�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobPS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample17.sh",  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"PS_S",        // �uPS_S�֐��v���V�F�[�_������s�����
			"ps_4_0",      // �s�N�Z���E�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobPS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadow D3DX11CompileShaderFromFile", hr);

	// �s�N�Z���E�V�F�[�_�̍쐬
	hr = g_pD3DDevice->CreatePixelShader(
			pBlobPS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobPS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
			NULL,
			&g_pPixelShaderShadow); // �s�N�Z���E�V�F�[�_���󂯎��ϐ�
	SAFE_RELEASE(pBlobPS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadow g_pD3DDevice->CreatePixelShader", hr);

	// **********************************************************
	// �u�����h�E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable  = FALSE;
	BlendState.IndependentBlendEnable = FALSE;
	BlendState.RenderTarget[0].BlendEnable           = TRUE;
	BlendState.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pD3DDevice->CreateBlendState(&BlendState, &g_pBlendStateShadow);
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadow g_pD3DDevice->CreateBlendState", hr);

	// **********************************************************
	// �[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g�̍쐬
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable      = TRUE; // �[�x�e�X�g����
	DepthStencil.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ZERO; // �������܂Ȃ�
	DepthStencil.DepthFunc        = D3D11_COMPARISON_GREATER; // ���̕��̂�`��
	DepthStencil.StencilEnable    = TRUE;     // �X�e���V���E�e�X�g����
	DepthStencil.StencilReadMask  = 0xff;     // �X�e���V���ǂݍ��݃}�X�N�B
	DepthStencil.StencilWriteMask = 0;        // �X�e���V���������݃}�X�N�B
			// �ʂ��\�������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;  // �ێ�
	DepthStencil.FrontFace.StencilFunc        = D3D11_COMPARISON_NOT_EQUAL; // �����łȂ��ꍇ�A����
			// �ʂ����������Ă���ꍇ�̃X�e���V���E�e�X�g�̐ݒ�
	DepthStencil.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;   // �ێ�
	DepthStencil.BackFace.StencilFunc        = D3D11_COMPARISON_NEVER;  // ��Ɏ��s
	hr = g_pD3DDevice->CreateDepthStencilState(&DepthStencil,
											   &g_pDepthStencilStateShadow);
	if (FAILED(hr))
		return DXTRACE_ERR(L"CreateShaderShadow g_pD3DDevice->CreateDepthStencilState", hr);

	return hr;
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

	// �o�b�N�E�o�b�t�@�̏��
	D3D11_TEXTURE2D_DESC descBackBuffer;
	pBackBuffer->GetDesc(&descBackBuffer);

    // �o�b�N�E�o�b�t�@�̕`��^�[�Q�b�g�E�r���[�����
    hr = g_pD3DDevice->CreateRenderTargetView(
            pBackBuffer,           // �r���[�ŃA�N�Z�X���郊�\�[�X
            NULL,                  // �`��^�[�Q�b�g�E�r���[�̒�`
            &g_pRenderTargetView); // �`��^�[�Q�b�g�E�r���[���󂯎��ϐ�
    SAFE_RELEASE(pBackBuffer);  // �ȍ~�A�o�b�N�E�o�b�t�@�͒��ڎg��Ȃ��̂ŉ��
    if(FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateRenderTargetView", hr);  // ���s

	// �[�x/�X�e���V���E�e�N�X�`���̍쐬
	D3D11_TEXTURE2D_DESC descDepth = descBackBuffer;
//	descDepth.Width     = descBackBuffer.Width;   // ��
//	descDepth.Height    = descBackBuffer.Height;  // ����
	descDepth.MipLevels = 1;       // �~�b�v�}�b�v�E���x����
	descDepth.ArraySize = 1;       // �z��T�C�Y
	descDepth.Format    = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;  // �t�H�[�}�b�g(�[�x�{�X�e���V��)
//	descDepth.SampleDesc.Count   = 1;  // �}���`�T���v�����O�̐ݒ�
//	descDepth.SampleDesc.Quality = 0;  // �}���`�T���v�����O�̕i��
	descDepth.Usage          = D3D11_USAGE_DEFAULT;      // �f�t�H���g�g�p�@
	descDepth.BindFlags      = D3D11_BIND_DEPTH_STENCIL; // �[�x/�X�e���V���Ƃ��Ďg�p
	descDepth.CPUAccessFlags = 0;   // CPU����̓A�N�Z�X���Ȃ�
	descDepth.MiscFlags      = 0;   // ���̑��̐ݒ�Ȃ�
	hr = g_pD3DDevice->CreateTexture2D(
			&descDepth,         // �쐬����2D�e�N�X�`���̐ݒ�
			NULL,               // 
			&g_pDepthStencil);  // �쐬�����e�N�X�`�����󂯎��ϐ�
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateTexture2D", hr);  // ���s

	// �[�x/�X�e���V�� �r���[�̍쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format             = descDepth.Format;            // �r���[�̃t�H�[�}�b�g
	descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags              = 0;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pD3DDevice->CreateDepthStencilView(
			g_pDepthStencil,       // �[�x/�X�e���V���E�r���[�����e�N�X�`��
			&descDSV,              // �[�x/�X�e���V���E�r���[�̐ݒ�
			&g_pDepthStencilView); // �쐬�����r���[���󂯎��ϐ�
	if (FAILED(hr))
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateDepthStencilView", hr);  // ���s

    // �r���[�|�[�g�̐ݒ�
    g_ViewPort[0].TopLeftX = 0.0f;    // �r���[�|�[�g�̈�̍���X���W�B
    g_ViewPort[0].TopLeftY = 0.0f;    // �r���[�|�[�g�̈�̍���Y���W�B
    g_ViewPort[0].Width    = (FLOAT)descBackBuffer.Width;   // �r���[�|�[�g�̈�̕�
    g_ViewPort[0].Height   = (FLOAT)descBackBuffer.Height;  // �r���[�|�[�g�̈�̍���
    g_ViewPort[0].MinDepth = 0.0f; // �r���[�|�[�g�̈�̐[�x�l�̍ŏ��l
    g_ViewPort[0].MaxDepth = 1.0f; // �r���[�|�[�g�̈�̐[�x�l�̍ő�l

	// �萔�o�b�t�@���X�V
	// �ˉe�ϊ��s��(�p�[�X�y�N�e�B�u(�����@)�ˉe)
	XMMATRIX mat = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(30.0f),		// ����p30��
			(float)descBackBuffer.Width / (float)descBackBuffer.Height,	// �A�X�y�N�g��
			1.0f,							// �O�����e�ʂ܂ł̋���
			400.0f);						// ������e�ʂ܂ł̋���
	mat = XMMatrixTranspose(mat);
	XMStoreFloat4x4(&g_cbCBuffer.Projection, mat);

	//�T�C�Y��ۑ�
	g_sizeWindow.cx = descBackBuffer.Width;
	g_sizeWindow.cy = descBackBuffer.Height;

	return S_OK;
}

/*--------------------------------------------
	Obj�̕`�揈��
--------------------------------------------*/
void DrawObj(bool adj = false)
{
	WFOBJ_GROUP* pGroup = g_wfObjKuma.GetGroup();
	int countGroup = g_wfObjKuma.GetCountGroup();
	for (int g = 0; g < countGroup; ++g) {
		if (pGroup[g].countIndex <= 0)
			continue;

		// �}�e���A���ݒ�
		WFOBJ_MTL* pMtl = g_wfMtl.GetMaterial(pGroup[g].mtl);
		ID3D11ShaderResourceView* srv[1];
		srv[0] = g_wfMtl.GetTextureDefault(pMtl->map_Kd);

		g_cbCBuffer.Diffuse.x = pMtl->Kd[0];
		g_cbCBuffer.Diffuse.y = pMtl->Kd[1];
		g_cbCBuffer.Diffuse.z = pMtl->Kd[2];
		g_cbCBuffer.Diffuse.w = pMtl->d;

		// �萔�o�b�t�@�̃}�b�v�擾
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		HRESULT hr = g_pImmediateContext->Map(
                  g_pCBuffer,              // �}�b�v���郊�\�[�X
                  0,                       // �T�u���\�[�X�̃C���f�b�N�X�ԍ�
                  D3D11_MAP_WRITE_DISCARD, // �������݃A�N�Z�X
                  0,                       //
                  &MappedResource);        // �f�[�^�̏������ݐ�|�C���^
		if (FAILED(hr)) {
			DXTRACE_ERR(L"DrawObj  g_pImmediateContext->Map", hr);  // ���s
			return;
		}
		// �f�[�^��������
		CopyMemory(MappedResource.pData, &g_cbCBuffer, sizeof(cbCBuffer));
		// �}�b�v����
		g_pImmediateContext->Unmap(g_pCBuffer, 0);

		// PS�ɃV�F�[�_�E���\�[�X�E�r���[��ݒ�
		g_pImmediateContext->PSSetShaderResources(
	        0,              // �ݒ肷��ŏ��̃X���b�g�ԍ�
	        1,              // �ݒ肷��V�F�[�_�E���\�[�X�E�r���[�̐�
	        srv);			// �ݒ肷��V�F�[�_�E���\�[�X�E�r���[�̔z��

		// ���b�V����`��
		g_wfObjKuma.Draw(g, adj);
	}
}

/*--------------------------------------------
	3D�I�u�W�F�N�g�̕`�揈��
--------------------------------------------*/
void RenderObj(void) {
	// **********************************************************
	// IA��OBJ�t�@�C���̒��_�o�b�t�@/�C���f�b�N�X�E�o�b�t�@��ݒ�
	g_wfObjKuma.SetIA();
	// IA�ɓ��̓��C�A�E�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	// VS�ɒ��_�V�F�[�_��ݒ�
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);

	// GS�ɃW�I���g���E�V�F�[�_��ݒ�
	g_pImmediateContext->GSSetShader(NULL, NULL, 0);

	// RS�Ƀ��X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->RSSetState(g_pRasterizerState);

	// PS�Ƀs�N�Z���E�V�F�[�_��ݒ�
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);

	// OM�Ƀu�����h�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	FLOAT BlendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	g_pImmediateContext->OMSetBlendState(g_pBlendState, BlendFactor, 0xffffffff);
	// OM�ɐ[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState, 0);

	// ***************************************
	// 3D�I�u�W�F�N�g��`��
	DrawObj();
}

/*--------------------------------------------
	�V���h�E�E�{�����[���̕`�揈��
--------------------------------------------*/
void RenderSV(void) {
	// **********************************************************
	// IA��OBJ�t�@�C���̒��_�o�b�t�@/�C���f�b�N�X�E�o�b�t�@��ݒ�
	g_wfObjKuma.SetIA(g_bShadowVolumeMode1);
	// IA�ɓ��̓��C�A�E�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	// VS�ɒ��_�V�F�[�_��ݒ�
	g_pImmediateContext->VSSetShader(g_pVertexShaderSV, NULL, 0);

	// GS�ɃW�I���g���E�V�F�[�_��ݒ�
	g_pImmediateContext->GSSetShader(g_bShadowVolumeMode1 ? g_pGeometryShaderSV1 : g_pGeometryShaderSV2, NULL, 0);

	// RS�Ƀ��X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->RSSetState(g_pRasterizerStateSV);

	// PS�Ƀs�N�Z���E�V�F�[�_��ݒ�
	g_pImmediateContext->PSSetShader(NULL, NULL, 0);

	// OM�Ƀu�����h�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	FLOAT BlendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	g_pImmediateContext->OMSetBlendState(g_pBlendStateSV, BlendFactor, 0xffffffff);
	// OM�ɐ[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilStateSV, 0);

	// ***************************************
	// 3D�I�u�W�F�N�g��`��
	DrawObj(g_bShadowVolumeMode1);
}

/*--------------------------------------------
	�V���h�E�̕`�揈��
--------------------------------------------*/
void RenderS(void) {
	// **********************************************************
	// IA�ɓ��̓��C�A�E�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->IASetInputLayout(NULL);
	// IA�Ƀv���~�e�B�u�̎�ނ�ݒ�
	g_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// VS�ɒ��_�V�F�[�_��ݒ�
	g_pImmediateContext->VSSetShader(g_pVertexShaderShadow, NULL, 0);

	// GS�ɃW�I���g���E�V�F�[�_��ݒ�
	g_pImmediateContext->GSSetShader(NULL, NULL, 0);

	// RS�Ƀ��X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->RSSetState(g_pRasterizerState);

	// PS�Ƀs�N�Z���E�V�F�[�_��ݒ�
	g_pImmediateContext->PSSetShader(g_pPixelShaderShadow, NULL, 0);

	// OM�Ƀu�����h�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	FLOAT BlendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	g_pImmediateContext->OMSetBlendState(g_pBlendStateShadow, BlendFactor, 0xffffffff);
	// OM�ɐ[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilStateShadow, 0);

	// ***************************************
	// 3D�I�u�W�F�N�g��`��
	g_pImmediateContext->Draw(4, 0);
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
                       g_ClearColor);         // �N���A����l

	// �[�x/�X�e���V���̃N���A
	g_pImmediateContext->ClearDepthStencilView(
			g_pDepthStencilView, // �N���A����[�x/�X�e���V���E�r���[
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,   // �[�x�l�ƃX�e���V���l���N���A����
			1.0f,                // �[�x�o�b�t�@���N���A����l
			0);                  // �X�e���V���E�o�b�t�@���N���A����l

	// ***************************************
	// �萔�o�b�t�@���X�V
	// �r���[�ϊ��s��
	XMVECTORF32 eyePosition   = { 0.0f, g_fEye, -g_fEye, 1.0f };  // ���_(�J�����̈ʒu)
	XMVECTORF32 focusPosition = { 0.0f, 0.0f,  0.0f, 1.0f };  // �����_
	XMVECTORF32 upDirection   = { 0.0f, 1.0f,  0.0f, 1.0f };  // �J�����̏����
	XMMATRIX mat = XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	XMStoreFloat4x4(&g_cbCBuffer.View, XMMatrixTranspose(mat));
	// �_�������W
	XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&g_vLightPos), mat);
	XMStoreFloat3(&g_cbCBuffer.Light, vec);
	// ���[���h�ϊ��s��
	XMFLOAT3 center = g_wfObjKuma.GetBoundingSphereCenter();
	XMMATRIX matTrans = XMMatrixTranslation(-center.x, -center.y, -center.z);

	float scale = 1.0f / g_wfObjKuma.GetBoundingSphereRadius();
	XMMATRIX matScale = XMMatrixScaling(scale, scale, scale);

	FLOAT rotate = (FLOAT)(XM_PI * (timeGetTime() % 3000)) / 1500.0f;
	XMMATRIX matY = XMMatrixRotationY(rotate);

	XMStoreFloat4x4(&g_cbCBuffer.World, XMMatrixTranspose(matTrans * matScale * matY));

	// **********************************************************
	// RS�Ƀr���[�|�[�g��ݒ�
	g_pImmediateContext->RSSetViewports(1, g_ViewPort);

	// OM�ɕ`��^�[�Q�b�g �r���[�Ɛ[�x/�X�e���V���E�r���[��ݒ�
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// VS�ɒ萔�o�b�t�@��ݒ�
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);

	// GS�ɒ萔�o�b�t�@��ݒ�
	g_pImmediateContext->GSSetConstantBuffers(0, 1, &g_pCBuffer);

	// PS�ɒ萔�o�b�t�@��ݒ�
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
	// PS�ɃT���v���[��ݒ�
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pTextureSampler);

	// 3D�I�u�W�F�N�g�̕`��
	RenderObj();
	// �V���h�E�E�{�����[���̕`��
	RenderSV();
	// �V���h�E�̕`��
	RenderS();

	// ***************************************
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

	// ���b�V���̉��
	g_wfMtl.Cleanup();
	g_wfObjKuma.Cleanup();

	// �擾�����C���^�[�t�F�C�X�̊J��
	SAFE_RELEASE(g_pTextureSampler);

	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pBlendState);
	SAFE_RELEASE(g_pRasterizerState);
	SAFE_RELEASE(g_pDepthStencilState);

	SAFE_RELEASE(g_pVertexShaderSV);
	SAFE_RELEASE(g_pGeometryShaderSV1);
	SAFE_RELEASE(g_pGeometryShaderSV2);
	SAFE_RELEASE(g_pBlendStateSV);
	SAFE_RELEASE(g_pRasterizerStateSV);
	SAFE_RELEASE(g_pDepthStencilStateSV);

	SAFE_RELEASE(g_pVertexShaderShadow);
	SAFE_RELEASE(g_pPixelShaderShadow);
	SAFE_RELEASE(g_pBlendStateShadow);
	SAFE_RELEASE(g_pDepthStencilStateShadow);

	SAFE_RELEASE(g_pCBuffer);

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
		SAFE_RELEASE(g_pDepthStencilView);					// �[�x/�X�e���V�� �r���[�̉��
		SAFE_RELEASE(g_pDepthStencil);						// �[�x/�X�e���V�� �e�N�X�`���̉��

		// �o�b�t�@�̕ύX
		g_pSwapChain->ResizeBuffers(3, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

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

//		case VK_F2:		// [F2]�L�[�Ő[�x�o�b�t�@�̃��[�h��؂�ւ���
//			g_bDepthMode = !g_bDepthMode;
//			break;

		case VK_F2:		// [F2]�L�[�ŃV���h�E �{�����[���̃��[�h��؂�ւ���
			g_bShadowVolumeMode1 = !g_bShadowVolumeMode1;
			break;

		case VK_F5:		// [F5]�L�[�ŉ�ʃ��[�h��؂�ւ���
			if (g_pSwapChain != NULL) {
				g_pSwapChain->GetFullscreenState(&fullscreen, NULL);
				g_pSwapChain->SetFullscreenState(!fullscreen, NULL);
			}
			break;

		case VK_UP:	//���_���߂Â���
			g_fEye -= 0.1f;
			if (g_fEye < 2.5f)
				g_fEye = 2.5f;
			break;

		case VK_DOWN:	// ���_����������
			g_fEye += 0.1f;
			if (g_fEye > 20.0f)
				g_fEye = 20.0f;
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
	}

	// ��ʂ̍X�V
	hr = Render();
	if (hr == DXGI_STATUS_OCCLUDED) {
		g_bStandbyMode = true;  // �X�^���o�C ���[�h�ɓ���
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

	// XNA Math���C�u�����̃T�|�[�g �`�F�b�N
    if (XMVerifyCPUSupport() != TRUE)
	{
		DXTRACE_MSG(L"WinMain XMVerifyCPUSupport");
		return 0;
	}

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
