#define STRICT                     // �^�`�F�b�N�������ɍs��
#define WIN32_LEAN_AND_MEAN        // �w�b�_�[����܂�g���Ȃ��֐����Ȃ��B
#define WINVER        0x0600       // Windows Vista�ڍs�Ή��A�v��
#define _WIN32_WINNT  0x0600       // Windows Vista�ڍs�Ή��A�v��


#include <Windows.h>

#include <crtdbg.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <DxErr.h>

#include "resource.h"

#define SAFE_RELEASE(x) { if(x) { (x)->Release(); (x)=NULL; } } //����}�N��


// �O���[�o���ϐ�
HINSTANCE g_Instance_Handle = NULL;
HWND      g_Window_Handle   = NULL;

WCHAR     g_App_Title[]         = L"Direct3D 11 Sample";
WCHAR     g_Window_Class_Name[] = L"D3D11S01";

//�N�����̕`��̈�T�C�Y
SIZE      g_Initial_Client_Erea_Size = { 640, 480 };  // �E�B���h�E�̃N���C�A���g�̈�

// �O���[�o���ϐ�(Direct3D�֘A)
// �@�\���x���̔z��
D3D_FEATURE_LEVEL g_Feature_Levels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
UINT              g_Num_Of_Feature_Levels    = 3; // �z��̗v�f��
D3D_FEATURE_LEVEL g_Feature_Levels_Supported;     // �f�o�C�X�쐬���ɕԂ����@�\���x��

// �C���^�[�t�F�C�X
ID3D11Device*        g_D3DDevice                = NULL;
ID3D11DeviceContext* g_Immediate_Device_Context = NULL;
IDXGISwapChain*      g_Swap_Chain               = NULL;

ID3D11RenderTargetView* g_Render_Target_View    = NULL;
D3D11_VIEWPORT          g_Viewport[1] ;

ID3D11Texture2D*        g_Depth_Stencil_Texture;
ID3D11DepthStencilView* g_Depth_Stencil_View; 


// �֐���`
LRESULT CALLBACK MainWndProc(HWND window_handle, UINT message, UINT parameter, LONG long_paramter);
HRESULT          InitBackBuffer(void);


HRESULT InitApp(HINSTANCE instance_handle) {
	// �A�v���P�[�V�����̃C���X�^���X�n���h����ۑ�
	g_Instance_Handle = instance_handle;

	// �E�C���h�E�N���X�̓o�^
	WNDCLASS window_class;
	window_class.style         = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc   = (WNDPROC)MainWndProc;
	window_class.cbClsExtra    = 0;
	window_class.cbWndExtra    = 0;
	window_class.hInstance     = instance_handle;
	window_class.hIcon         = LoadIcon(instance_handle, (LPCTSTR)IDI_ICON1);
//  window_class.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	window_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
	window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	window_class.lpszMenuName  = NULL;
	window_class.lpszClassName = g_Window_Class_Name;

	if(!RegisterClass( &window_class)) {
		return DXTRACE_ERR(L"InitApp", GetLastError());
	}

	// ���C���E�B���h�E�쐬
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

	// �E�B���h�E�\��
	ShowWindow(g_Window_Handle, SW_SHOWNORMAL);
	UpdateWindow(g_Window_Handle);

	return S_OK;
}

//
// Direct3D ������
HRESULT InitDirect3D(void)
{
	// �E�B���h�E�̃N���C�A���g�@�G���A
	RECT rect;
	GetClientRect(g_Window_Handle, &rect);
	UINT width  = rect.right  - rect.left;
	UINT height = rect.bottom - rect.top;

	// �f�o�C�X�ƃX���b�v�`�F�C���̍쐬
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

	// �n�[�h�E�F�A�f�o�C�X���쐬
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

	// �o�b�N�@�o�b�t�@�̏�����
	result = InitBackBuffer();
	if( FAILED(result)){
		return DXTRACE_ERR(L"InitDirect3D InitBackBuffer", result);
	}

	return S_OK;
}

//
// �o�b�N�o�b�t�@�̏�����
HRESULT InitBackBuffer() {
	HRESULT result;

	//�X���b�v�`�F�C������ŏ��̃o�b�N�o�b�t�@���擾����B
	ID3D11Texture2D *p_back_buffer; //�o�b�t�@�̃A�N�Z�X�Ɏg���C���^�[�t�F�[�X
	result = g_Swap_Chain->GetBuffer(0,	__uuidof(ID3D11Texture2D),(LPVOID*)&p_back_buffer );
	if(FAILED(result)) {
		return DXTRACE_ERR(L"InitBackBuffer g_Swap_Cahin->Getbuffer", result);
	}

	//�o�b�N�o�b�t�@�̏��
	D3D11_TEXTURE2D_DESC back_buffer_desc;
	p_back_buffer->GetDesc(&back_buffer_desc);

	//�o�b�N�o�b�t�@�̕`��^�[�Q�b�g�r���[�����
	result = g_D3DDevice->CreateRenderTargetView( p_back_buffer, NULL, &g_Render_Target_View );
	SAFE_RELEASE(p_back_buffer);
	if(FAILED(result)) {
		return DXTRACE_ERR(L"InitBackBuffer g_DRDevice->CreateRenderTargetView", result); //���s
	}

	// �[�x/�X�e���V���E�e�N�X�`���̍쐬
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
		return DXTRACE_ERR(L"InitBackBuffer g_pD3DDevice->CreateTexture2D", result); // ���s
	}

	// �[�x/�X�e���V���r���[�̍쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	depth_stencil_view_desc.Format = depth_stencil_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Flags  = 0;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	result = g_D3DDevice->CreateDepthStencilView( g_Depth_Stencil_Texture, &depth_stencil_view_desc, &g_Depth_Stencil_View);
	if(FAILED(result)) {
		return DXTRACE_ERR(L"InitBackBuffer g_D3DDevice->CreateDepthStencilView", result);
	}

	// �r���[�|�[�g�̐ݒ�
	g_Viewport[0].TopLeftX = 0.0f;
	g_Viewport[0].TopLeftY = 0.0f;
	g_Viewport[0].Width    = 640.0f;
	g_Viewport[0].Height   = 480.0f;
	g_Viewport[0].MinDepth = 0.0f;
	g_Viewport[0].MaxDepth = 1.0f;

	return S_OK;
}

//
// ��ʂ̕`�揈��
HRESULT Render(void)
{
	HRESULT result;

	// �`��^�[�Q�b�g�̃N���A
	float clear_color[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	g_Immediate_Device_Context->ClearRenderTargetView( g_Render_Target_View, clear_color );
	
	// �[�x/�X�e���V���l�̃N���A
	g_Immediate_Device_Context->ClearDepthStencilView( g_Depth_Stencil_View, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// ���X�^���C�U�Ƀr���[�|�[�g��ݒ�
	g_Immediate_Device_Context->RSSetViewports(1, g_Viewport);

	// �`��^�[�Q�b�g�E�r���[���o�̓}�l�[�W���[�̕`��^�[�Q�b�g�Ƃ��Đݒ�
	g_Immediate_Device_Context->OMSetRenderTargets(	1, &g_Render_Target_View, g_Depth_Stencil_View);
	
	//
	//�`��(�ȗ�)
	//

	// �o�b�N�E�o�b�t�@�̕\��
	result = g_Swap_Chain->Present( 0,    // ��ʂ𒼂��ɍX�V����
                                  0 );  // ��ʂ����ۂɍX�V����

  return result;
}


//
// Direct3D�̏I������
bool CleanupDirect3D(void)
{
  // �f�o�C�X�E�X�e�[�g�̃N���A
  if(g_Immediate_Device_Context)
    g_Immediate_Device_Context->ClearState();

  // �擾�����C���^�[�t�F�C�X�̉��
  SAFE_RELEASE(g_Depth_Stencil_View);
  SAFE_RELEASE(g_Depth_Stencil_Texture);
  SAFE_RELEASE(g_Render_Target_View);
  SAFE_RELEASE(g_Swap_Chain);
  SAFE_RELEASE(g_Immediate_Device_Context);
  SAFE_RELEASE(g_D3DDevice);

  return true;
}


//
// �A�v���P�[�V�����̏I������
bool CleanupApp( void )
{
  // �E�C���h�E�@�N���X�̓o�^����
  UnregisterClass(g_Window_Class_Name, g_Instance_Handle);
  return true;
}

//
// �E�B���h�E����
LRESULT CALLBACK MainWndProc(HWND window_handle, UINT message, UINT w_param, LONG l_param)
{
  HRESULT result = S_OK;

  switch(message)
  {
  case WM_DESTROY:
    // Direct3D�̏I������
    CleanupDirect3D();
    // �E�C���h�E�����
    PostQuitMessage(0);
    g_Window_Handle = NULL;
    return 0;

  case WM_KEYDOWN:
    // �L�[���͂̏���
    switch(w_param)
    {
    case VK_ESCAPE: // [ESC] �L�[�ŃE�C���h�E�����
      PostMessage(window_handle, WM_CLOSE, 0, 0);
      break;
    }
    break;
  }

  // �f�t�H���g����
  return DefWindowProc(window_handle, message, w_param, l_param);
}

//
// �f�o�C�X�̏�������
HRESULT IsDeviceRemoved(void)
{
  HRESULT result;

  // �f�o�C�X�̏����m�F
  result = g_D3DDevice->GetDeviceRemovedReason();
  switch(result)
  {
  case S_OK:
    break;  // ����
  case DXGI_ERROR_DEVICE_HUNG:
  case DXGI_ERROR_DEVICE_RESET:
    DXTRACE_ERR(L"IsDeviceRemoved g_D3DDevice->GetDeviceRemovedReaseon", result);
    CleanupDirect3D();
    result = InitDirect3D();
    if( FAILED(result))
      return result;
    break;
  case DXGI_ERROR_DEVICE_REMOVED:
  case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
  case DXGI_ERROR_INVALID_CALL:
  default: 
    DXTRACE_ERR(L"IsDeviceRemoved g_pD3DDevice->GetDeviceRemovedReason", result);
		return result;   // �ǂ����悤���Ȃ��̂ŁA�A�v���P�[�V�������I���B
  };

  return S_OK;
}

//
// �A�C�h�����̏���
bool AppIdle(void)
{
  if(!g_D3DDevice)
    return false;

  HRESULT result;
  // �f�o�C�X�̏�������
  result = IsDeviceRemoved();
  if( FAILED(result))
    return false;

  // ��ʂ̍X�V
  Render();

  return true;
}

//
// ���C��
int WINAPI wWinMain( HINSTANCE instance_handle, HINSTANCE, LPWSTR, int)
{
  // �f�o�b�O�@�q�[�v�@�}�l�[�W���ɂ�郁�������蓖�Ă̒ǐՕ��@��ݒ�
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  // �A�v���P�[�V�����Ɋւ��鏉����
  HRESULT result = InitApp(instance_handle);
  if( FAILED( instance_handle))
  {
    DXTRACE_ERR(L"WinMain InitApp", result);
    return 0;
  }

  // Direct3D�̏�����
  result = InitDirect3D();
  if(FAILED(result)) {
    DXTRACE_ERR(L"WinMain InitDirect3D", result);
    CleanupDirect3D();
    CleanupApp();
    return 0;
  }

  // ���b�Z�[�W�@���[�v
  MSG message;
  do
  {
    if( PeekMessage( &message, 0, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&message);
      DispatchMessage( &message);
    }
    else
    {
      // �A�C�h������
      if (!AppIdle()){
        // �G���[������ꍇ�A�A�v���P�[�V�������I������
        DestroyWindow(g_Window_Handle);
      }
    }
  }while(message.message != WM_QUIT);

  // �A�v���P�[�V�����̏I������
  CleanupApp();

  return (int)message.wParam;
}