#include "stdafx.h"
#include "EditEx.h"

IMPLEMENT_DYNAMIC(CEditEx, CEdit)

CEditEx::CEditEx()
{
}

CEditEx::~CEditEx()
{
}

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
END_MESSAGE_MAP()

BOOL CEditEx::PreTranslateMessage(MSG* pMsg)
{   
    if(WM_KEYDOWN == pMsg->message)   
    {  
        if(::GetFocus() == m_hWnd && (GetKeyState( VK_CONTROL) & 0xFF00 ) == 0xFF00)   
        {  
            if( pMsg->wParam == 'A' || pMsg->wParam == 'a')  
            {  
                this->SetSel(0, -1);  
                return true;  
            }  
      
            if( pMsg->wParam == 'C' || pMsg->wParam == 'c')  
            {  
				/*
				CString strData;
				GetWindowText(strData);

				int len = strData.GetLength();
				if (len <= 0) return true; 

				if (!OpenClipboard()) return true; 
				
				EmptyClipboard(); 

				HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (len + 1));
				if (hglbCopy == NULL) 
				{ 
					CloseClipboard(); 
					return true; 
				}

				char* lptstrCopy = (char*)GlobalLock(hglbCopy); 
				strcpy(lptstrCopy, (CStringA)strData);
				GlobalUnlock(hglbCopy); 
				SetClipboardData(CF_TEXT, hglbCopy); 
				CloseClipboard();
				*/

                this->Copy();  
                return true;  
            }  
   
            if( pMsg->wParam == 'X' || pMsg->wParam == 'x')  
            {  
                this->Cut();  
                return true;  
            }  
      
            if( pMsg->wParam == 'V' || pMsg->wParam == 'v')  
            {  
				if (!IsClipboardFormatAvailable(CF_TEXT)) 
					return true;  

				if (!OpenClipboard()) 
					return true;  

				HGLOBAL hglb = GetClipboardData(CF_TEXT);
				if (hglb != NULL) 
				{ 
					char* lptstr = (char*) GlobalLock(hglb); 
					if (lptstr != NULL) 
					{  
						CString displayData = CString(lptstr);
						SetWindowText(displayData);
						GlobalUnlock(hglb); 
					} 
				} 
				CloseClipboard(); 

                //this->Paste();  
                return true;  
            }  
      
            if( pMsg->wParam == 'Z' || pMsg->wParam == 'z')  
            {  
                this->Undo();  
                return true;  
			}  
        }  
    }  

    return CEdit::PreTranslateMessage(pMsg);  
 }  