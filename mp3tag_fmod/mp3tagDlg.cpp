#include "stdafx.h"
#include "mp3tag.h"
#include "mp3tagDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Cmp3tagDlg::Cmp3tagDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cmp3tagDlg::IDD, pParent)
{
	m_bClickPlay = FALSE;
	m_strCurrentPath = "";
	m_nlength = 0;
	m_strTimer = "";
	nCurPosition = 0;	
	m_nIndex = 0;
	m_nSize = 0;
	m_strSeconds = "";
	m_bEditingLyrics = FALSE;
}

void Cmp3tagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_sliderRunning);
	DDX_Control(pDX, IDC_EDIT_LYRICS, m_edtLyrics);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(Cmp3tagDlg, CDialog)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SELECTFILE, &Cmp3tagDlg::OnBnClickedButtonSelectfile)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &Cmp3tagDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_MAKE, &Cmp3tagDlg::OnBnClickedButtonMake)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &Cmp3tagDlg::OnBnClickedButtonWrite)	
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()	
	ON_BN_CLICKED(IDC_BUTTON_SKIPLEFT, &Cmp3tagDlg::OnBnClickedButtonSkipleft)
	ON_BN_CLICKED(IDC_BUTTON_SKIPRIGHT, &Cmp3tagDlg::OnBnClickedButtonSkipright)
END_MESSAGE_MAP()

CString Cmp3tagDlg::ReadMp3(const char* file)
{
	CString strReturn = "";

	CFile mp3file(file, CFile::modeRead | CFile::typeBinary| CFile::shareDenyNone);
	mp3file.Seek(-128,CFile::end);

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
		CString szAlbum = tagstr.Mid(60, 30);
		CString szYear = tagstr.Mid(90, 4);
		CString szComment = tagstr.Mid(94,28);		
						
		//Trim white spaces
		szTitle.TrimRight();
		szArtist.TrimRight();
		szAlbum.TrimRight();
		szYear.TrimRight();
		szComment.TrimRight();
		
		strReturn.Format("%s - %s", szArtist, szTitle);
	}
	mp3file.Close();

	return strReturn;	
}

void Cmp3tagDlg::ReadStringList(CString strInput)
{
	mLineArray.clear();
	
	const char* pSep = "\r\n";
	char* chline = strtok((char*)(LPCTSTR)strInput, pSep);
	while(chline != NULL)
	{
		mLineArray.push_back(chline);	
		chline = strtok(NULL, pSep);
	}
}

CString Cmp3tagDlg::GetElementData(int nOpt, CString strData)
{
	CString strLyrics1;
	CString strLyrics2;
   	
	int nIndex = strData.Find("|");
	if( nIndex == -1 )
		return CString("");

	strLyrics1 = strData.Mid(0, nIndex);
	strLyrics2 = strData.Right(strData.GetLength() - nIndex - 1);
	
	if(nOpt == 0)
		return strLyrics1;	
	else if(nOpt == 1)
		return strLyrics2;

	return CString("");
}

CString Cmp3tagDlg::GetModulePath()
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

CString Cmp3tagDlg::GetFileExtName(CString strFilename)
{
	int nPos = strFilename.ReverseFind('.');
	if(nPos > 0)
		return strFilename.Mid(nPos + 1);
	return strFilename;
}

CString Cmp3tagDlg::GetFileFullName(CString strFilename)
{
	int nPos = strFilename.ReverseFind('\\');
	if(nPos > 0)
		return strFilename.Mid(nPos + 1);
	
	return strFilename;
}

CString Cmp3tagDlg::GetFileName(CString strFilename)
{
	int nPos = strFilename.ReverseFind('.');
	if(nPos > 0)
		return strFilename.Left(nPos);
	
	return strFilename;
}

CString Cmp3tagDlg::GetFilePath(CString strFilename)
{
	int nPos = strFilename.ReverseFind('\\');
	if(nPos > 0)
		return strFilename.Left(nPos);

	return strFilename;
}

BOOL Cmp3tagDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ListCtrl.DeleteAllItems(); 
	m_ListCtrl.InsertColumn(0, _T("시간"), LVCFMT_CENTER, 50, -1); 
    m_ListCtrl.InsertColumn(1, _T("가사"), LVCFMT_LEFT, 180, -1); 
	m_ListCtrl.InsertColumn(2, _T("번역"), LVCFMT_LEFT, 110, -1); 

	SetDlgItemText(IDC_STATIC_TITLE, "");
	SetDlgItemText(IDC_STATIC_TIME, "");
		
	return TRUE; 
}

void Cmp3tagDlg::OnPaint()
{
	CPaintDC dc(this);
}

void Cmp3tagDlg::OnBnClickedButtonSelectfile()
{	
	Stop();

	TCHAR szFile[MAX_PATH];
    ZeroMemory(szFile, MAX_PATH);
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize	= sizeof(OPENFILENAME);
    ofn.Flags		= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.hwndOwner	= this->m_hWnd;
    ofn.lpstrFilter	= _T("Mp3 Files Types(*.Mp3)\0*.Mp3;*.Mp3\0\0");
    ofn.lpstrTitle	= _T("Load Mp3 File");
    ofn.lpstrFile	= szFile;
    ofn.nMaxFile	= MAX_PATH;
	if (IDOK == GetOpenFileName(&ofn))
	{
		m_strCurrentPath = (LPCTSTR)szFile;
				
		CString szBuff = ReadMp3(szFile);
		SetDlgItemText(IDC_STATIC_TITLE, szBuff);
	}
	else
		m_strCurrentPath = "";
}

void Cmp3tagDlg::OnBnClickedButtonPlay()
{
	if( m_bClickPlay == FALSE)
	{
		Play();
	}
	else
	{
		Pause();	
	}
}

void Cmp3tagDlg::Stop()
{	
	nCurPosition = 0;	

	m_FPlayer.Stop();//m_Player.Stop();
		
	m_sliderRunning.SetPos(nCurPosition);

	m_bClickPlay = FALSE;

	SetDlgItemText(IDC_STATIC_TIME, "00:00 / 00:00");

	KillTimer(1);
}

void Cmp3tagDlg::Pause() 
{
	m_FPlayer.Pause();//m_Player.Pause();

	m_bClickPlay = FALSE;
}

void Cmp3tagDlg::Play()
{	
	m_FPlayer.Open(m_strCurrentPath);//m_Player.Open(m_strCurrentPath);

	KillTimer(1);
	SetTimer(1, 1000, NULL);

	m_FPlayer.Play();//m_Player.Play();

	m_nlength = m_FPlayer.GetPosition();//m_Player.GetPosition();

	m_sliderRunning.SetPos(nCurPosition);

	m_FPlayer.SetVolume(1);//m_Player.SetVolume(100);

	m_bClickPlay = TRUE;
}

void Cmp3tagDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		int nCurTime = int( m_FPlayer.GetPosition() / 1000 );//int( m_Player.GetPosition() / 1000 );
		int nLength = int( m_FPlayer.GetRunningTime() / 1000 );//int( m_Player.GetRunningTime() / 1000 );
			
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
				
			m_strTimer.Format("%2d:%s / %2d:%s", nCurTime/60, szCur2, nLength/60, szLen2);	
			m_strSeconds.Format("%2d:%s", nCurTime/60, szCur2);
			nCurPosition = nCurTime*100/nLength;	
				
			m_sliderRunning.SetPos(nCurPosition);
			SetDlgItemText(IDC_STATIC_TIME, m_strTimer);

			UpdateData(FALSE);
				
			if( nCurPosition == 100 ) 
			{
				nCurPosition = 0;
				KillTimer(1);
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void Cmp3tagDlg::OnDestroy()
{
	CDialog::OnDestroy();

	Stop();
}

void Cmp3tagDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ((CSliderCtrl*)pScrollBar == &m_sliderRunning)
	{
		if(nSBCode == SB_PAGELEFT || nSBCode == SB_PAGERIGHT || SB_THUMBTRACK == nSBCode)
		{
			m_FPlayer.SetPosition(m_sliderRunning.GetPos() * m_FPlayer.GetRunningTime() / 100);//m_Player.SetPosition(m_sliderRunning.GetPos() * m_Player.GetRunningTime() / 100);
		}
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void Cmp3tagDlg::OnBnClickedButtonMake()
{
	m_bEditingLyrics = FALSE;
	m_nIndex = 0;

	CString strLyrics;
	GetDlgItemText(IDC_EDIT_LYRICS, strLyrics);

	if(strLyrics != "")	ReadStringList(strLyrics);

	m_nSize = mLineArray.size();
	if(m_nSize == 0) return;
	
	Stop();
    Play();

	m_ListCtrl.DeleteAllItems(); 

	m_bEditingLyrics = TRUE;
}

void Cmp3tagDlg::OnBnClickedButtonWrite()
{
	if(!m_bEditingLyrics) return;

	if(m_nSize == m_nIndex)
	{
		CString strCreatePath;
		strCreatePath.Format("%s\\%s.lys", GetModulePath(), GetFileName(GetFileFullName(m_strCurrentPath)) );

		CStdioFile file;
		file.Open(strCreatePath, CStdioFile::modeCreate|CStdioFile::modeWrite);
		
		int item = m_ListCtrl.GetItemCount() - 1;
		while(item >=0)
		{
			CString strTime = m_ListCtrl.GetItemText(item, 0); 
			CString strLyrics1 = m_ListCtrl.GetItemText(item, 1); 
			CString strLyrics2 = m_ListCtrl.GetItemText(item, 2); 		
            CString strResult;
			strResult.Format("%s|%s|%s\r\n", strTime, strLyrics1, strLyrics2);	
			
			file.WriteString(strResult);

			item--;
		}

		file.Close();

		AfxMessageBox("complete");
	}
	else
	{	
		CString strLyrics1 = GetElementData(0, mLineArray[m_nIndex]);
		if(strLyrics1 == "") 
			strLyrics1 = mLineArray[m_nIndex];
		
		CString strLyrics2 = GetElementData(1, mLineArray[m_nIndex]);

		InsertListView(m_strSeconds, strLyrics1, strLyrics2);
		m_nIndex++;	
	}
}

void Cmp3tagDlg::InsertListView(CString strTime, CString strLyrics, CString strAlih)
{
	int nIndex = m_ListCtrl.GetItemCount();
	m_ListCtrl.InsertItem(nIndex, _T(strTime));
	m_ListCtrl.SetItemText(nIndex, 1, _T(strLyrics));
	m_ListCtrl.SetItemText(nIndex, 2, _T(strAlih));
}

void Cmp3tagDlg::OnBnClickedButtonSkipleft()
{
	Pause();
	m_FPlayer.SkipBack();//m_Player.SkipBack(1);
	Pause(); 
}

void Cmp3tagDlg::OnBnClickedButtonSkipright()
{
	Pause();
	m_FPlayer.SkipForward();//m_Player.SkipForward(1);
	Pause(); 
}
