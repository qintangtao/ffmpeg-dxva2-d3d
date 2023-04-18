// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <d3d9.h>
#include <dxva2api.h>

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/time.h"
}

#include "config.h"

#if CONFIG_DXVA2
#include "libavutil/hwcontext_dxva2.h"
#endif

#if CONFIG_D3D11VA
#include "libavutil/hwcontext_d3d11va.h"
#endif