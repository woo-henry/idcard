#include "pch.h"
#include "framework.h"
#include "idcard_app.h"
#include "idcard_main_dlg.h"
#include "idcard_config.h"
#include "idcard_manager.h"
//////////////////////////////////////////////////////////////////////////
IdCardApp theApp;

BEGIN_MESSAGE_MAP(IdCardApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
IdCardApp::IdCardApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

BOOL IdCardApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDS_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	if (!BaseLogInitialize())
	{
		AfxMessageBox(IDS_LOG_INIT_FAILED);
		return FALSE;
	}

	BaseLogInfo("==============================================================================");
	BaseLogInfo("                             IdCardCollector/1.0.0                            ");
	BaseLogInfo("==============================================================================");
	BaseLogInfo("IdCardCollector is ready to startup ...");

	boost::shared_ptr<IdCardConfig> idcard_config = IdCardConfig::GetInstance();
	if (idcard_config == nullptr)
	{
		AfxMessageBox(IDS_CONFIG_INSTANCE_INIT_FAILED);
		return FALSE;
	}

	int result = idcard_config->Load(nullptr);
	if (result != IDCARD_ERROR_SUCCESS)
	{
		AfxMessageBox(IDS_CONFIG_INSTANCE_INIT_FAILED);
		return FALSE;
	}

	boost::shared_ptr<IdCardManager> idcard_manager = IdCardManager::GetInstance();
	if (idcard_manager == nullptr)
	{
		AfxMessageBox(IDS_MANAGER_INSTANCE_CREATE_FAILED);
		return FALSE;
	}

	result = idcard_manager->Init(idcard_config);
	if (result != IDCARD_ERROR_SUCCESS)
	{
		AfxMessageBox(IDS_MANAGER_INSTANCE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("IdCardCollection"));

	IdCardMainDlg dlg;
	m_pMainWnd = &dlg;
	BaseLogDebug("IdCardCollector show dialog");
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	BaseLogInfo("IdCardCollector exit : %d", nResponse);

	return FALSE;
}

int IdCardApp::ExitInstance()
{
	BaseLogShutdown();

	return CWinApp::ExitInstance();
}