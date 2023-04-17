# ffmpeg-dxva2-d3d
Render directly after GPU decoding

### ffmpeg
> version 6.0

---
### decode
* av_hwdevice_ctx_create
```
	av_dict_set_int(&opts, "width", ctx->width, 0);
	av_dict_set_int(&opts, "height", ctx->height, 0);
	av_dict_set_int(&opts, "hwnd", (int64_t )g_hwWnd, 0);
	 av_hwdevice_ctx_create(&hw_device_ctx, type, NULL, NULL, 0);
```

* av_hwframe_ctx_init
```
	frames_ctx = (AVHWFramesContext *)(hw_frames_ref->data);
	frames_ctx->format = hw_pix_fmt;
	frames_ctx->sw_format = sw_format;
	frames_ctx->width = width;
	frames_ctx->height = height;
	frames_ctx->initial_pool_size = 20;
	av_hwframe_ctx_init(hw_frames_ref);
```

---

### show

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