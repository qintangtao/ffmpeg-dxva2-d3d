// ffmpeg_DXVA_decoder.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "ffmpeg_DXVA_decoder.h"

#define MAX_LOADSTRING 100
//#define ENABLED_SAVE_FILE

#pragma warning(disable : 4996)

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

HANDLE g_hdecodeThread = NULL;
HWND   g_hwWnd = NULL;
BOOL   g_bDecodeThreadCanRun = FALSE;

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FFMPEG_DXVA_DECODER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FFMPEG_DXVA_DECODER));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FFMPEG_DXVA_DECODER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FFMPEG_DXVA_DECODER);
	//wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

//#define CreateWindowW(lpClassName, lpWindowName, dwStyle, x, y,\
//nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)\

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		g_bDecodeThreadCanRun = TRUE;
		g_hwWnd = hWnd;
		g_hdecodeThread = CreateThread(NULL, 0, ThreadProc, 0, 0,NULL);
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		g_bDecodeThreadCanRun = FALSE;
		WaitForSingleObject(g_hdecodeThread, INFINITE);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//CD3DVidRender m_D3DVidRender;
#if 0
AVPixelFormat GetHwFormat(AVCodecContext *s, const AVPixelFormat *pix_fmts)
{
	InputStream* ist = (InputStream*)s->opaque;
	ist->active_hwaccel_id = HWACCEL_DXVA2;
	ist->hwaccel_pix_fmt = AV_PIX_FMT_DXVA2_VLD;
	return ist->hwaccel_pix_fmt;
}
#endif

static AVBufferRef *hw_device_ctx = NULL;
static enum AVPixelFormat hw_pix_fmt;

static enum AVPixelFormat get_hw_format(AVCodecContext *ctx,
	const enum AVPixelFormat *pix_fmts)
{
	const enum AVPixelFormat *p;

	for (p = pix_fmts; *p != -1; p++) {
		if (*p == hw_pix_fmt)
			return *p;
	}

	fprintf(stderr, "Failed to get HW surface format.\n");
	return AV_PIX_FMT_NONE;
}

#if CONFIG_D3D11VA
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_3.h>

ID3D11Device        *m_pD3D11Device;
ID3D11DeviceContext *m_pD3D11DeviceContext;
ID3D11VideoDevice   *m_pD3D11VideoDevice;
ID3D11VideoContext  *m_pD3D11VideoContext;

static IDXGIDevice1 *m_pDXGIdevice = NULL;
static IDXGIAdapter *m_pDXGIAdapter = NULL;
static IDXGIFactory2 *m_pIDXGIFactory3 = NULL;
static IDXGIOutput *m_pDXGIOutput = NULL;
static IDXGISwapChain1 *m_swapChain1 = NULL;
static IDXGISwapChain2 *m_swapchain2 = NULL;
static ID3D11RenderTargetView *m_pRenderTargetView = NULL;
static ID3D11VideoProcessorEnumerator *m_pD3D11VideoProcessorEnumerator = NULL;
static ID3D11VideoProcessor *m_pD3D11VideoProcessor = NULL;

static HRESULT d3d11_init_display()
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pBackBuffer = NULL;

	try {
		hr = m_pD3D11Device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&m_pDXGIdevice);
		if (FAILED(hr)) return hr;

		hr = m_pDXGIdevice->GetParent(__uuidof(IDXGIAdapter), (void **)&m_pDXGIAdapter);
		if (FAILED(hr)) return hr;

		hr = m_pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&m_pIDXGIFactory3);
		if (FAILED(hr)) return hr;

		hr = m_pDXGIAdapter->EnumOutputs(0, &m_pDXGIOutput);
		if (FAILED(hr)) return hr;

		RECT rect;
		GetClientRect(g_hwWnd, &rect);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
		memset(&swapChainDesc, 0, sizeof(swapChainDesc));
		swapChainDesc.Width = rect.right - rect.left;
		swapChainDesc.Height = rect.bottom - rect.top;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		//swapChainDesc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_IGNORE;
		//swapChainDesc.Flags = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		swapChainDesc.Flags = 0;

		hr = m_pIDXGIFactory3->CreateSwapChainForHwnd(m_pD3D11Device, g_hwWnd, &swapChainDesc, NULL, NULL, &m_swapChain1);
		if (FAILED(hr)) return hr;

		m_swapchain2 = (IDXGISwapChain2 *)m_swapChain1;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc2;
		m_swapChain1->GetDesc1(&swapChainDesc2);
		
		m_swapchain2->SetMaximumFrameLatency(1);

		//	IF_FAILED_THROW(m_swapchain2->SetFullscreenState(TRUE, NULL));//full screen
		//ResizeSwapChain();

		hr = m_swapchain2->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
		if (FAILED(hr)) return hr;
			
		// Create a render target view
		hr = m_pD3D11Device->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
		pBackBuffer->Release();
		if (FAILED(hr)) return hr;

		// Set new render target
		m_pD3D11DeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);

		D3D11_VIEWPORT VP;
		VP.Width = swapChainDesc.Width;
		VP.Height = swapChainDesc.Height;
		VP.MinDepth = 0.0f;
		VP.MaxDepth = 1.0f;
		VP.TopLeftX = 0;
		VP.TopLeftY = 0;
		m_pD3D11DeviceContext->RSSetViewports(1, &VP);

	}
	catch (HRESULT) {

	}
	return hr;
}

void d3d11va_retrieve_data(AVCodecContext *avctx, AVFrame *frame)
{
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC texture_desc;
	D3D11_TEXTURE2D_DESC bktexture_desc;
	ID3D11VideoProcessorInputView* pD3D11VideoProcessorInputViewIn = NULL;
	ID3D11VideoProcessorOutputView* pD3D11VideoProcessorOutputView = NULL;
	ID3D11Texture2D *pDXGIBackBuffer = NULL;

	if (frame == NULL || frame->format != AV_PIX_FMT_D3D11)
		return;

	int index = (intptr_t)frame->data[1];
	ID3D11Texture2D* hwTexture = (ID3D11Texture2D*)frame->data[0];

	hwTexture->GetDesc(&texture_desc);

	hr = m_swapchain2->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pDXGIBackBuffer);
	if (FAILED(hr)) return;

	pDXGIBackBuffer->GetDesc(&bktexture_desc);

#ifndef NDEBUG
	RECT rect;
	GetClientRect(g_hwWnd, &rect);

	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;

	if (w != bktexture_desc.Width || h != bktexture_desc.Height) {	
		hr = m_swapchain2->ResizeBuffers(
			2,
			w, h,
			DXGI_FORMAT_UNKNOWN,
			0);
		//if (hr == DXGI_ERROR_DEVICE_REMOVED) {}
		//else if (FAILED(hr)) return;
	}
#endif

	if (!m_pD3D11VideoProcessorEnumerator || !m_pD3D11VideoProcessor)
	{
		if (m_pD3D11VideoProcessorEnumerator)
			m_pD3D11VideoProcessorEnumerator->Release();
		if (m_pD3D11VideoProcessor)
			m_pD3D11VideoProcessor->Release();

		//SAFE_RELEASE(m_pD3D11VideoProcessorEnumerator);
		//SAFE_RELEASE(m_pD3D11VideoProcessor);

		D3D11_VIDEO_PROCESSOR_CONTENT_DESC ContentDesc;
		ZeroMemory(&ContentDesc, sizeof(ContentDesc));
		ContentDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
		ContentDesc.InputWidth = texture_desc.Width;
		ContentDesc.InputHeight = texture_desc.Height;
		ContentDesc.OutputWidth = bktexture_desc.Width;
		ContentDesc.OutputHeight = bktexture_desc.Height;
		ContentDesc.Usage = D3D11_VIDEO_USAGE_PLAYBACK_NORMAL;
		hr = m_pD3D11VideoDevice->CreateVideoProcessorEnumerator(&ContentDesc, &m_pD3D11VideoProcessorEnumerator);
		if (FAILED(hr)) return;

		UINT uiFlags;
		DXGI_FORMAT VP_Output_Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		hr = m_pD3D11VideoProcessorEnumerator->CheckVideoProcessorFormat(VP_Output_Format, &uiFlags);
		if (FAILED(hr)) return;

		DXGI_FORMAT VP_input_Format = texture_desc.Format;
		hr = m_pD3D11VideoProcessorEnumerator->CheckVideoProcessorFormat(VP_input_Format, &uiFlags);
		if (FAILED(hr)) return;

		//  NV12 surface to RGB backbuffer
		RECT srcrc = { 0, 0, (LONG)texture_desc.Width, (LONG)texture_desc.Height };
		RECT destcrc = { 0, 0, (LONG)bktexture_desc.Width, (LONG)bktexture_desc.Height };

		hr = m_pD3D11VideoDevice->CreateVideoProcessor(m_pD3D11VideoProcessorEnumerator, 0, &m_pD3D11VideoProcessor);
		if (FAILED(hr)) return;

		m_pD3D11VideoContext->VideoProcessorSetStreamFrameFormat(m_pD3D11VideoProcessor, 0, D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE);
		m_pD3D11VideoContext->VideoProcessorSetStreamOutputRate(m_pD3D11VideoProcessor, 0, D3D11_VIDEO_PROCESSOR_OUTPUT_RATE_NORMAL, TRUE, NULL);

		m_pD3D11VideoContext->VideoProcessorSetStreamSourceRect(m_pD3D11VideoProcessor, 0, TRUE, &srcrc);
		m_pD3D11VideoContext->VideoProcessorSetStreamDestRect(m_pD3D11VideoProcessor, 0, TRUE, &destcrc);
		m_pD3D11VideoContext->VideoProcessorSetOutputTargetRect(m_pD3D11VideoProcessor, TRUE, &destcrc);


		D3D11_VIDEO_COLOR color;
		color.YCbCr = { 0.0625f, 0.5f, 0.5f, 0.5f }; // black color
		m_pD3D11VideoContext->VideoProcessorSetOutputBackgroundColor(m_pD3D11VideoProcessor, TRUE, &color);

	}

	//        IF_FAILED_THROW(m_pD3D11Device->CreateRenderTargetView(m_pDXGIBackBuffer, nullptr, &_rtv));
	//        D3D11_VIEWPORT VP;
	//        ZeroMemory(&VP, sizeof(VP));
	//        VP.Width = bktexture_desc.Width;
	//        VP.Height = bktexture_desc.Height;
	//        VP.MinDepth = 0.0f;
	//        VP.MaxDepth = 1.0f;
	//        VP.TopLeftX = 0;
	//        VP.TopLeftY = 0;
	//        m_pD3D11DeviceContext->RSSetViewports(1, &VP);

	D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC pInDesc;
	ZeroMemory(&pInDesc, sizeof(pInDesc));
	D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC InputViewDesc;

	pInDesc.FourCC = 0;
	pInDesc.ViewDimension = D3D11_VPIV_DIMENSION_TEXTURE2D;
	pInDesc.Texture2D.MipSlice = 0;
	pInDesc.Texture2D.ArraySlice = index;

	hr = m_pD3D11VideoDevice->CreateVideoProcessorInputView(hwTexture, m_pD3D11VideoProcessorEnumerator, &pInDesc, &pD3D11VideoProcessorInputViewIn);
	if (FAILED(hr)) return;

	D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC pOutDesc;
	ZeroMemory(&pOutDesc, sizeof(pOutDesc));

	pOutDesc.ViewDimension = D3D11_VPOV_DIMENSION_TEXTURE2D;
	pOutDesc.Texture2D.MipSlice = 0;

	hr = m_pD3D11VideoDevice->CreateVideoProcessorOutputView(pDXGIBackBuffer, m_pD3D11VideoProcessorEnumerator, &pOutDesc, &pD3D11VideoProcessorOutputView);
	if (FAILED(hr)) return;

	D3D11_VIDEO_PROCESSOR_STREAM StreamData;
	ZeroMemory(&StreamData, sizeof(StreamData));
	StreamData.Enable = TRUE;
	StreamData.OutputIndex = 0;
	StreamData.InputFrameOrField = 0;
	StreamData.PastFrames = 0;
	StreamData.FutureFrames = 0;
	StreamData.ppPastSurfaces = NULL;
	StreamData.ppFutureSurfaces = NULL;
	StreamData.pInputSurface = pD3D11VideoProcessorInputViewIn;
	StreamData.ppPastSurfacesRight = NULL;
	StreamData.ppFutureSurfacesRight = NULL;

	hr = m_pD3D11VideoContext->VideoProcessorBlt(m_pD3D11VideoProcessor, pD3D11VideoProcessorOutputView, 0, 1, &StreamData);
	if (FAILED(hr)) return;

	DXGI_PRESENT_PARAMETERS parameters;
	ZeroMemory(&parameters, sizeof(parameters));

	if (m_swapchain2 != NULL)
	{
		//hr = m_swapchain2->Present1(0, DXGI_PRESENT_ALLOW_TEARING, &parameters);
		m_swapchain2->Present1(0, DXGI_PRESENT_DO_NOT_WAIT, &parameters);
	}

	//hwTexture->Release();
	pD3D11VideoProcessorOutputView->Release();
	pD3D11VideoProcessorInputViewIn->Release();
	pDXGIBackBuffer->Release();

	//SAFE_RELEASE(hwTexture);
	//SAFE_RELEASE(_rtv);
	//SAFE_RELEASE(pD3D11VideoProcessorOutputView);
	//SAFE_RELEASE(pD3D11VideoProcessorInputViewIn);

}

#endif

static int hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type)
{
	AVDictionary *opts = NULL;
	char *device = NULL;
	int err = 0;

	if (type == AV_HWDEVICE_TYPE_DXVA2) {
		av_dict_set_int(&opts, "width", ctx->width, 0);
		av_dict_set_int(&opts, "height", ctx->height, 0);
		av_dict_set_int(&opts, "hwnd", (int64_t)g_hwWnd, 0);
	}
	else if (type == AV_HWDEVICE_TYPE_D3D11VA) {
		device = "0";
	}

	if ((err = av_hwdevice_ctx_create(&hw_device_ctx, type,
		device, opts, 0)) < 0) {
		fprintf(stderr, "Failed to create specified HW device.\n");
		av_dict_free(&opts);
		return err;
	}
	ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);

#if CONFIG_D3D11VA
	if (type == AV_HWDEVICE_TYPE_D3D11VA) {
		AVHWDeviceContext* hw_device_ctx = (AVHWDeviceContext*)ctx->hw_device_ctx->data;
		AVD3D11VADeviceContext *d3d11_device_ctx = (AVD3D11VADeviceContext*)hw_device_ctx->hwctx;

		m_pD3D11Device = d3d11_device_ctx->device;
		m_pD3D11DeviceContext = d3d11_device_ctx->device_context;
		m_pD3D11VideoDevice = d3d11_device_ctx->video_device;
		m_pD3D11VideoContext = d3d11_device_ctx->video_context;

		d3d11_init_display();
	}
#endif

	av_dict_free(&opts);
	return err;
}

static void calculate_display_rect(RECT *rect,
	int scr_xleft, int scr_ytop, int scr_width, int scr_height,
	int pic_width, int pic_height, AVRational pic_sar)
{
	float aspect_ratio;
	int width, height, x, y;

	if (pic_sar.num == 0)
		aspect_ratio = 0;
	else
		aspect_ratio = (float)av_q2d(pic_sar);

	if (aspect_ratio <= 0.0)
		aspect_ratio = 1.0;
	aspect_ratio *= (float)pic_width / (float)pic_height;

	/* XXX: we suppose the screen has a 1.0 pixel ratio */
	height = scr_height;
	width = lrint(height * aspect_ratio) & ~1;
	if (width > scr_width) {
		width = scr_width;
		height = lrint(width / aspect_ratio) & ~1;
	}
	x = (scr_width - width) / 2;
	y = (scr_height - height) / 2;
	rect->left = scr_xleft + x;
	rect->top = scr_ytop + y;
	rect->right = rect->left + FFMAX(width, 1);
	rect->bottom = rect->top + FFMAX(height, 1);
}


typedef struct DXVA2DevicePriv {
	HMODULE d3dlib;
	HMODULE dxva2lib;

	HANDLE device_handle;

	IDirect3D9       *d3d9;
	IDirect3DDevice9 *d3d9device;
} DXVA2DevicePriv;


static IDirect3DSurface9 * m_pDirect3DSurfaceRender = NULL;
static IDirect3DSurface9 * m_pBackBuffer = NULL;
static CRITICAL_SECTION cs;
static FILE *output_file = NULL;
static int dxva2_retrieve_data(AVCodecContext *avctx, AVFrame *frame)
{
	AVHWDeviceContext  *device_ctx = (AVHWDeviceContext*)avctx->hw_device_ctx->data;
	DXVA2DevicePriv    *priv = (DXVA2DevicePriv       *)device_ctx->user_opaque;
	LPDIRECT3DSURFACE9 surface = (LPDIRECT3DSURFACE9)frame->data[3];
	RECT m_rtViewport;
	RECT rect = { 0 };

	EnterCriticalSection(&cs);
	GetClientRect(g_hwWnd, &m_rtViewport);

	// reset
#if 0
	D3DDISPLAYMODE d3ddm;
	D3DPRESENT_PARAMETERS d3dpp;
	IDirect3D9_GetAdapterDisplayMode(priv->d3d9, D3DADAPTER_DEFAULT, &d3ddm);
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferCount = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Flags = D3DPRESENTFLAG_VIDEO,
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.hDeviceWindow = g_hwWnd;
	d3dpp.BackBufferWidth = m_rtViewport.right - m_rtViewport.left;
	d3dpp.BackBufferHeight = m_rtViewport.bottom - m_rtViewport.top;
	d3dpp.EnableAutoDepthStencil = FALSE;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	IDirect3DDevice9_Reset(priv->d3d9device, &d3dpp);
#endif

	IDirect3DDevice9Ex_Clear(priv->d3d9device, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	IDirect3DDevice9Ex_BeginScene(priv->d3d9device);
	if (m_pBackBuffer) {
		IDirect3DSurface9_Release(m_pBackBuffer);
		m_pBackBuffer = NULL;
	}
	IDirect3DDevice9Ex_GetBackBuffer(priv->d3d9device, 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);

	RECT SourceRect = { 0,0,((~0 - 1)&frame->width),((~0 - 1)&frame->height) };
	IDirect3DDevice9Ex_StretchRect(priv->d3d9device, surface, &SourceRect, m_pBackBuffer, NULL, D3DTEXF_LINEAR);
	//IDirect3DDevice9Ex_StretchRect(priv->d3d9device, surface, &SourceRect, m_pBackBuffer, NULL, D3DTEXF_POINT);
	
	IDirect3DDevice9Ex_EndScene(priv->d3d9device);
	
#if 0
	GetClientRect(g_hwWnd, &m_rtViewport);
	calculate_display_rect(&rect, m_rtViewport.left, m_rtViewport.top, m_rtViewport.right - m_rtViewport.left, m_rtViewport.bottom - m_rtViewport.top, frame->width, frame->height, frame->sample_aspect_ratio);
	IDirect3DDevice9Ex_Present(priv->d3d9device, NULL, &rect, NULL, NULL);
#else
	IDirect3DDevice9Ex_Present(priv->d3d9device, NULL, NULL, NULL, NULL);
#endif

	LeaveCriticalSection(&cs);

	return 0;
}

static int decode_write(AVCodecContext *avctx, AVPacket *packet)
{
	AVFrame *frame = NULL;
#ifdef ENABLED_SAVE_FILE
	AVFrame *sw_frame = NULL;
	uint8_t *buffer = NULL;
	int size;
#endif
	int ret;

	ret = avcodec_send_packet(avctx, packet);
	if (ret < 0) {
		fprintf(stderr, "Error during decoding\n");
		//return ret;
		return 0;
	}

	while (1) {
		if (!(frame = av_frame_alloc())) {
			fprintf(stderr, "Can not alloc frame\n");
			ret = AVERROR(ENOMEM);
			goto fail;
		}

#ifdef ENABLED_SAVE_FILE
		if (!(sw_frame = av_frame_alloc())) {
			fprintf(stderr, "Can not alloc frame\n");
			ret = AVERROR(ENOMEM);
			goto fail;
		}
#endif

		ret = avcodec_receive_frame(avctx, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
			av_frame_free(&frame);
#ifdef ENABLED_SAVE_FILE
			av_frame_free(&sw_frame);
#endif
			return 0;
		}
		else if (ret < 0) {
			fprintf(stderr, "Error while decoding\n");
			goto fail;
		}

#ifndef ENABLED_SAVE_FILE
		if (frame->format == AV_PIX_FMT_DXVA2_VLD)
			dxva2_retrieve_data(avctx, frame);
#if CONFIG_D3D11VA
		else if (frame->format == AV_PIX_FMT_D3D11)
			d3d11va_retrieve_data(avctx, frame);
#endif

		av_usleep(30000);
#else
		/* retrieve data from GPU to CPU */
		if ((ret = av_hwframe_transfer_data(sw_frame, frame, 0)) < 0) {
			fprintf(stderr, "Error transferring the data to system memory\n");
			goto fail;
		}

		//const char *pf = av_get_pix_fmt_name((enum AVPixelFormat)sw_frame->format);
		printf("Pixel format: %s\n", av_get_pix_fmt_name((enum AVPixelFormat)sw_frame->format));

		size = av_image_get_buffer_size((enum AVPixelFormat)sw_frame->format, sw_frame->width,
			sw_frame->height, 1);

		buffer = (uint8_t *)av_malloc(size);
		if (!buffer) {
			fprintf(stderr, "Can not alloc buffer\n");
			ret = AVERROR(ENOMEM);
			goto fail;
		}
		ret = av_image_copy_to_buffer(buffer, size,
			(const uint8_t * const *)sw_frame->data,
			(const int *)sw_frame->linesize, (enum AVPixelFormat)sw_frame->format,
			sw_frame->width, sw_frame->height, 1);
		if (ret < 0) {
			fprintf(stderr, "Can not copy image to buffer\n");
			goto fail;
		}

		if ((ret = fwrite(buffer, 1, size, output_file)) < 0) {
			fprintf(stderr, "Failed to dump raw data.\n");
			goto fail;
		}
#endif

	fail:
		av_frame_free(&frame);
#ifdef ENABLED_SAVE_FILE
		av_frame_free(&sw_frame);
		av_freep(&buffer);
#endif
		if (ret < 0)
			return ret;
	}
}

DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	AVFormatContext *fc = NULL;
	AVPacket *packet = NULL;
	AVCodecContext *codecctx;
	const AVCodec *codec;
	enum AVHWDeviceType type;
	int ret, i, videoindex;

	//const char *filename = "H:/BaiduNetdiskDownload/Android/01.mp4";
	const char *filename = "H:/BaiduNetdiskDownload/Android/02.mp4";
	//const char *filename = "rtsp://192.168.1.64:555/channel5";
	
#ifdef  ENABLED_SAVE_FILE
	output_file = fopen("H:/BaiduNetdiskDownload/Android/02.nv12", "w+b");
#endif //  ENABLED_SAVE_FILE


	InitializeCriticalSection(&cs);

	packet = av_packet_alloc();
	if (!packet) {
		fprintf(stderr, "Failed to allocate AVPacket\n");
		exit(EXIT_FAILURE);
	}

	ret = avformat_open_input(&fc, filename, NULL, NULL);//打开文件
	if (ret < 0) {
		printf("error %x in avformat_open_input\n", ret);
		exit(EXIT_FAILURE);
	}

	ret = avformat_find_stream_info(fc, NULL);//取出流信息
	if (ret < 0) {
		printf("error %x in avformat_find_stream_info\n", ret);
		exit(EXIT_FAILURE);
	}

	//查找视频流和音频流
	av_dump_format(fc, 0, filename, 0);//列出输入文件的相关流信息

	videoindex = -1;

	for (i = 0; i < fc->nb_streams; i++)
	{
		if (fc->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
		}
	}
	if (videoindex == -1) {
		av_log(NULL, AV_LOG_DEBUG, "can't find video stream\n");
		exit(EXIT_FAILURE);
	}

	codec = avcodec_find_decoder(fc->streams[videoindex]->codecpar->codec_id);	//根据流信息找到解码器
	if (!codec) {
		printf("decoder not found\n");
		exit(EXIT_FAILURE);
	}

	codecctx = avcodec_alloc_context3(codec);
	if (!codecctx) {
		printf("codecctx not found\n");
		exit(EXIT_FAILURE);
	}

	if (avcodec_parameters_to_context(codecctx, fc->streams[videoindex]->codecpar) < 0)
		exit(EXIT_FAILURE);

	//type = av_hwdevice_find_type_by_name("dxva2");
	type = av_hwdevice_find_type_by_name("d3d11va");
	
	if (type == AV_HWDEVICE_TYPE_NONE)
		exit(EXIT_FAILURE);

	for (i = 0;; i++) {
		const AVCodecHWConfig *config = avcodec_get_hw_config(codec, i);
		if (!config) {
			fprintf(stderr, "Decoder %s does not support device type.\n",
				codec->name);
			exit(EXIT_FAILURE);
		}
		if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
			config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX &&
			config->device_type == type) {
			hw_pix_fmt = config->pix_fmt;
			break;
		}
	}

	codecctx->get_format = get_hw_format;

	if (hw_decoder_init(codecctx, type) < 0)
		exit(EXIT_FAILURE);

	codecctx->pix_fmt = hw_pix_fmt;

	ret = avcodec_open2(codecctx, codec, NULL);
	if (ret < 0) {
		printf("error %x in avcodec_open2\n", ret);
		exit(EXIT_FAILURE);
	}

	while (g_bDecodeThreadCanRun && ret >= 0) {
		if ((ret = av_read_frame(fc, packet)) < 0)
			break;

		if (packet->stream_index == videoindex)
			ret = decode_write(codecctx, packet);

		av_packet_unref(packet);
	}

	ret = decode_write(codecctx, NULL);

	av_packet_free(&packet);
	avcodec_close(codecctx);
	avformat_close_input(&fc);
	if (output_file)
		fclose(output_file);
	DeleteCriticalSection(&cs);

	exit(EXIT_SUCCESS);
	return 0;
}