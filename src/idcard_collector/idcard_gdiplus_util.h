#ifndef _IDCARD_GDIPLUS_UTIL_H_
#define _IDCARD_GDIPLUS_UTIL_H_
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

using namespace Gdiplus;

class Utility
{
public:
	static Bitmap* CreateBitmapFromMemory(const void *buf, size_t size);
	static bool Memory2Global(const void *buf, size_t size, HGLOBAL global);
};

#endif // _IDCARD_GDIPLUS_UTIL_H_
