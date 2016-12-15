#pragma once

class CSkinButton : public CButton
{
	DECLARE_DYNAMIC(CSkinButton)

public:
	CSkinButton();
	virtual ~CSkinButton();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	
public:
	void Free();
	void SetEnableButton();
	void SetDisableButton();

	void SetVisibleButton(BOOL bVisible);

	BOOL SetImage(UINT uid, LPCTSTR pType, HMODULE hInst, int nStartX, int nStartY, int nDivide = 4);
	BOOL SetImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst, int nStartX, int nStartY, int nDivide = 4);
	BOOL SetImg(LPCTSTR pName, int nStartX, int nStartY, int nDivide = 4 );
	BOOL SetImgRefresh(LPCTSTR pName);
	
	BOOL IsAvailableDraw();

	LRESULT WindowProc(UINT nMessage, WPARAM wParam, LPARAM lParam);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	BOOL IsHover(){ return m_bHover; }

	int GetImageWidth() const;
	int GetImageHeight() const;

	afx_msg BOOL OnBnClicked();
	void OnHover();
	void DrawButton(int id);
	void OnDrawWindow(Graphics& Gps);
	void OnDrawLayerdWindow(Graphics& Gps);	
	void SetHover(BOOL bSet);
private:
	BOOL m_bHover;
	BOOL m_bTracking;
	BOOL m_Clicked;
	int m_Status;
	CGdiPlusBitmapResource * m_pBGImage;

	int m_nStartX;
	int m_nStartY;
	int m_nDivide;
	BOOL m_bDisable;

	BOOL m_bVisible;
};