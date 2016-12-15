#include "stdafx.h"
#include "mp3tagplayer.h"
#include "mp3tagplayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(Cmp3tagplayerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

Cmp3tagplayerApp::Cmp3tagplayerApp()
{
	m_gdiplusToken = NULL;
}

Cmp3tagplayerApp theApp;

BOOL Cmp3tagplayerApp::InitInstance()
{	
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, "mp3tagplayerModuleMiniInstance_");
	if (ERROR_ALREADY_EXISTS == ::GetLastError())
		return FALSE;
	
	CoInitialize(NULL);

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	SetClassName();

	Cmp3tagplayerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	return FALSE;
}

int Cmp3tagplayerApp::ExitInstance()
{
	if(m_gdiplusToken != NULL)
	{
		GdiplusShutdown(m_gdiplusToken);
		m_gdiplusToken = NULL;
	}

	CoUninitialize();

	return CWinAppEx::ExitInstance();
}

void Cmp3tagplayerApp::SetClassName()
{
	WNDCLASS wc;
	GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);;

	wc.lpszClassName = _T("mp3tagplayerModuleMini");
	AfxRegisterClass(&wc);
}