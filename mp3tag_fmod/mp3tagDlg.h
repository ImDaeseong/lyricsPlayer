#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class Cmp3tagDlg : public CDialog
{
public:
	Cmp3tagDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_MP3TAG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	

protected:	
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonSelectfile();
	afx_msg void OnBnClickedButtonPlay();
    afx_msg void OnBnClickedButtonMake();
	afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnBnClickedButtonSkipleft();
	afx_msg void OnBnClickedButtonSkipright();
	DECLARE_MESSAGE_MAP()

private:	
	CListCtrl m_ListCtrl;
	CEditEx m_edtLyrics;
	CSliderCtrl m_sliderRunning;
	CAudioPlayer m_Player;
	CFmodPlayer m_FPlayer;

	BOOL m_bClickPlay;
	CString m_strCurrentPath;
	int m_nlength;
	CString	m_strTimer;
	int nCurPosition;

	void Play();
	void Stop();
	void Pause();	

	CString ReadMp3(const char* file);
	void ReadStringList(CString strInput);
	CString GetElementData(int nOpt, CString strData);
    CString GetModulePath();
	CString GetFileExtName(CString strFilename);
	CString GetFileFullName(CString strFilename);
	CString GetFileName(CString strFilename);
	CString GetFilePath(CString strFilename);

	std::vector<CString> mLineArray;
	int m_nIndex;
	int m_nSize;
	CString m_strSeconds;

	void InsertListView(CString strTime, CString strLyrics, CString strAlih);	

	BOOL m_bEditingLyrics;
};
