#ifndef _IDCARD_MAIN_DLG_H_
#define _IDCARD_MAIN_DLG_H_
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <afxdialogex.h>
#include "idcard_manager.h"

class IdCardMainDlg 
	: public CDialogEx
	, public IdCardReadCallback
{
public:
	IdCardMainDlg(CWnd* pParent = nullptr);
	virtual ~IdCardMainDlg();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IDCARD_CONSOLE_DIALOG };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
protected:
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnClickedCollection();
	afx_msg LRESULT OnRefreshIdCard(WPARAM wParam, LPARAM lParam);
private:
	void OnReadCompleted(IdCardData* data);
	void OnReadError(int error);
	void DoCollecting();
	void FormatDate(std::string& format_date, const char* date, int length);
	//////////////////////////////////////////////////////////////////////////
	void DoTest();
private:
	HICON	_icon;
	HANDLE	_collection_signal;
	boost::shared_ptr<boost::thread> _collection_thread;
	HANDLE _collection_thread_handle;
	volatile bool _exiting;
	unsigned int _timer_second;
	IdCardData* _idcard_data;
	CStatic _ctl_people_name;
	CStatic _ctl_people_sex;
	CStatic _ctl_people_nation;
	CStatic _ctl_people_birth_year;
	CStatic _ctl_people_birth_month;
	CStatic _ctl_people_birth_day;
	CStatic _ctl_people_id_code;
	CStatic _ctl_people_address;
	CStatic _ctl_sign_department;
	CStatic _ctl_valid_date;
	CStatic _ctl_status;
private:
	DECLARE_MESSAGE_MAP()
};

#endif // _IDCARD_MAIN_DLG_H_