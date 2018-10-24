#pragma once

#include "../atlheaders.h"
#include <atlcrack.h>
#include <atlmisc.h>
#define WM_KEY                      (WM_USER + 444)
#include "MemberFunctionCallback.h"

// CWinHotkeyCtrl

class CWinHotkeyCtrl : public CWindowImpl<CWinHotkeyCtrl, CEdit,CControlWinTraits>
{
	

public:
	CWinHotkeyCtrl();
	virtual ~CWinHotkeyCtrl();
	enum { IDM_CLEARHOTKEYDATA = 0xFF};
	void UpdateText();
	DWORD GetWinHotkey();
	BOOL GetWinHotkey(UINT* pvkCode, UINT* pfModifiers);
	void SetWinHotkey(DWORD dwHk);
	void SetWinHotkey(UINT vkCode, UINT fModifiers);
	void Clear();
DECLARE_WND_SUPERCLASS(_T("CWinHotkeyCtrl"), CEdit::GetWndClassName());

	BEGIN_MSG_MAP(CWinHotkeyCtrl)
		MESSAGE_HANDLER(WM_KEY, OnKey)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MSG_WM_CHAR(OnChar)
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_CONTEXTMENU(OnContextMenu)
	END_MSG_MAP()

private:
	static HHOOK sm_hhookKb;
	static CWinHotkeyCtrl* sm_pwhcFocus;

	UINT m_vkCode;
	DWORD m_fModSet;
	DWORD m_fModRel;
	BOOL m_fIsPressed;
    CBTHookMemberFunctionCallback m_callback;
private:
	BOOL InstallKbHook();
	BOOL UninstallKbHook();
    
#if _WIN32_WINNT < 0x500
	static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
#else // _WIN32_WINNT >= 0x500
	/*static */LRESULT /*CALLBACK*/ LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
#endif // _WIN32_WINNT >= 0x500

	LRESULT OnKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//  

public:
	 void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	 BOOL OnSetCursor(HWND pWnd, UINT nHitTest, UINT message);
	
	 void OnContextMenu(HWND /*pWnd*/, CPoint /*point*/);
protected:
	virtual void PreSubclassWindow();
};
