# ffmpeg-dxva2-d3d
Render directly after GPU decoding

### ffmpeg
> version 6.0

---

### test

* set path
```
DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	const char *filename = "H:/test.mp4";
}
```

* set device
```
DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	//type = av_hwdevice_find_type_by_name("dxva2");
	type = av_hwdevice_find_type_by_name("d3d11va");
}
```

---

### DXVA2+D3D9

#### decoder
* av_hwdevice_ctx_create
```
av_dict_set_int(&opts, "width", ctx->width, 0);
av_dict_set_int(&opts, "height", ctx->height, 0);
av_dict_set_int(&opts, "hwnd", (int64_t )g_hwWnd, 0);
av_hwdevice_ctx_create(&hw_device_ctx, type, NULL, NULL, 0);
```

#### render

* decode_write
```
if (frame->format == AV_PIX_FMT_DXVA2_VLD)
		dxva2_retrieve_data(avctx, frame);
```

* struct DXVA2DevicePriv
```
typedef struct DXVA2DevicePriv {
	HMODULE d3dlib;
	HMODULE dxva2lib;
	HANDLE device_handle;
	IDirect3D9       *d3d9;
	IDirect3DDevice9 *d3d9device;
} DXVA2DevicePriv;
```

* dxva2_retrieve_data
```
AVHWDeviceContext  *device_ctx = (AVHWDeviceContext*)avctx->hw_device_ctx->data;
DXVA2DevicePriv    *priv = (DXVA2DevicePriv       *)device_ctx->user_opaque;
LPDIRECT3DSURFACE9 surface = (LPDIRECT3DSURFACE9)frame->data[3];
IDirect3DDevice9Ex_Clear(priv->d3d9device, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
IDirect3DDevice9Ex_BeginScene(priv->d3d9device);
if (m_pBackBuffer) {
	IDirect3DSurface9_Release(m_pBackBuffer);
	m_pBackBuffer = NULL;
}
IDirect3DDevice9Ex_GetBackBuffer(priv->d3d9device, 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
RECT SourceRect = { 0,0,((~0 - 1)&frame->width),((~0 - 1)&frame->height) };
IDirect3DDevice9Ex_StretchRect(priv->d3d9device, surface, &SourceRect, m_pBackBuffer, NULL, D3DTEXF_LINEAR);
IDirect3DDevice9Ex_EndScene(priv->d3d9device);
GetClientRect(g_hwWnd, &m_rtViewport);
IDirect3DDevice9Ex_Present(priv->d3d9device, NULL, &m_rtViewport, NULL, NULL);
```

---

### D3D11VA+D3D11

#### decoder
* av_hwdevice_ctx_create
```
av_dict_set_int(&opts, "width", ctx->width, 0);
av_dict_set_int(&opts, "height", ctx->height, 0);
av_dict_set_int(&opts, "hwnd", (int64_t )g_hwWnd, 0);
av_hwdevice_ctx_create(&hw_device_ctx, type, NULL, NULL, 0);
m_pD3D11Device = d3d11_device_ctx->device;
m_pD3D11DeviceContext = d3d11_device_ctx->device_context;
m_pD3D11VideoDevice = d3d11_device_ctx->video_device;
m_pD3D11VideoContext = d3d11_device_ctx->video_context;
d3d11_init(g_hwWnd);
```

* d3d11_init
```
RECT rect;
GetClientRect(hWnd, &rect);
DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
swapChainDesc.Width = rect.right - rect.left;
swapChainDesc.Height = rect.bottom - rect.top;
swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
swapChainDesc.Stereo = FALSE;
swapChainDesc.SampleDesc.Count = 1;
swapChainDesc.SampleDesc.Quality = 0;
swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
swapChainDesc.BufferCount = 2;
swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
swapChainDesc.Flags = 0;
pIDXGIFactory3->CreateSwapChainForHwnd(m_pD3D11Device, hWnd, &swapChainDesc, NULL, NULL, &pSwapChain1);
```

#### render

* decode_write
```
if (frame->format == AV_PIX_FMT_D3D11)
	d3d11va_retrieve_data(avctx, frame);
```

* d3d11va_retrieve_data
```
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
m_pD3D11VideoContext->VideoProcessorBlt(m_pD3D11VideoProcessor, pD3D11VideoProcessorOutputView, 0, 1, &StreamData);
pSwapChain2->Present1(0, DXGI_PRESENT_DO_NOT_WAIT, &parameters);
```