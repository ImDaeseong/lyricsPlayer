#pragma once
#include "afxwin.h"

struct LyricsInfo
{
	CString Time;
	CString Lyrics1;
	CString Lyrics2;
};

struct mp3Info
{
	CString mp3;
	CString lyric;
};

class Cmp3tagplayerDlg : public CDialog
{
public:
	Cmp3tagplayerDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_MP3TAGPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:	
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnTimer(UINT_PTR nIDEvent);	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonAddfile();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonLeft();
	afx_msg void OnBnClickedButtonRight();

private:
	CString GetFileExtName(CString strFilename);
	CString GetFileName(CString strFilename);
	CString GetModulePath();
	void SetTopActivate();
	void MoveLocationDialog();
	void DrawSkin(CDC* pDC);
	BOOL LoadSkin();
	CGdiPlusBitmapResource* m_pBackSkin;
	
	int m_nScreenSizeX;
	int m_nScreenSizeY;


	void InitButtons();
	CSkinButton m_btnPlay;
	CSkinButton m_btnAddFile;
	CSkinButton m_btnClose;
    CSkinButton m_btnLeft;
	CSkinButton m_btnRight;

	BOOL m_bMouseOn;
	HWND GetCurPosHandle(POINT point);
	void CheckCurPos();

	
	CAudioPlayer m_Player;
	void Play();
	void Stop();
	void Pause();	

	CString m_strCurrentMp3Path;
	CString m_strCurrentLyricsPath;                                         
	int m_nCurPosition;
	BOOL m_bPlayPause;

	void PrePlayMusic();
	void NextPlayMusic(BOOL bFirstPlay = FALSE);
	void SetFileInfo(CString strPath, CStringArray &aryMp3lst, CStringArray &arylyslst);
	CString ReadMp3Info();
	
	void InitLyrics();
	BOOL CheckDisplayTime(CString strCurTime, CString strSavedTime);
	void ReadLyrics();
	CString GetElementData(int nOpt, CString strData);
	void DisplayLyrics(Graphics& gps);

	void SearchLyricsInfo();
	std::vector<LyricsInfo> m_LyricsInfo;
	BOOL m_bReadLyricsComplete;
	CString m_strLyrics1;
	CString m_strLyrics2; 
	CString m_strCurrentTime;
	CString m_strPlayTime;
	CString m_strTitle;
	int m_nReadIndex;	

	std::vector<mp3Info> m_mp3Info;
	int m_nIndex;
	
};
