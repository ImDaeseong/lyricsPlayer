#include "stdafx.h"
#include "SkinButton.h"

#define WM_BTN_PAINT WM_USER + 100
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }

IMPLEMENT_DYNAMIC(CSkinButton, CButton)
CSkinButton::CSkinButton():m_bHover(FALSE), m_bTracking(FALSE), m_Clicked(FALSE), m_Status(0)
{
	m_nStartX = 0;
	m_nStartY = 0;
	m_pBGImage = NULL;
	m_nDivide = 4;
	m_bDisable = FALSE;
	m_bVisible = FALSE;
}

CSkinButton::~CSkinButton()
{
	SAFE_DELETE(m_pBGImage);
}

BEGIN_MESSAGE_MAP(CSkinButton, CButton)	
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnBnClicked)
END_MESSAGE_MAP()

void CSkinButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);
	CButton::PreSubclassWindow();
}

void CSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	UINT state = lpDrawItemStruct->itemState;
	int DrawID;

	if (m_bDisable)
	{
		DrawID = 3;
	}
	else
	{
		if (state & ODS_DISABLED) 
		{
			DrawID = 3;
		}	
		else
		{	
			if (state & ODS_SELECTED) 
			{
				DrawID = 2;
			}
			else
			{
				DrawID = IsHover() ? 1: 0;
			}
		}	
	}
	DrawButton(DrawID);
}

LRESULT CSkinButton::WindowProc(UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	LRESULT nResult = CWnd::WindowProc(nMessage,wParam,lParam);
	switch(nMessage)
	{
	case WM_MOUSEMOVE:
		{
			if (!m_bTracking)
			{
				TRACKMOUSEEVENT Tme;
				Tme.cbSize = sizeof(Tme);
				Tme.hwndTrack = GetSafeHwnd();
				Tme.dwFlags = TME_LEAVE|TME_HOVER;
				Tme.dwHoverTime = 1;
				if (_TrackMouseEvent(&Tme))		
					m_bTracking=TRUE;
			}

			if (!m_bDisable)
				::SetCursor(LoadCursor(NULL, IDC_HAND));

			break;				
		}
	case	WM_LBUTTONDOWN:
	case	WM_RBUTTONDOWN:
	case	WM_LBUTTONUP:
	case	WM_RBUTTONUP:
		{

			if (!m_bDisable)
				::SetCursor(LoadCursor(NULL, IDC_HAND));

			break;
		}
	case WM_MOUSEHOVER:
		m_bHover = TRUE;	
		OnHover();
		
		break;
	case WM_MOUSELEAVE:
		m_bTracking = m_bHover = FALSE;
		OnHover();
		break;
	}

	return nResult;
}

BOOL CSkinButton::OnBnClicked()
{
	if(m_Clicked)
	{
		m_Clicked = FALSE;
	}
	else
	{
		m_Clicked = TRUE;
	}

	RedrawWindow();
	return FALSE;
}

void CSkinButton::OnHover()
{
	RedrawWindow();

	if (!m_bDisable)
		::SetCursor(LoadCursor(NULL, IDC_HAND));
}

int CSkinButton::GetImageWidth() const
{
	int res = 0;
	if(m_pBGImage != NULL && m_pBGImage->m_pBitmap != NULL)
	{
		res = m_pBGImage->m_pBitmap->GetWidth() / m_nDivide;
	}
	return res;
}

int CSkinButton::GetImageHeight() const
{
	int res = 0;
	if(m_pBGImage != NULL && m_pBGImage->m_pBitmap != NULL)
	{
		res = m_pBGImage->m_pBitmap->GetHeight();
	}
	return res;
}

void CSkinButton::DrawButton(int id)
{
	m_Status = id;
	GetOwner()->SendMessage(WM_PAINT , 0 , 0 );
}

void CSkinButton::OnDrawWindow(Graphics& gps)
{
	if(m_bVisible) return;

	CRect rc;
	GetWindowRect(rc);
	GetOwner()->ScreenToClient(rc);

	int nX = m_pBGImage->m_pBitmap->GetWidth() / m_nDivide;
	int nY = m_pBGImage->m_pBitmap->GetHeight();

	gps.DrawImage(m_pBGImage->m_pBitmap, Rect(rc.left, rc.top, nX, nY), nX * m_Status, 0, nX, nY, UnitPixel);
}

void CSkinButton::OnDrawLayerdWindow(Graphics& Gps)
{
	if(m_bVisible) return;

	CRect rc;
	GetWindowRect(rc);
	GetOwner()->ScreenToClient(rc);	

	CSize szFrame = (GetStyle() & WS_THICKFRAME) ? CSize(GetSystemMetrics(SM_CXSIZEFRAME),  GetSystemMetrics(SM_CYSIZEFRAME)) : CSize(GetSystemMetrics(SM_CXFIXEDFRAME), GetSystemMetrics(SM_CYFIXEDFRAME));
	rc.OffsetRect(szFrame.cy, szFrame.cy);

	int nX = m_pBGImage->m_pBitmap->GetWidth() / m_nDivide;
	int nY = m_pBGImage->m_pBitmap->GetHeight();

	if(m_nDivide == 1)
		Gps.DrawImage(m_pBGImage->m_pBitmap, Rect(rc.left, rc.top, nX, nY), 0, 0, nX, nY, UnitPixel);
	else
		Gps.DrawImage(m_pBGImage->m_pBitmap, Rect(rc.left, rc.top, nX, nY), nX * m_Status, 0, nX, nY, UnitPixel);
}

BOOL CSkinButton::SetImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst, int nStartX, int nStartY, int nDivide)
{
	if (!m_pBGImage)
	{
		m_pBGImage = new CGdiPlusBitmapResource;
		if (!m_pBGImage->Load(pName, pType, hInst))
		{
			Free();
			return FALSE;
		}
	
		m_nDivide = nDivide;
		m_nStartX = nStartX;
		m_nStartY = nStartY;
		int nX = m_pBGImage->m_pBitmap->GetWidth() / m_nDivide;
		int nY = m_pBGImage->m_pBitmap->GetHeight();
		MoveWindow(m_nStartX, m_nStartY, nX, nY);	

		return TRUE;
	}
	return FALSE;
}

BOOL CSkinButton::SetImage(UINT uid, LPCTSTR pType, HMODULE hInst, int nStartX, int nStartY, int nDivide)
{
	if (!m_pBGImage)
	{
		m_pBGImage = new CGdiPlusBitmapResource;

		if (!m_pBGImage->Load(uid, pType, hInst))
		{
			Free();
			return FALSE;
		}
	
		m_nDivide = nDivide;
		m_nStartX = nStartX;
		m_nStartY = nStartY;
		int nX = m_pBGImage->m_pBitmap->GetWidth() / m_nDivide;
		int nY = m_pBGImage->m_pBitmap->GetHeight();	
		MoveWindow(m_nStartX, m_nStartY, nX, nY);	

		return TRUE;
	}
	return FALSE;
}

BOOL CSkinButton::SetImg(LPCTSTR pName, int nStartX, int nStartY, int nDivide)
{
	if(!m_pBGImage)
	{
		m_pBGImage = new CGdiPlusBitmapResource;
		if(!m_pBGImage->LoadPath(pName))
		{
			Free();
			return FALSE;
		}

		m_nDivide = nDivide;
		m_nStartX = nStartX;
		m_nStartY = nStartY;
		int nX = m_pBGImage->m_pBitmap->GetWidth() /m_nDivide;
		int nY = m_pBGImage->m_pBitmap->GetHeight();
		MoveWindow(m_nStartX, m_nStartY, nX, nY);	

		return TRUE;
	}
	return FALSE;	
}

BOOL CSkinButton::SetImgRefresh(LPCTSTR pName)
{
	if(!m_pBGImage)
		m_pBGImage = new CGdiPlusBitmapResource;
	
	if(!m_pBGImage->LoadPath(pName))
	{
		Free();
		return FALSE;
	}
		
	int nX = m_pBGImage->m_pBitmap->GetWidth() /m_nDivide;
	int nY = m_pBGImage->m_pBitmap->GetHeight();
	MoveWindow(m_nStartX, m_nStartY, nX, nY);	

	return TRUE;
}

BOOL CSkinButton::IsAvailableDraw()
{
	if(!m_pBGImage)
		return FALSE;
	return TRUE;
}

void CSkinButton::SetDisableButton()
{
	m_bDisable = TRUE;
	RedrawWindow();
}

void CSkinButton::SetEnableButton()
{
	m_bDisable = FALSE;
	RedrawWindow();
}

void CSkinButton::Free()
{
	SAFE_DELETE(m_pBGImage);
}

void CSkinButton::SetHover(BOOL bSet)
{
	if(bSet)
	{
		m_bHover = TRUE;	
		OnHover();
	}
	else
	{
		m_bTracking = m_bHover = FALSE;
		OnHover();
	}
}

void CSkinButton::SetVisibleButton(BOOL bVisible)
{
	m_bVisible = bVisible;
	RedrawWindow();
}