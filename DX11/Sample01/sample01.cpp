#define STRICT                     // �^�`�F�b�N�������ɍs��
#define WIN32_LEAN_AND_MEAN        // �w�b�_�[����܂�g���Ȃ��֐����Ȃ��B
#define WINVER        0x0600       // Windows Vista�ڍs�Ή��A�v��
#define _WIN32_WINNT  0x0600       // Windows Vista�ڍs�Ή��A�v��


#include <Windows.h>

#include <D3D11.h>
#include <D3DX11.h>
#include <DxErr.h>

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
//	window_class.hIcon         = LoadIcon(instance_handle, (LPCTSTR)IDI_ICON1);
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