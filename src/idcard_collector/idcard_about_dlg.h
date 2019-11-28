#ifndef _IDCARD_ABOUT_DLG_H_
#define _IDCARD_ABOUT_DLG_H_
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <afxdialogex.h>

class IdCardAboutDlg : public CDialogEx
{
public:
	IdCardAboutDlg();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
protected:
	DECLARE_MESSAGE_MAP()
};

#endif // _IDCARD_ABOUT_DLG_H_