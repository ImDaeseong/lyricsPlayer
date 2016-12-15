#include "stdafx.h"
#include "mp3tagplayer.h"
#include "mp3tagplayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Cmp3tagplayerDlg::Cmp3tagplayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cmp3tagplayerDlg::IDD, pParent)
{
	m_pBackSkin	= NULL;	

	m_nScreenSizeX = 0;
	m_nScreenSizeY = 0;

	m_bMouseOn = FALSE;

	m_strCurrentMp3Path = "";
	m_strCurrentLyricsPath = "";
	m_nCurPosition = 0;

	m_bPlayPause = FALSE;

	m_bReadLyricsComplete = FALSE;
	m_strCurrentTime = "";
	m_strPlayTime = "";
	m_strLyrics1 = "";
	m_strLyrics2 = ""; 
	m_strTitle = "";
	m_nReadIndex = 0;

	m_nIndex = 0;
}

void Cmp3tagplayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_btnPlay);
	DDX_Control(pDX, IDC_BUTTON_ADDFILE, m_btnAddFile);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_LEFT, m_btnLeft);
	DDX_Control(pDX, IDC_BUTTON_RIGHT, m_btnRight);
}

BEGIN_MESSAGE_MAP(Cmp3tagplayerDlg, CDialog)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_ACTIVATE()
	ON_WM_DROPFILES()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &Cmp3tagplayerDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_ADDFILE, &Cmp3tagplayerDlg::OnBnClickedButtonAddfile)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &Cmp3tagplayerDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_LEFT, &Cmp3tagplayerDlg::OnBnClickedButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, &Cmp3tagplayerDlg::OnBnClickedButtonRight)
END_MESSAGE_MAP()

HWND Cmp3tagplayerDlg::GetCurPosHandle(POINT point)
{
	return ::WindowFromPoint(point);
}

void Cmp3tagplayerDlg::CheckCurPos()
{
	POINT point;
	::GetCursorPos(&point);

	HWND hCur = GetCurPosHandle(point);
	if (hCur == GetSafeHwnd())
	{
		if(m_bMouseOn == FALSE)	m_bMouseOn = TRUE;
        m_btnPlay.SetVisibleButton(FALSE); 
		m_btnAddFile.SetVisibleButton(FALSE); 
		m_btnClose.SetVisibleButton(FALSE); 
		m_btnLeft.SetVisibleButton(FALSE); 
	    m_btnRight.SetVisibleButton(FALSE); 
	}
	else
	{
		HWND hOwner;	
		do
		{
			hOwner = ::GetParent(hCur);

			if (hOwner == NULL)
			{
				m_bMouseOn = FALSE;
				m_btnPlay.SetVisibleButton(TRUE); 
				m_btnAddFile.SetVisibleButton(TRUE); 
				m_btnClose.SetVisibleButton(TRUE); 
				m_btnLeft.SetVisibleButton(TRUE); 
	            m_btnRight.SetVisibleButton(TRUE); 
				return;
			}			

			if (hOwner == GetSafeHwnd())
			{
				if(m_bMouseOn == FALSE)
				{
					m_bMouseOn = TRUE;	
					m_btnPlay.SetVisibleButton(FALSE); 
					m_btnAddFile.SetVisibleButton(FALSE); 
					m_btnClose.SetVisibleButton(FALSE); 
					m_btnLeft.SetVisibleButton(FALSE); 
	                m_btnRight.SetVisibleButton(FALSE); 
				}
				return;
			}

			hCur = hOwner;
		}while (hCur);
	}
}

void Cmp3tagplayerDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect Rect;
	GetClientRect(&Rect);

	if (!m_bMouseOn && Rect.PtInRect(point))
	{
		m_bMouseOn = TRUE;	
		m_btnPlay.SetVisibleButton(FALSE); 
		m_btnAddFile.SetVisibleButton(FALSE); 
		m_btnClose.SetVisibleButton(FALSE); 
		m_btnLeft.SetVisibleButton(FALSE); 
	    m_btnRight.SetVisibleButton(FALSE); 
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

void Cmp3tagplayerDlg::OnDestroy() 
{
	InitLyrics();

	m_mp3Info.clear();

	KillTimer(1);
	KillTimer(2);
	
	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	

	CDialog::OnDestroy();
}

void Cmp3tagplayerDlg::OnTimer(UINT nIDEvent) 
{	
	if(nIDEvent == 1)
	{
		int nCurTime = int( m_Player.GetPosition() / 1000 );
		int nLength = int( m_Player.GetRunningTime() / 1000 );
			
		if( nLength > 0 )
		{
			int nCur2 = nCurTime%60;
			CString szCur2;
			if( nCur2 < 10 )
				szCur2.Format("0%d", nCur2);
			else
				szCur2.Format("%d", nCur2);
				
			int nLen2 = nLength%60;
			CString szLen2;
			if( nLen2 < 10 )
				szLen2.Format("0%d", nLen2);
			else
				szLen2.Format("%d", nLen2);
			
			m_strCurrentTime.Format("%2d:%s", nCurTime/60, szCur2);
			m_strPlayTime.Format("%2d:%s / %2d:%s", nCurTime/60, szCur2, nLength/60, szLen2);
			m_nCurPosition = nCurTime*100/nLength;	
						
			if(m_bReadLyricsComplete)
			{
				if(m_LyricsInfo.size() == m_nReadIndex)	m_nReadIndex = 0;

				if(m_LyricsInfo.size() > 0)
				{
					SearchLyricsInfo();
				}
			}
			else
				Invalidate();
				
			if( m_nCurPosition == 100 ) 
			{
				m_nCurPosition = 0;
				KillTimer(1);
				NextPlayMusic();
			}
		}
	}	
	else if(nIDEvent == 2)
	{
		if (m_bMouseOn)	CheckCurPos();
	}
	
	CDialog::OnTimer(nIDEvent);
}

void Cmp3tagplayerDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{ 
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	SetTopActivate(); 
}

BOOL Cmp3tagplayerDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void Cmp3tagplayerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (nType == SIZE_MAXIMIZED || nType == SIZE_RESTORED)
		Invalidate();
}

LRESULT Cmp3tagplayerDlg::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	CDC* pDC = GetDC();
	if(pDC != NULL)
	{
		DrawSkin(pDC);
		ReleaseDC(pDC);
	}
	
	int nScreenSizeX = GetSystemMetrics(SM_CXSCREEN);
	int nScreenSizeY = GetSystemMetrics(SM_CYSCREEN);
	if( nScreenSizeX != m_nScreenSizeX || nScreenSizeY != m_nScreenSizeY )
	{
		m_nScreenSizeX = nScreenSizeX;
		m_nScreenSizeY = nScreenSizeY;
		MoveLocationDialog();
	}
	return 0;
}

void Cmp3tagplayerDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void Cmp3tagplayerDlg::InitButtons()
{	
	CRect rClient;
	GetClientRect(&rClient);
	
	m_btnPlay.SetImage(MAKEINTRESOURCE(IDB_PNG_PLAY), "PNG", AfxGetApp()->m_hInstance, ((rClient.right - rClient.left)/2) - 7, 5, 1);
	m_btnAddFile.SetImage(MAKEINTRESOURCE(IDB_PNG_ADDFILE), "PNG", AfxGetApp()->m_hInstance,  (rClient.right - 55), (rClient.bottom - 50), 1);
	m_btnClose.SetImage(MAKEINTRESOURCE(IDB_PNG_CLOSE), "PNG", AfxGetApp()->m_hInstance, (rClient.right - 25), 5, 1);

	m_btnLeft.SetImage(MAKEINTRESOURCE(IDB_PNG_LEFT), "PNG", AfxGetApp()->m_hInstance, ((rClient.right - rClient.left)/2) -30, 5, 1);
	m_btnRight.SetImage(MAKEINTRESOURCE(IDB_PNG_RIGHT), "PNG", AfxGetApp()->m_hInstance, ((rClient.right - rClient.left)/2) + 17, 5, 1);

	m_btnPlay.SetVisibleButton(TRUE); 
	m_btnAddFile.SetVisibleButton(TRUE); 
	m_btnClose.SetVisibleButton(TRUE); 
	m_btnLeft.SetVisibleButton(TRUE); 
	m_btnRight.SetVisibleButton(TRUE); 
}

BOOL Cmp3tagplayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	DragAcceptFiles(TRUE);

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
		
	if(!LoadSkin())
	{
		CDialog::OnCancel();
		return FALSE;
	}

	CDC* pDC = GetDC();
	DrawSkin(pDC);
	ReleaseDC(pDC);	
		
	SetTimer(2, 100, NULL);
	
	return TRUE;  
}

CString Cmp3tagplayerDlg::GetFileExtName(CString strFilename)
{
	int nPos = strFilename.ReverseFind('.');
	if(nPos > 0)
		return strFilename.Mid(nPos + 1);
	return strFilename;
}

CString Cmp3tagplayerDlg::GetFileName(CString strFilename)
{
	int nPos = strFilename.ReverseFind('.');
	if(nPos > 0)
		return strFilename.Left(nPos);
	
	return strFilename;
}

CString Cmp3tagplayerDlg::GetModulePath()
{
	char cTemp[MAX_PATH];
	char *spzRealDirEnd;
	CString strModulePath;

	GetModuleFileName(NULL, cTemp, MAX_PATH);  
	spzRealDirEnd = strrchr(cTemp, '\\');
	*spzRealDirEnd = '\0';
	strModulePath = (CString)cTemp;
	return strModulePath;
}

BOOL Cmp3tagplayerDlg::LoadSkin()
{
	m_pBackSkin = new CGdiPlusBitmapResource;
	if(!m_pBackSkin->Load(IDB_PNG_BG, "PNG", AfxGetApp()->m_hInstance))
	{
		delete m_pBackSkin;
		m_pBackSkin = NULL;
		return FALSE;
	}
	
	MoveLocationDialog();

	InitButtons();

	return TRUE;
}

void Cmp3tagplayerDlg::DrawSkin(CDC* pDC)
{
	CDC memDC;
	CBitmap bmp;
	CBitmap* bmp_old;

	CRect rect;
	GetWindowRect(&rect);

	int cx = rect.Width();
	int cy = rect.Height();

	memDC.CreateCompatibleDC(pDC);

	BITMAPINFO bmInfo;
	memset(&bmInfo, 0x00, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = cx;
	bmInfo.bmiHeader.biHeight = cy;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biSizeImage = 0;
	bmInfo.bmiHeader.biClrUsed = 0;
	bmInfo.bmiHeader.biClrImportant = 0;

	LPBYTE pbmpBits = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(pDC->GetSafeHdc(), (LPBITMAPINFO)&bmInfo, DIB_RGB_COLORS, (void **)&pbmpBits, NULL, 0);
	if(hBitmap == NULL)
		bmp.CreateCompatibleBitmap(pDC, cx, cy);
	else
		bmp.Attach(hBitmap);

	bmp_old = memDC.SelectObject(&bmp);
	Graphics gps(memDC.GetSafeHdc());

	gps.DrawImage(m_pBackSkin->m_pBitmap,
		Rect(0, 0, m_pBackSkin->m_pBitmap->GetWidth(), m_pBackSkin->m_pBitmap->GetHeight()), 
		0, 0, m_pBackSkin->m_pBitmap->GetWidth(), m_pBackSkin->m_pBitmap->GetHeight(), UnitPixel);
	
	if(m_btnPlay.IsAvailableDraw())
		m_btnPlay.OnDrawLayerdWindow(gps);	

	if(m_btnAddFile.IsAvailableDraw())
		m_btnAddFile.OnDrawLayerdWindow(gps);
	
	if(m_btnClose.IsAvailableDraw())
		m_btnClose.OnDrawLayerdWindow(gps);

	if(m_btnLeft.IsAvailableDraw())
		m_btnLeft.OnDrawLayerdWindow(gps);

	if(m_btnRight.IsAvailableDraw())
		m_btnRight.OnDrawLayerdWindow(gps);

	DisplayLyrics(gps);

	CPoint ptWindowScreenPosition(rect.TopLeft());
	CSize szWindow(cx, cy);

	BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	CPoint ptSrc(0,0);

	HDC hdc = gps.GetHDC();
	BOOL bRet= ::UpdateLayeredWindow(GetSafeHwnd(),pDC->GetSafeHdc(),&ptWindowScreenPosition,&szWindow,hdc,&ptSrc,0,&blendPixelFunction,ULW_ALPHA);
	gps.ReleaseHDC(hdc);
	memDC.SelectObject(bmp_old);
	bmp.DeleteObject();
	memDC.DeleteDC();
}

void Cmp3tagplayerDlg::MoveLocationDialog()
{	
	m_nScreenSizeX = GetSystemMetrics(SM_CXSCREEN);
	m_nScreenSizeY = GetSystemMetrics(SM_CYSCREEN);
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();
	int nOffSet = 50;

	RECT rcWorkArea;
	SystemParametersInfo (SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow((rcWorkArea.left+1), (rcWorkArea.bottom - cy + nOffSet), (rcWorkArea.right - 2), (cy - nOffSet));
	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}

void Cmp3tagplayerDlg::SetTopActivate()
{
	::SetWindowPos(this->GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	::SetForegroundWindow(this->GetSafeHwnd());
	::SetActiveWindow(this->GetSafeHwnd());
}

void Cmp3tagplayerDlg::OnDropFiles(HDROP hDropInfo)
{	
	CStringArray aryMp3lst, arylyslst; 

	TCHAR szFilePathName[MAX_PATH];
	UINT uFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);  

	for (UINT count = 0; count < uFileCount; count++)
	{	
		DragQueryFile(hDropInfo, count, szFilePathName, _MAX_PATH); 

		if( lstrcmpi(GetFileExtName(szFilePathName), "mp3") == 0 )
			aryMp3lst.Add(szFilePathName);	
		else if( lstrcmpi(GetFileExtName(szFilePathName), "lys") == 0)
			arylyslst.Add(szFilePathName);	
	}
	
	mp3Info vInfo;
	CString strMp3Name = "";
	CString strLyricsName = "";

	for(int i=0; i < aryMp3lst.GetCount(); i++)
	{		
		strMp3Name = aryMp3lst.GetAt(i);		

		for(int k=0; k< arylyslst.GetCount(); k++)
		{	
			strLyricsName = arylyslst.GetAt(k);
			if( GetFileName(strMp3Name) == GetFileName(strLyricsName))
				break;
			else 
				strLyricsName = "";
		}
		vInfo.mp3 = strMp3Name;
		vInfo.lyric = strLyricsName;
       	m_mp3Info.push_back(vInfo);
	}
	aryMp3lst.RemoveAll();
	arylyslst.RemoveAll();	

	NextPlayMusic(TRUE);
	
	CDialog::OnDropFiles(hDropInfo);
}

void Cmp3tagplayerDlg::Stop()
{	
	m_Player.Stop();

	KillTimer(1);
}

void Cmp3tagplayerDlg::Pause() 
{
	m_Player.Pause();
}

void Cmp3tagplayerDlg::Play()
{	
	m_Player.Open(m_strCurrentMp3Path);

	KillTimer(1);
	SetTimer(1, 1000, NULL);

	m_Player.Play();

	m_Player.SetVolume(100);
}

void Cmp3tagplayerDlg::InitLyrics()
{
	m_LyricsInfo.clear();
	
	m_bReadLyricsComplete = FALSE;

	m_strLyrics1 = "";
	m_strLyrics2 = "";
}

BOOL Cmp3tagplayerDlg::CheckDisplayTime(CString strCurTime, CString strSavedTime)
{
	int nCount = strCurTime.Find(_T(":"));
	int nLength = strCurTime.GetLength();
	CString strMinute1 = strCurTime.Left(nCount);
	CString strSecond1 = strCurTime.Right(nLength - nCount - 1);
		
	nCount = strSavedTime.Find(_T(":"));
	nLength = strSavedTime.GetLength();
	CString strMinute2 = strSavedTime.Left(nCount);
	CString strSecond2 = strSavedTime.Right(nLength - nCount - 1);

	int nCurTotalTime = (atoi(strMinute1) * 60) + atoi(strSecond1);
	int nSavedTotalTime = (atoi(strMinute2) * 60) + atoi(strSecond2);
	
	if(nCurTotalTime == nSavedTotalTime)
		return TRUE;
	return FALSE;
}

void Cmp3tagplayerDlg::SearchLyricsInfo()
{
	std::vector<LyricsInfo>::iterator it;
	for (it = m_LyricsInfo.begin();	it != m_LyricsInfo.end(); it++)
	{
		BOOL bDisplay = CheckDisplayTime(m_strCurrentTime, it->Time);
		if( bDisplay)
		{
			m_strLyrics1 = it->Lyrics1;
			m_strLyrics2 = it->Lyrics2;
			Invalidate();
			break;
		}
	}
}

void Cmp3tagplayerDlg::ReadLyrics()
{
	InitLyrics();

	CStdioFile file;
	file.Open(m_strCurrentLyricsPath, CStdioFile::modeRead);

	CString strBuffer;
	LyricsInfo lycInfo;
	while(file.ReadString(strBuffer)) 
	{
		lycInfo.Time = GetElementData(0, strBuffer);
		lycInfo.Lyrics1 = GetElementData(1, strBuffer);
		lycInfo.Lyrics2 = GetElementData(2, strBuffer);
		m_LyricsInfo.push_back(lycInfo);
	}
	file.Close();

	m_bReadLyricsComplete = TRUE;
}

CString Cmp3tagplayerDlg::GetElementData(int nOpt, CString strData)
{
	CString strTime;
	CString strLyrics1;
	CString strLyrics2;
   	
	int nIndex = strData.Find("|");
	if( nIndex == -1 )
		return CString("");

	strTime = strData.Mid(0, nIndex);
	strData = strData.Right(strData.GetLength() - nIndex - 1);


	nIndex = strData.Find("|");
	if(nIndex == -1)
		return CString("");

	strLyrics1 = strData.Mid(0, nIndex);
	strLyrics2 = strData.Right(strData.GetLength() - nIndex - 1);
	
	if(nOpt == 0)
		return strTime;	
	else if(nOpt == 1)
		return strLyrics1;
	else if(nOpt == 2)
		return strLyrics2;

	return CString("");
}

void Cmp3tagplayerDlg::DisplayLyrics(Graphics& gps)
{	
USES_CONVERSION;

	Gdiplus::Font font1(L"µ¸¿ò", 50, FontStyleBold, UnitPixel);
	SolidBrush brush1(Color(234, 137, 6));

	Gdiplus::Font font2(L"µ¸¿ò", 50, FontStyleBold, UnitPixel);
	SolidBrush brush2(Color(64, 193, 255));

	Gdiplus::Font font3(L"±¼¸²", 11, FontStyleRegular, UnitPixel);
	SolidBrush brush3(Color(255, 255, 255));

	Gdiplus::Font font4(L"±¼¸²", 12, FontStyleRegular, UnitPixel);
	SolidBrush brush4(Color(255, 255, 255));

	StringFormat stringFormat(StringFormatFlagsDisplayFormatControl);
	stringFormat.SetAlignment(StringAlignmentCenter);	
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	
	CRect rect;
    GetClientRect(&rect);
	
	if(m_strLyrics1 != "" && m_strLyrics2 != "" )
	{
		RectF rectF1(REAL(rect.left + 5), REAL(rect.top + 30), REAL(rect.Width()), REAL(50));
		gps.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
		gps.DrawString(A2W(m_strLyrics1), -1, &font1, rectF1, &stringFormat, &brush1);

		RectF rectF2(REAL(rect.left + 5), REAL(rect.top + 80), REAL(rect.Width()), REAL(50));
		gps.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
		gps.DrawString(A2W(m_strLyrics2), -1, &font2, rectF2, &stringFormat, &brush2);
	}
	else
	{
		RectF rectF1(REAL(rect.left + 5), REAL(rect.top + 60), REAL(rect.Width()), REAL(50));
		gps.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
		gps.DrawString(A2W(m_strLyrics1), -1, &font1, rectF1, &stringFormat, &brush1);	
	}

	if(m_strPlayTime != "")
	{
		RectF rectF1(REAL(rect.left + 1), REAL(rect.bottom - 30), REAL(rect.Width()), REAL(30));
		gps.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
		gps.DrawString(A2W(m_strPlayTime), -1, &font3, rectF1, &stringFormat, &brush3);	
	}

	if(m_strTitle != "")
	{
		StringFormat stringFormat(StringFormatFlagsDisplayFormatControl);
		stringFormat.SetAlignment(StringAlignmentNear);	
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		RectF rectF1(REAL(rect.left + 1), REAL(rect.top), REAL(rect.Width()), REAL(30));
		gps.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
		gps.DrawString(A2W(m_strTitle), -1, &font4, rectF1, &stringFormat, &brush4);		
	}
}

void Cmp3tagplayerDlg::OnBnClickedButtonPlay()
{
	if(m_bPlayPause == FALSE)
	{
		m_bPlayPause = TRUE;
		m_Player.Pause();
	}
	else
	{
		m_bPlayPause = FALSE;
		m_Player.Play();	
	}
}

void Cmp3tagplayerDlg::OnBnClickedButtonAddfile()
{	
	CString strSelectPath;
	LPMALLOC pMalloc;

    if (::SHGetMalloc(&pMalloc) == NOERROR)
    {
        BROWSEINFO bi;
        char pszBuffer[MAX_PATH];
        LPITEMIDLIST pidl;
		
        bi.hwndOwner = GetSafeHwnd();
        bi.pidlRoot = NULL;
        bi.pszDisplayName = pszBuffer;
        bi.lpszTitle = _T("Select Mp3");
        bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
        bi.lpfn = NULL;
        bi.lParam = 0;

        if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
        {
            if (::SHGetPathFromIDList(pidl, pszBuffer))
            { 
				strSelectPath = pszBuffer;
            }

            pMalloc->Free(pidl);
        }
        pMalloc->Release();
    }

	if(strSelectPath == "")
		return;

	CStringArray aryMp3lst, arylyslst; 
	SetFileInfo(strSelectPath, aryMp3lst, arylyslst);

	mp3Info vInfo;
	CString strMp3Name = "";
	CString strLyricsName = "";

	for(int i=0; i < aryMp3lst.GetCount(); i++)
	{		
		strMp3Name = aryMp3lst.GetAt(i);		

		for(int k=0; k< arylyslst.GetCount(); k++)
		{	
			strLyricsName = arylyslst.GetAt(k);
			if( GetFileName(strMp3Name) == GetFileName(strLyricsName))
				break;
			else 
				strLyricsName = "";
		}
		vInfo.mp3 = strMp3Name;
		vInfo.lyric = strLyricsName;
       	m_mp3Info.push_back(vInfo);
	}
	aryMp3lst.RemoveAll();
	arylyslst.RemoveAll();	

	NextPlayMusic(TRUE);
}

void Cmp3tagplayerDlg::SetFileInfo(CString strPath, CStringArray &aryMp3lst, CStringArray &arylyslst)
{	
	CFileFind finder;
	CString strWildcard = strPath;
	strWildcard += _T("\\*.*");

	CString strFullPath;
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			strFullPath = finder.GetFilePath();
			SetFileInfo(strFullPath, aryMp3lst, arylyslst);
		}
		else
		{
			CString strFullPath = finder.GetFilePath();
			CString strFileName = finder.GetFileName();
			CString strExtention = strFileName.Mid(strFileName.ReverseFind('.')+1); 

			if(lstrcmpi(strExtention, "mp3") == 0)
				aryMp3lst.Add(strFullPath);
			else if(lstrcmpi(strExtention, "lys") == 0 )
				arylyslst.Add(strFullPath);
		}
	}
	finder.Close();
}

void Cmp3tagplayerDlg::PrePlayMusic()
{
	if(m_mp3Info.size() == 0) return;

	m_nIndex --;

	if(m_nIndex < 0)
		m_nIndex = m_mp3Info.size() - 1;
		
	m_strCurrentMp3Path = m_mp3Info[m_nIndex].mp3;
	m_strCurrentLyricsPath = m_mp3Info[m_nIndex].lyric;
	m_strTitle = ReadMp3Info();
	
	Stop();
	Play();

	if(m_strCurrentLyricsPath != "") 
		ReadLyrics();
	else
		InitLyrics();
}

void Cmp3tagplayerDlg::NextPlayMusic(BOOL bFirstPlay)
{	
	if(m_mp3Info.size() == 0) return;

	if(bFirstPlay)
	{		
		m_nIndex = 0;
		m_strCurrentMp3Path = m_mp3Info[m_nIndex].mp3;
		m_strCurrentLyricsPath = m_mp3Info[m_nIndex].lyric;
		m_strTitle = ReadMp3Info();

		Stop();
		Play();

		if(m_strCurrentLyricsPath != "") 
			ReadLyrics();
		else
			InitLyrics();
	}
	else
	{
		m_nIndex ++;

		if(m_nIndex > (m_mp3Info.size()-1) )
			m_nIndex = 0;
		
		m_strCurrentMp3Path = m_mp3Info[m_nIndex].mp3;
		m_strCurrentLyricsPath = m_mp3Info[m_nIndex].lyric;
		m_strTitle = ReadMp3Info();
		
		Stop();
		Play();

		if(m_strCurrentLyricsPath != "") 
			ReadLyrics();
		else
			InitLyrics();
	}
}

CString Cmp3tagplayerDlg::ReadMp3Info()
{
	CString strReturn = "";

	CFile mp3file(m_strCurrentMp3Path, CFile::modeRead | CFile::typeBinary| CFile::shareDenyNone);
	mp3file.Seek(-128, CFile::end);

	char tempbuff[2];
	char tagbuff[124];
	char tagBuffer[4];
	mp3file.Read(tagBuffer,3);

	char tag[4] = "TAG";
	
	if(strncmp(tag, tagBuffer,3) == 0)
	{
		memset(tagbuff,0, 124);	
		for (int i=0;i<=122; i++)
		{
			memset(tempbuff,0,2);
			mp3file.Seek(-125+i,CFile::end);
			mp3file.Read(tempbuff,1);

			if(tempbuff[0] != '\0') tagbuff[i] = tempbuff[0];
			else tagbuff[i] = 0x20;
		}
					
		mp3file.Seek(-2,CFile::end);
		unsigned char tempbuff2[3];
		memset(tempbuff2,0,3);
		mp3file.Read(tempbuff2,2);
		int track = tempbuff2[0];
		int genre = tempbuff2[1];
		
		CString tagstr;
		tagstr = tagbuff;
		
		CString szTitle = tagstr.Left(30);
		CString szArtist = tagstr.Mid(30, 30);
				
		szTitle.TrimRight();
		szArtist.TrimRight();
		strReturn.Format("%s - %s", szArtist, szTitle);
	}
	mp3file.Close();
	return strReturn;	
}

void Cmp3tagplayerDlg::OnBnClickedButtonClose()
{
	CDialog::OnOK();
}

void Cmp3tagplayerDlg::OnBnClickedButtonLeft()
{
	PrePlayMusic();
}

void Cmp3tagplayerDlg::OnBnClickedButtonRight()
{
	NextPlayMusic();
}
