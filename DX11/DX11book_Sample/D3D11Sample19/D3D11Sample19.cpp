/*----------------------------------------------------------
	Direct3D 11�T���v��
		�EMicrosoft DirectX SDK (February 2010)
		�EVisual Studio 2010 Express
		�EWindows 7/Vista
		�E�V�F�[�_���f��4.0
		�Ή�

	D3D11Sample19.cpp
		�u�R���s���[�g�E�V�F�[�_�v
--------------------------------------------------------------*/

#define STRICT					// �^�`�F�b�N�������ɍs�Ȃ�
#define WIN32_LEAN_AND_MEAN		// �w�b�_�[���炠�܂�g���Ȃ��֐����Ȃ�
#define WINVER        0x0600	// Windows Vista�ȍ~�Ή��A�v�����w��(�Ȃ��Ă��悢)
#define _WIN32_WINNT  0x0600	// ����

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// ����}�N��

#define DATASIZE (1920*1080*3)		// �v�Z����f�[�^��
//#define COPYRESOURCE				// CopyResource���\�b�h���g��

#include <windows.h>
#include <mmsystem.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include <locale.h>

// �K�v�ȃ��C�u�����������N����
#pragma comment( lib, "d3d11.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx11d.lib" )
#else
#pragma comment( lib, "d3dx11.lib" )
#endif
#pragma comment( lib, "dxerr.lib" )

#pragma comment( lib, "winmm.lib" )

/*-------------------------------------------
	�O���[�o���ϐ�(�A�v���P�[�V�����֘A)
--------------------------------------------*/
ID3D11Device*        g_pD3DDevice        = NULL;
ID3D11DeviceContext* g_pImmediateContext = NULL;
D3D_FEATURE_LEVEL    g_pFeatureLevels[] =  { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 }; // �@�\���x���̔z��
D3D_FEATURE_LEVEL    g_FeatureLevelsSupported; // �f�o�C�X�쐬���ɕԂ����@�\���x��

ID3D11ComputeShader* g_pComputeShader = NULL;  // �R���s���[�g�E�V�F�[�_
ID3D11Buffer*        g_pCBuffer = NULL;        // �萔�o�b�t�@

ID3D11Buffer* g_pBuffer[2] = { NULL, NULL }; // �o�b�t�@ ���\�[�X
ID3D11Buffer* g_pReadBackBuffer = NULL;      // ���[�h�o�b�N�p�o�b�t�@ ���\�[�X
ID3D11ShaderResourceView*  g_pSRV[2] = { NULL, NULL }; // �V�F�[�_ ���\�[�X �r���[
ID3D11UnorderedAccessView* g_pUAV[2] = { NULL, NULL }; // �A���I�[�_�[�h �A�N�Z�X �r���[

// �V�F�[�_�̃R���p�C�� �I�v�V����
#if defined(DEBUG) || defined(_DEBUG)
UINT g_flagCompile = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION
					| D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
UINT g_flagCompile = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#endif

// �萔�o�b�t�@�̃f�[�^
struct cbCBuffer {
    unsigned int g_iCount;  // ���Z����f�[�^��
	unsigned int dummy[3];
};
cbCBuffer g_cbCBuffer;

/*-------------------------------------------
	�f�o�C�X�̍쐬
--------------------------------------------*/
HRESULT CreateDevice(void)
{
	HRESULT hr;

	// �n�[�h�E�F�A �f�o�C�X������
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
			g_pFeatureLevels, _countof(g_pFeatureLevels), D3D11_SDK_VERSION, &g_pD3DDevice,
			&g_FeatureLevelsSupported, &g_pImmediateContext);
	if (SUCCEEDED(hr)) {
		//�u�R���s���[�g �V�F�[�_�v�u�������o�b�t�@�[�v�u�\�����o�b�t�@�v�̃T�|�[�g����
	    D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
	    g_pD3DDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
	    if(hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x) {
			wprintf_s(L"[D3D_DRIVER_TYPE_HARDWARE]\n");
			return hr;
		}
		SAFE_RELEASE(g_pImmediateContext);
		SAFE_RELEASE(g_pD3DDevice);
	}

	// WARP�f�o�C�X������
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
			g_pFeatureLevels, _countof(g_pFeatureLevels), D3D11_SDK_VERSION, &g_pD3DDevice,
			&g_FeatureLevelsSupported, &g_pImmediateContext);
	if (SUCCEEDED(hr)) {
		//�u�R���s���[�g �V�F�[�_�v�u�������o�b�t�@�[�v�u�\�����o�b�t�@�v�̃T�|�[�g����
	    D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
	    g_pD3DDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
	    if(hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x) {
			wprintf_s(L"[D3D_DRIVER_TYPE_WARP]\n");
			return hr;
		}
		SAFE_RELEASE(g_pImmediateContext);
		SAFE_RELEASE(g_pD3DDevice);
	}

	// ���t�@�����X �f�o�C�X������
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, 0,
			g_pFeatureLevels, _countof(g_pFeatureLevels), D3D11_SDK_VERSION, &g_pD3DDevice,
			&g_FeatureLevelsSupported, &g_pImmediateContext);
	if (SUCCEEDED(hr)) {
		//�u�R���s���[�g �V�F�[�_�v�u�������o�b�t�@�[�v�u�\�����o�b�t�@�v�̃T�|�[�g����
	    D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
	    g_pD3DDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
	    if(hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x) {
			wprintf_s(L"[D3D_DRIVER_TYPE_REFERENCE]\n");
			return hr;
		}
		SAFE_RELEASE(g_pImmediateContext);
		SAFE_RELEASE(g_pD3DDevice);
	}

	// ���s
	return DXTRACE_ERR(L"D3D11CreateDevice", hr);;
}

/*-------------------------------------------
	�V�F�[�_�̍쐬
--------------------------------------------*/
HRESULT CreateShader(void)
{
	HRESULT hr;

	// �R���s���[�g�E�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobVS = NULL;
	hr = D3DX11CompileFromFile(
			L"..\\misc\\D3D11Sample19.sh",  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"CS",          // �uCS�֐��v���V�F�[�_������s�����
			"cs_4_0",      // �R���s���[�g�E�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobVS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR(L"D3DX11CompileShaderFromFile", hr);

	// �R���s���[�g�E�V�F�[�_�̍쐬
	hr = g_pD3DDevice->CreateComputeShader(
			pBlobVS->GetBufferPointer(),// �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobVS->GetBufferSize(),	// �o�C�g�E�R�[�h�̒���
			NULL,
			&g_pComputeShader);			//
	SAFE_RELEASE(pBlobVS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateComputeShader", hr);

	return hr;
}

/*-------------------------------------------
	�萔�o�b�t�@�̍쐬
--------------------------------------------*/
HRESULT CreateCBuffer(void)
{
	HRESULT hr;

	// �萔�o�b�t�@�̒�`
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;    // ���I(�_�C�i�~�b�N)�g�p�@
	cBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER; // �萔�o�b�t�@
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPU���珑������
	cBufferDesc.MiscFlags      = 0;
	cBufferDesc.StructureByteStride = 0;
	cBufferDesc.ByteWidth      = sizeof(cbCBuffer); // �o�b�t�@�E�T�C�Y

	// �萔�o�b�t�@�̍쐬
	hr = g_pD3DDevice->CreateBuffer(&cBufferDesc, NULL, &g_pCBuffer);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateBuffer", hr);

	return hr;
}

/*-------------------------------------------
	���\�[�X�̍쐬
--------------------------------------------*/
HRESULT CreateResource(void)
{
	HRESULT hr;

	//�������p�f�[�^
	FLOAT* initdata = new FLOAT[DATASIZE];
	for (int i=0; i<DATASIZE; ++i)
	    initdata[i] = 1;

	// CPU�ɂ�鍇�v�v�Z
	DWORD time = timeGetTime(); // ���Ԃ̌v���J�n

	FLOAT sum = 0.0f;
	for (int i=0; i<DATASIZE; ++i)
	    sum += initdata[i];

	time = timeGetTime() - time; // ���Ԃ̌v���I��
	wprintf_s(L"[CPU] %u ms [SUM = %f]\n", time, sum);

	// ���\�[�X�̐ݒ�
    D3D11_BUFFER_DESC Desc;
    ZeroMemory(&Desc, sizeof(Desc));
    Desc.ByteWidth = DATASIZE * sizeof(FLOAT); // �o�b�t�@ �T�C�Y
    Desc.Usage     = D3D11_USAGE_DEFAULT;
    Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // �\�����o�b�t�@
    Desc.StructureByteStride = sizeof(FLOAT);

	D3D11_SUBRESOURCE_DATA SubResource;
	SubResource.pSysMem          = initdata;
	SubResource.SysMemPitch      = 0;
	SubResource.SysMemSlicePitch = 0;

	// �ŏ��̓��̓��\�[�X(�f�[�^������������)
    hr = g_pD3DDevice->CreateBuffer(&Desc, &SubResource, &g_pBuffer[0]);
	delete[] initdata;
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateBuffer", hr);

	// �ŏ��̏o�̓��\�[�X
    hr = g_pD3DDevice->CreateBuffer(&Desc, NULL, &g_pBuffer[1]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateBuffer", hr);

	// ���[�h�o�b�N�p�o�b�t�@ ���\�[�X�̍쐬
    ZeroMemory(&Desc, sizeof(Desc));
#ifdef COPYRESOURCE
	Desc.ByteWidth = DATASIZE * sizeof(FLOAT);	// �o�b�t�@ �T�C�Y
#else
    Desc.ByteWidth = sizeof(FLOAT);				// �o�b�t�@ �T�C�Y
#endif
    Desc.Usage     = D3D11_USAGE_STAGING;  // CPU����ǂݏ����\�ȃ��\�[�X
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // CPU����ǂݍ���
    Desc.StructureByteStride = sizeof(FLOAT);

    hr = g_pD3DDevice->CreateBuffer(&Desc, NULL, &g_pReadBackBuffer);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateBuffer", hr);

	return hr;
}
/*-------------------------------------------
	�V�F�[�_ ���\�[�X �r���[�̍쐬
--------------------------------------------*/
HRESULT CreateSRV(void)
{
	HRESULT hr;

	// �V�F�[�_ ���\�[�X �r���[�̐ݒ�
	D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV;
    ZeroMemory(&DescSRV, sizeof(DescSRV));
	DescSRV.Format        = DXGI_FORMAT_UNKNOWN;
	DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    DescSRV.Buffer.ElementWidth = DATASIZE; // �f�[�^��

	// �V�F�[�_ ���\�[�X �r���[�̍쐬
	hr = g_pD3DDevice->CreateShaderResourceView(g_pBuffer[0], &DescSRV, &g_pSRV[0]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateShaderResourceView", hr);

	hr = g_pD3DDevice->CreateShaderResourceView(g_pBuffer[1], &DescSRV, &g_pSRV[1]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateShaderResourceView", hr);

	return hr;
}

/*-------------------------------------------
	�A���I�[�_�[�h�E�A�N�Z�X�E�r���[�̍쐬
--------------------------------------------*/
HRESULT CreateUAV(void)
{
	HRESULT hr;

	// �A���I�[�_�[�h�E�A�N�Z�X�E�r���[�̐ݒ�
	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
	ZeroMemory(&DescUAV, sizeof(DescUAV));
	DescUAV.Format        = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    DescUAV.Buffer.NumElements = DATASIZE; // �f�[�^��

	// �A���I�[�_�[�h�E�A�N�Z�X�E�r���[�̍쐬
	hr = g_pD3DDevice->CreateUnorderedAccessView(g_pBuffer[0], &DescUAV, &g_pUAV[0]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateUnorderedAccessView", hr);

	hr = g_pD3DDevice->CreateUnorderedAccessView(g_pBuffer[1], &DescUAV, &g_pUAV[1]);
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pD3DDevice->CreateUnorderedAccessView", hr);

	return hr;
}

/*-------------------------------------------
	�R���s���[�g �V�F�[�_���g�������Z
--------------------------------------------*/
HRESULT ComputeShader(void)
{
	HRESULT hr = S_OK;
	DWORD time1 = timeGetTime(); // ���Ԃ̌v���J�n

	// �萔�o�b�t�@���o�C���h
	g_pImmediateContext->CSSetConstantBuffers(0, 1, &g_pCBuffer);
	// �V�F�[�_��ݒ�
	g_pImmediateContext->CSSetShader(g_pComputeShader, NULL, 0);

	// ���Z�����s
	unsigned int datacount = DATASIZE;
	bool flag = false;
	ID3D11ShaderResourceView* pViewNULL = NULL;
	do {
		// ���\�[�X�����ւ���
		flag = !flag;

		// �A���I�[�_�[�h�E�A�N�Z�X�E�r���[�ɐݒ肷�邽�߁A
		// ���݂̃V�F�[�_ ���\�[�X �r���[������
		g_pImmediateContext->CSSetShaderResources(0, 1, &pViewNULL);
		// �A���I�[�_�[�h�E�A�N�Z�X�E�r���[�̐ݒ�
		g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &g_pUAV[flag ? 1 : 0], NULL);
		// �V�F�[�_ ���\�[�X �r���[�̐ݒ�
		g_pImmediateContext->CSSetShaderResources(0, 1, &g_pSRV[flag ? 0 : 1]);

		// �萔�o�b�t�@���X�V
		g_cbCBuffer.g_iCount = datacount;
		D3D11_MAPPED_SUBRESOURCE MappedResource = {0}; 
		hr = g_pImmediateContext->Map(g_pCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		if (FAILED(hr))
			return DXTRACE_ERR(L"g_pImmediateContext->Map", hr);
	    memcpy(MappedResource.pData, &g_cbCBuffer, sizeof(g_cbCBuffer));
	    g_pImmediateContext->Unmap(g_pCBuffer, 0);

		// �f�B�X�p�b�`����X���b�h �O���[�v�̐�
		unsigned int threadgroup = (datacount + 127) / 128;
		// �R���s���[�g�E�V�F�[�_�̎��s
		g_pImmediateContext->Dispatch(threadgroup, 1, 1);

		// ���Ɍv�Z����f�[�^��
		datacount = threadgroup;
	} while (datacount > 1);

	DWORD time2 = timeGetTime(); // �v�Z�I������

	// �v�Z���ʂ����[�h�o�b�N�p�o�b�t�@ ���\�[�X�Ƀ_�E�����[�h
#ifdef COPYRESOURCE
	g_pImmediateContext->CopyResource(g_pReadBackBuffer, g_pBuffer[flag ? 1 : 0]);
#else
	D3D11_BOX box;
	box.left  = 0; box.right  = sizeof(FLOAT);
	box.top   = 0; box.bottom = 1;
	box.front = 0; box.back   = 1;
	g_pImmediateContext->CopySubresourceRegion(g_pReadBackBuffer, 0, 0, 0, 0, g_pBuffer[flag ? 1 : 0], 0, &box);
#endif

	DWORD time3 = timeGetTime(); // �_�E�����[�h�I������

	// ���ʂ�ǂݍ���
	D3D11_MAPPED_SUBRESOURCE MappedResource = {0};
    hr = g_pImmediateContext->Map(g_pReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResource);       
	if (FAILED(hr))
		return DXTRACE_ERR(L"g_pImmediateContext->Map", hr);
	FLOAT sum = *(FLOAT*)MappedResource.pData;
    g_pImmediateContext->Unmap(g_pReadBackBuffer, 0);
	
	DWORD time4 = timeGetTime(); // �_�E�����[�h�I������
	wprintf_s(L"[GPU] %u ms - %u ms - %u ms [SUM = %f]\n", time2 - time1, time3 - time2, time4 - time3, sum);

	return hr;
}

/*-------------------------------------------
	main�֐�
--------------------------------------------*/
int wmain(int argc, WCHAR* argv[])
{
	HRESULT hr;

	// ���P�[����ݒ�
	_wsetlocale(LC_ALL, L"Japanese");

	// **********************************************************
	// Direct3D11�f�o�C�X�̍쐬
	hr = CreateDevice();

	// **********************************************************
	// �R���s���[�g�E�V�F�[�_�̍쐬
	if (SUCCEEDED(hr))
		hr = CreateShader();

	// **********************************************************
	// �萔�o�b�t�@�̍쐬
	if (SUCCEEDED(hr))
		hr = CreateCBuffer();

	// **********************************************************
	// ���\�[�X�̍쐬
	if (SUCCEEDED(hr))
		hr = CreateResource();

	// **********************************************************
	// �V�F�[�_ ���\�[�X �r���[�̍쐬
	if (SUCCEEDED(hr))
		hr = CreateSRV();

	// **********************************************************
	// �A���I�[�_�[�h�E�A�N�Z�X�E�r���[�̍쐬
	if (SUCCEEDED(hr))
		hr = CreateUAV();

	// **********************************************************
	// �R���s���[�g�E�V�F�[�_���g�������Z
	if (SUCCEEDED(hr))
		ComputeShader();

	// **********************************************************
	// �J��
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
