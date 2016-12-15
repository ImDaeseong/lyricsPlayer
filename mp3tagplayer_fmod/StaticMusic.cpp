#include "stdafx.h"
#include "StaticMusic.h"

CStaticMusic::CStaticMusic()
{
	m_strText = "";
	m_bLine = FALSE;
}

CStaticMusic::~CStaticMusic()
{
}

BEGIN_MESSAGE_MAP(CStaticMusic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CStaticMusic::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CStaticMusic::OnPaint() 
{		
	CPaintDC dc(this); 
}

void CStaticMusic::OnDrawLayerdWindow(Graphics& Gps)
{	
USES_CONVERSION;

	Gps.SetSmoothingMode(SmoothingModeHighQuality);
	Gps.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	CRect rect;
    GetClientRect(&rect);

	RectF destRect(REAL(rect.left),	REAL(rect.top),	REAL(rect.Width()),	REAL(rect.Height()));
		
	if(m_bLine)
	{
		Pen dashedPen(Color(50, 50, 50, 0), 1);
		dashedPen.SetDashStyle(Gdiplus::DashStyle::DashStyleDash);
		Gps.DrawRectangle(&dashedPen, destRect.X + 1, destRect.Y + 1, destRect.Width-2, destRect.Height-2);
	}

	Gdiplus::Font font(L"µ¸¿ò", 12, FontStyle::FontStyleRegular, UnitPixel);
	SolidBrush brush(Color(255, 255, 255));

	StringFormat stringFormat(StringFormatFlagsDisplayFormatControl);
	stringFormat.SetAlignment(StringAlignmentCenter);	
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	RectF rectF(0, 0, (float)rect.Width(), (float)rect.Height());		
	Gps.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	Gps.DrawString(A2W(m_strText), -1, &font, rectF, &stringFormat, &brush);	

	//check size
	//SolidBrush bgbrush(Color(255, 0, 0));
	//Gps.FillRectangle(&bgbrush, destRect);
}

void CStaticMusic::SetLyrics(CString strText)
{
	m_strText = strText;
	Invalidate(FALSE);
}
