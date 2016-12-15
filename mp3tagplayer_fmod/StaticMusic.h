#pragma once

class CStaticMusic : public CStatic
{
public:
	CStaticMusic();
	virtual ~CStaticMusic();

public:
	void OnDrawLayerdWindow(Graphics& Gps);
	void SetLyrics(CString strText);

protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

private:
	CString m_strText;
	BOOL m_bLine;
};
