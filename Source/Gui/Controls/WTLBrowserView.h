// WTLBrowserView.h : interface of the CWTLBrowserView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WTLBROWSERVIEW_H__27BEDF70_F34D_4698_A226_534A3D32E98B__INCLUDED_)
#define AFX_WTLBROWSERVIEW_H__27BEDF70_F34D_4698_A226_534A3D32E98B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Browser.h"
#include "Core/3rdpart/FastDelegate.h"

class CWTLBrowserView : public CWindowImpl<CWTLBrowserView, CAxWindow>, public CWebBrowser2<CWTLBrowserView>
{
public:
    CWTLBrowserView() : m_bSecured(FALSE)
    {
    }
    
    DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

    BOOL PreTranslateMessage(MSG* pMsg);

    BEGIN_MSG_MAP(CWTLBrowserView)
        CHAIN_MSG_MAP(CWebBrowser2<CWTLBrowserView>)
    END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//    LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//    LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//    LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    void OnStatusTextChange(const String& szText);
    void OnProgressChange(long nProgress, long nProgressMax);
    void OnSetSecureLockIcon(long nSecureLockIcon);
    void OnNavigateComplete2(IDispatch* pDisp, const CString& szURL);
    BOOL OnNavigateError(IDispatch* pDisp, const String& szURL, const String& szTargetFrameName, LONG nStatusCode);
    void OnPropertyChange(const String& szProperty);
    void OnDocumentComplete(IDispatch* pDisp, const String& szURL);

    BOOL IsSecured() const
    {
        return m_bSecured;
    }

    fastdelegate::FastDelegate1<const CString&> onNavigateComplete2;
    fastdelegate::FastDelegate1<const CString&> onDocumentComplete;
    fastdelegate::FastDelegate2<const CString&, LONG, bool> onNavigateError;
protected:
    BOOL m_bSecured;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WTLBROWSERVIEW_H__27BEDF70_F34D_4698_A226_534A3D32E98B__INCLUDED_)
