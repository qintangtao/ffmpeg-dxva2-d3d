// ffmpeg_DXVA_decoder.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ffmpeg_DXVA_decoder.h"

#define MAX_LOADSTRING 100
//#define ENABLED_SAVE_FILE

#pragma warning(disable : 4996)

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

HANDLE g_hdecodeThread = NULL;
HWND   g_hwWnd = NULL;
BOOL   g_bDecodeThreadCanRun = FALSE;

// �˴���ģ���а����ĺ�����ǰ������: 
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

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FFMPEG_DXVA_DECODER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FFMPEG_DXVA_DECODER));

    MSG msg;

    // ����Ϣѭ��: 
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
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
            // �����˵�ѡ��: 
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
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
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

// �����ڡ������Ϣ�������
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

	av_dict_free(&opts);
	return err;
}

static int set_hwframe_ctx(AVCodecContext *ctx, AVBufferRef *hw_device_ctx, enum AVPixelFormat hw_pix_fmt, enum AVPixelFormat sw_format)
{
	int surface_alignment, num_surfaces;
	AVBufferRef *hw_frames_ref;
	AVHWFramesContext *frames_ctx = NULL;
	int err = 0;

	//if (hw_pix_fmt != AV_PIX_FMT_DXVA2_VLD || hw_pix_fmt != AV_PIX_FMT_D3D11)
	//	return 0;

	if (!(hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ctx))) {
		fprintf(stderr, "Failed to create frame context.\n");
		return -1;
	}

	/* decoding MPEG-2 requires additional alignment on some Intel GPUs,
	but it causes issues for H.264 on certain AMD GPUs..... */
	if (ctx->codec_id == AV_CODEC_ID_MPEG2VIDEO)
		surface_alignment = 32;
	/* the HEVC DXVA2 spec asks for 128 pixel aligned surfaces to ensure
	all coding features have enough room to work with */
	else if (ctx->codec_id == AV_CODEC_ID_HEVC || ctx->codec_id == AV_CODEC_ID_AV1)
		surface_alignment = 128;
	else
		surface_alignment = 16;

	/* 1 base work surface */
	num_surfaces = 1;

	/* add surfaces based on number of possible refs */
	if (ctx->codec_id == AV_CODEC_ID_H264 || ctx->codec_id == AV_CODEC_ID_HEVC)
		num_surfaces += 16;
	else if (ctx->codec_id == AV_CODEC_ID_VP9 || ctx->codec_id == AV_CODEC_ID_AV1)
		num_surfaces += 8;
	else
		num_surfaces += 2;

	// If the user has requested that extra output surfaces be
	// available then add them here.
	if (ctx->extra_hw_frames > 0)
		num_surfaces += ctx->extra_hw_frames;

	// If frame threading is enabled then an extra surface per thread
	// is also required.
	if (ctx->active_thread_type & FF_THREAD_FRAME)
		num_surfaces += ctx->thread_count;

	frames_ctx = (AVHWFramesContext *)(hw_frames_ref->data);
	frames_ctx->format = hw_pix_fmt;
	frames_ctx->sw_format = sw_format;
	frames_ctx->width = FFALIGN(ctx->width, surface_alignment);
	frames_ctx->height = FFALIGN(ctx->height, surface_alignment);
	frames_ctx->initial_pool_size = num_surfaces;

#if CONFIG_DXVA2
	if (frames_ctx->format == AV_PIX_FMT_DXVA2_VLD) {
		AVDXVA2FramesContext *frames_hwctx = (AVDXVA2FramesContext *)frames_ctx->hwctx;

		frames_hwctx->surface_type = DXVA2_VideoDecoderRenderTarget;
	}
#endif

#if CONFIG_D3D11VA
	if (frames_ctx->format == AV_PIX_FMT_D3D11) {
		AVD3D11VAFramesContext *frames_hwctx = (AVD3D11VAFramesContext *)frames_ctx->hwctx;

		frames_hwctx->BindFlags = D3D11_BIND_DECODER;
	}
#endif

	if ((err = av_hwframe_ctx_init(hw_frames_ref)) < 0) {
		fprintf(stderr, "Failed to initialize frame context.");
		av_buffer_unref(&hw_frames_ref);
		return err;
	}
	ctx->hw_frames_ctx = av_buffer_ref(hw_frames_ref);
	if (!ctx->hw_frames_ctx)
		err = AVERROR(ENOMEM);

	av_buffer_unref(&hw_frames_ref);
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

	ret = avformat_open_input(&fc, filename, NULL, NULL);//���ļ�
	if (ret < 0) {
		printf("error %x in avformat_open_input\n", ret);
		exit(EXIT_FAILURE);
	}

	ret = avformat_find_stream_info(fc, NULL);//ȡ������Ϣ
	if (ret < 0) {
		printf("error %x in avformat_find_stream_info\n", ret);
		exit(EXIT_FAILURE);
	}

	//������Ƶ������Ƶ��
	av_dump_format(fc, 0, filename, 0);//�г������ļ����������Ϣ

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

	codec = avcodec_find_decoder(fc->streams[videoindex]->codecpar->codec_id);	//��������Ϣ�ҵ�������
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

	type = av_hwdevice_find_type_by_name("dxva2");
	//type = av_hwdevice_find_type_by_name("d3d11va");
	
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

#if 0
	//  auto create AVHWFramesContext for decode
	/* set hw_frames_ctx for encoder's AVCodecContext */
	if ((ret = set_hwframe_ctx(codecctx, hw_device_ctx, hw_pix_fmt, AV_PIX_FMT_NV12)) < 0) {
		fprintf(stderr, "Failed to set hw frame context.\n");
		exit(EXIT_FAILURE);
	}
#endif

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