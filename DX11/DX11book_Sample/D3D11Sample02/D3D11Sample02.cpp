/*----------------------------------------------------------
	Direct3D 11サンプル
		・Microsoft DirectX SDK (February 2010)
		・Visual Studio 2010 Express
		・Windows 7/Vista
		・シェーダモデル4.0
		対応

	D3D11Sample02.cpp
		「グラフィックス カードの情報」
--------------------------------------------------------------*/

#define STRICT					// 型チェックを厳密に行なう
#define WIN32_LEAN_AND_MEAN		// ヘッダーからあまり使われない関数を省く
#define WINVER        0x0600	// Windows Vista以降対応アプリを指定(なくてもよい)
#define _WIN32_WINNT  0x0600	// 同上

#include <d3d11.h>
#include <DXGI.h>
#include <stdio.h>
#include <locale.h>

// 必要なライブラリをリンクする
#pragma comment( lib, "dxgi.lib" )

#pragma comment( lib, "d3d11.lib" )

WCHAR* FormatName[] = {
	L"DXGI_FORMAT_UNKNOWN",
	L"DXGI_FORMAT_R32G32B32A32_TYPELESS",
	L"DXGI_FORMAT_R32G32B32A32_FLOAT",
	L"DXGI_FORMAT_R32G32B32A32_UINT",
	L"DXGI_FORMAT_R32G32B32A32_SINT",
	L"DXGI_FORMAT_R32G32B32_TYPELESS",
	L"DXGI_FORMAT_R32G32B32_FLOAT",
	L"DXGI_FORMAT_R32G32B32_UINT",
	L"DXGI_FORMAT_R32G32B32_SINT",
	L"DXGI_FORMAT_R16G16B16A16_TYPELESS",
	L"DXGI_FORMAT_R16G16B16A16_FLOAT",
	L"DXGI_FORMAT_R16G16B16A16_UNORM",
	L"DXGI_FORMAT_R16G16B16A16_UINT",
	L"DXGI_FORMAT_R16G16B16A16_SNORM",
	L"DXGI_FORMAT_R16G16B16A16_SINT",
	L"DXGI_FORMAT_R32G32_TYPELESS",
	L"DXGI_FORMAT_R32G32_FLOAT",
	L"DXGI_FORMAT_R32G32_UINT",
	L"DXGI_FORMAT_R32G32_SINT",
	L"DXGI_FORMAT_R32G8X24_TYPELESS",
	L"DXGI_FORMAT_D32_FLOAT_S8X24_UINT",
	L"DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS",
	L"DXGI_FORMAT_X32_TYPELESS_G8X24_UINT",
	L"DXGI_FORMAT_R10G10B10A2_TYPELESS",
	L"DXGI_FORMAT_R10G10B10A2_UNORM",
	L"DXGI_FORMAT_R10G10B10A2_UINT",
	L"DXGI_FORMAT_R11G11B10_FLOAT",
	L"DXGI_FORMAT_R8G8B8A8_TYPELESS",
	L"DXGI_FORMAT_R8G8B8A8_UNORM",
	L"DXGI_FORMAT_R8G8B8A8_UNORM_SRGB",
	L"DXGI_FORMAT_R8G8B8A8_UINT",
	L"DXGI_FORMAT_R8G8B8A8_SNORM",
	L"DXGI_FORMAT_R8G8B8A8_SINT",
	L"DXGI_FORMAT_R16G16_TYPELESS",
	L"DXGI_FORMAT_R16G16_FLOAT",
	L"DXGI_FORMAT_R16G16_UNORM",
	L"DXGI_FORMAT_R16G16_UINT",
	L"DXGI_FORMAT_R16G16_SNORM",
	L"DXGI_FORMAT_R16G16_SINT",
	L"DXGI_FORMAT_R32_TYPELESS",
	L"DXGI_FORMAT_D32_FLOAT",
	L"DXGI_FORMAT_R32_FLOAT",
	L"DXGI_FORMAT_R32_UINT",
	L"DXGI_FORMAT_R32_SINT",
	L"DXGI_FORMAT_R24G8_TYPELESS",
	L"DXGI_FORMAT_D24_UNORM_S8_UINT",
	L"DXGI_FORMAT_R24_UNORM_X8_TYPELESS",
	L"DXGI_FORMAT_X24_TYPELESS_G8_UINT",
	L"DXGI_FORMAT_R8G8_TYPELESS",
	L"DXGI_FORMAT_R8G8_UNORM",
	L"DXGI_FORMAT_R8G8_UINT",
	L"DXGI_FORMAT_R8G8_SNORM",
	L"DXGI_FORMAT_R8G8_SINT",
	L"DXGI_FORMAT_R16_TYPELESS",
	L"DXGI_FORMAT_R16_FLOAT",
	L"DXGI_FORMAT_D16_UNORM",
	L"DXGI_FORMAT_R16_UNORM",
	L"DXGI_FORMAT_R16_UINT",
	L"DXGI_FORMAT_R16_SNORM",
	L"DXGI_FORMAT_R16_SINT",
	L"DXGI_FORMAT_R8_TYPELESS",
	L"DXGI_FORMAT_R8_UNORM",
	L"DXGI_FORMAT_R8_UINT",
	L"DXGI_FORMAT_R8_SNORM",
	L"DXGI_FORMAT_R8_SINT",
	L"DXGI_FORMAT_A8_UNORM",
	L"DXGI_FORMAT_R1_UNORM",
	L"DXGI_FORMAT_R9G9B9E5_SHAREDEXP",
	L"DXGI_FORMAT_R8G8_B8G8_UNORM",
	L"DXGI_FORMAT_G8R8_G8B8_UNORM",
	L"DXGI_FORMAT_BC1_TYPELESS",
	L"DXGI_FORMAT_BC1_UNORM",
	L"DXGI_FORMAT_BC1_UNORM_SRGB",
	L"DXGI_FORMAT_BC2_TYPELESS",
	L"DXGI_FORMAT_BC2_UNORM",
	L"DXGI_FORMAT_BC2_UNORM_SRGB",
	L"DXGI_FORMAT_BC3_TYPELESS",
	L"DXGI_FORMAT_BC3_UNORM",
	L"DXGI_FORMAT_BC3_UNORM_SRGB",
	L"DXGI_FORMAT_BC4_TYPELESS",
	L"DXGI_FORMAT_BC4_UNORM",
	L"DXGI_FORMAT_BC4_SNORM",
	L"DXGI_FORMAT_BC5_TYPELESS",
	L"DXGI_FORMAT_BC5_UNORM",
	L"DXGI_FORMAT_BC5_SNORM",
	L"DXGI_FORMAT_B5G6R5_UNORM",
	L"DXGI_FORMAT_B5G5R5A1_UNORM",
	L"DXGI_FORMAT_B8G8R8A8_UNORM",
	L"DXGI_FORMAT_B8G8R8X8_UNORM",
	L"DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM",
    L"DXGI_FORMAT_B8G8R8A8_TYPELESS",
    L"DXGI_FORMAT_B8G8R8A8_UNORM_SRGB",
    L"DXGI_FORMAT_B8G8R8X8_TYPELESS",
    L"DXGI_FORMAT_B8G8R8X8_UNORM_SRGB",
    L"DXGI_FORMAT_BC6H_TYPELESS",
    L"DXGI_FORMAT_BC6H_UF16",
    L"DXGI_FORMAT_BC6H_SF16",
    L"DXGI_FORMAT_BC7_TYPELESS",
    L"DXGI_FORMAT_BC7_UNORM",
    L"DXGI_FORMAT_BC7_UNORM_SRGB"
};

WCHAR* RotationName[] = {
	L"DXGI_MODE_ROTATION_UNSPECIFIED",
	L"DXGI_MODE_ROTATION_IDENTITY",
	L"DXGI_MODE_ROTATION_ROTATE90",
	L"DXGI_MODE_ROTATION_ROTATE180",
	L"DXGI_MODE_ROTATION_ROTATE270"
};

WCHAR* ScanlineName[] = {
	L"DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED",
	L"DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE",
	L"DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST",
	L"DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST"
};

WCHAR* ScalingName[] = {
	L"DXGI_MODE_SCALING_UNSPECIFIED",
	L"DXGI_MODE_SCALING_CENTERED",
	L"DXGI_MODE_SCALING_STRETCHED"
};

WCHAR* FeatureLevelName[] = {
	L"D3D_FEATURE_LEVEL_9_1",
    L"D3D_FEATURE_LEVEL_9_2",
    L"D3D_FEATURE_LEVEL_9_3",
    L"D3D_FEATURE_LEVEL_10_0",
    L"D3D_FEATURE_LEVEL_10_1",
    L"D3D_FEATURE_LEVEL_11_0"
};

UINT FeatureLevelNumber[] = {
	0x9100,
    0x9200,
    0x9300,
    0xa000,
    0xa100,
    0xb000
};

int wmain(int argc, WCHAR* argv[])
{
	// ロケールを設定
	_wsetlocale(LC_ALL, L"Japanese");

	// IDXGIFactoryインターフェイスの取得
	IDXGIFactory* pFactory;
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
	if (FAILED(hr)) {
		wprintf_s(L"CreateDXGIFactoryの実行に失敗\n");
		return 0;
	}

	// IDXGIAdapter(ビデオ カード)の列挙
	UINT iAdapter = 0;
	IDXGIAdapter* pAdapter;
	while(pFactory->EnumAdapters(iAdapter, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		wprintf_s(L"[Adapter(%i)]*************************\n", iAdapter);

		// IDXGIAdapterの情報
		DXGI_ADAPTER_DESC descAdapter;
		hr = pAdapter->GetDesc(&descAdapter);
		if (SUCCEEDED(hr)) {
			wprintf_s(L"Description = %s\n", descAdapter.Description);
			wprintf_s(L"VendorId    = %i\n", descAdapter.VendorId);
			wprintf_s(L"DeviceId    = %i\n", descAdapter.DeviceId);
			wprintf_s(L"SubSysId    = %i\n", descAdapter.SubSysId);
			wprintf_s(L"Revision    = %i\n", descAdapter.Revision);
			wprintf_s(L"DedicatedVideoMemory  = %i\n", descAdapter.DedicatedVideoMemory);
			wprintf_s(L"DedicatedSystemMemory = %i\n", descAdapter.DedicatedSystemMemory);
			wprintf_s(L"SharedSystemMemory    = %i\n", descAdapter.SharedSystemMemory);
		}
		else {
			wprintf_s(L"[ERROR] IDXGIAdapter::GetDescの実行に失敗\n");
		}

		// IDXGIOutput(ディスプレイ)の列挙
		UINT iOutput = 0;
		IDXGIOutput* pOutput;
		while(pAdapter->EnumOutputs(iOutput, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			wprintf_s(L"  [Output(%i)]---------------------\n", iOutput);

			// IDXGIOutputの情報
			DXGI_OUTPUT_DESC descOutput;
			hr = pOutput->GetDesc(&descOutput);
			if (SUCCEEDED(hr)) {
				wprintf_s(L"  DeviceName         = %s\n", descOutput.DeviceName);
				wprintf_s(L"  DesktopCoordinates = (%i, %i, %i, %i)\n",
					descOutput.DesktopCoordinates.left, descOutput.DesktopCoordinates.top,
					descOutput.DesktopCoordinates.right, descOutput.DesktopCoordinates.bottom);
				wprintf_s(L"  AttachedToDesktop  = %i\n", (int)descOutput.AttachedToDesktop);
				wprintf_s(L"  Rotation           = %s\n", RotationName[(int)descOutput.Rotation]);
			}
			else {
				wprintf_s(L"  [ERROR] IDXGIOutput::GetDescの実行に失敗\n");
			}

			// ディスプレイ モードの情報
			for (UINT fnum = 1; fnum < _countof(FormatName); ++fnum) {
				DXGI_FORMAT format = (DXGI_FORMAT)fnum;	// サポートを調べるフォーマット
				UINT flags         = 0;					// プログレッシブ＆スケーリングしないモード

				// フォーマットとスキャンライン設定でサポートされているディスプレイ モード数
				UINT num = 0;
				hr = pOutput->GetDisplayModeList(format, flags, &num, 0);
				if (FAILED(hr)) {
					wprintf_s(L"    [ERROR] IDXGIOutput::GetDisplayModeListでディスプレイ モード数の取得に失敗\n");
					continue;
				}
				if (num == 0)
					continue;
				wprintf_s(L"    [%s = %i]\n", FormatName[fnum], num);
				// サポートされているディスプレイ モードを取得
				DXGI_MODE_DESC* pDescs = new DXGI_MODE_DESC[num];
				hr = pOutput->GetDisplayModeList(format, flags, &num, pDescs);
				if (FAILED(hr))
					wprintf_s(L"    [ERROR] IDXGIOutput::GetDisplayModeListでディスプレイ モードの取得に失敗\n");
				else {
					for (UINT i = 0; i < num; ++i) {
						wprintf_s(L"      [DisplayMode(%i)]\n", i);
						wprintf_s(L"      Width            = %i\n", pDescs[i].Width);
						wprintf_s(L"      Height           = %i\n", pDescs[i].Height);
						wprintf_s(L"      RefreshRate      = %i/%i\n", pDescs[i].RefreshRate.Numerator, pDescs[i].RefreshRate.Denominator);
						wprintf_s(L"      Format           = %s\n", FormatName[(int)pDescs[i].Format]);
						wprintf_s(L"      ScanlineOrdering = %s\n", ScanlineName[(int)pDescs[i].ScanlineOrdering]);
						wprintf_s(L"      Scaling          = %s\n", ScalingName[(int)pDescs[i].Scaling]);
					}
				}
				delete[] pDescs;
			}

			pOutput->Release();
			++iOutput;
		}

		pAdapter->Release();
		++iAdapter;
	}

	pFactory->Release();

	// 各フォーマットでサポートされている機能
	wprintf_s(L"\n");
	ID3D11Device*        g_pd3dDevice = NULL;
	ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	D3D_FEATURE_LEVEL g_FeatureLevelsSupported; // デバイス作成時に返される機能レベル
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
			NULL, 0, D3D11_SDK_VERSION, &g_pd3dDevice,
			&g_FeatureLevelsSupported, &g_pd3dDeviceContext);
	if (FAILED(hr)) {
		hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
				NULL, 0, D3D11_SDK_VERSION, &g_pd3dDevice,
				&g_FeatureLevelsSupported, &g_pd3dDeviceContext);
		if (FAILED(hr)) {
			hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, 0,
					NULL, 0, D3D11_SDK_VERSION, &g_pd3dDevice,
					&g_FeatureLevelsSupported, &g_pd3dDeviceContext);
			if (FAILED(hr)) {
				wprintf_s(L"[ERROR] D3D11CreateDeviceに失敗\n");
				return 0;
			}
			else
				wprintf_s(L"[D3D_DRIVER_TYPE_REFERENCE]\n");
		}
		else
			wprintf_s(L"[D3D_DRIVER_TYPE_WARP]\n");
	}
	else
		wprintf_s(L"[D3D_DRIVER_TYPE_HARDWARE]\n");

	//機能レベル
	for (UINT flevel=0; flevel < _countof(FeatureLevelNumber); ++flevel) {
		if (FeatureLevelNumber[flevel] != g_FeatureLevelsSupported)
			continue;
		wprintf_s(L"[%s]\n", FeatureLevelName[flevel]);
		break;
	}

	//各DXGI_FORMATの機能を調べる
	for (UINT fnum = 1; fnum < _countof(FormatName); ++fnum) {
		DXGI_FORMAT format = (DXGI_FORMAT)fnum;	// 機能を調べるフォーマット
		wprintf_s(L"[%s]\n  ", FormatName[fnum]);

		UINT FormatSupport;
		hr = g_pd3dDevice->CheckFormatSupport(format, &FormatSupport);
		if (FAILED(hr)) {
			wprintf_s(L"  [ERROR] ID3D11Device::CheckFormatSupportに失敗\n");
			continue;
		}

		if (FormatSupport & D3D11_FORMAT_SUPPORT_BUFFER)
			wprintf_s(L"[B ");
		else
			wprintf_s(L"[- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER)
			wprintf_s(L"V ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER)
			wprintf_s(L"I ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_SO_BUFFER)
			wprintf_s(L"S] ");
		else
			wprintf_s(L"-] ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_TEXTURE1D)
			wprintf_s(L"[1 ");
		else
			wprintf_s(L"[- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_TEXTURE2D)
			wprintf_s(L"2 ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_TEXTURE3D)
			wprintf_s(L"3 ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_TEXTURECUBE)
			wprintf_s(L"C] ");
		else
			wprintf_s(L"-] ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_SHADER_LOAD)
			wprintf_s(L"[L ");
		else
			wprintf_s(L"[- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE)
			wprintf_s(L"S ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON)
			wprintf_s(L"C ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT)
			wprintf_s(L"*] ");
		else
			wprintf_s(L"-] ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_MIP)
			wprintf_s(L"[M ");
		else
			wprintf_s(L"[- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN)
			wprintf_s(L"A] ");
		else
			wprintf_s(L"-] ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_RENDER_TARGET)
			wprintf_s(L"R ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_BLENDABLE)
			wprintf_s(L"B ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL)
			wprintf_s(L"D ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_CPU_LOCKABLE)
			wprintf_s(L"C ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE)
			wprintf_s(L"M ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_DISPLAY)
			wprintf_s(L"D ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT)
			wprintf_s(L"C ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET)
			wprintf_s(L"[R ");
		else
			wprintf_s(L"[- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD)
			wprintf_s(L"L] ");
		else
			wprintf_s(L"-] ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_SHADER_GATHER)
			wprintf_s(L"G ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST)
			wprintf_s(L"C ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW)
			wprintf_s(L"V ");
		else
			wprintf_s(L"- ");
		if (FormatSupport & D3D11_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON)
			wprintf_s(L"C\n");
		else
			wprintf_s(L"-\n");
	}

	g_pd3dDeviceContext->Release();
	g_pd3dDevice->Release();

	return 0;
}


