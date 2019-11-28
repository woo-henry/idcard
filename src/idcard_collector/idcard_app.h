#ifndef _IDCARD_APP_H_
#define _IDCARD_APP_H_
//////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif
//////////////////////////////////////////////////////////////////////////
#include "resource.h"		// main symbols

class IdCardApp : public CWinApp
{
public:
	IdCardApp();
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
private:
	DECLARE_MESSAGE_MAP()
};

extern IdCardApp theApp;

#endif // _IDCARD_APP_H_
