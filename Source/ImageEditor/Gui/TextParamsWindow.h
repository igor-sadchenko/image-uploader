/*

    Image Uploader -  free application for uploading images/files to the Internet

    Copyright 2007-2018 Sergey Svistunov (zenden2k@yandex.ru)

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/

#pragma once
#include "atlheaders.h"
#include "Gui/Controls/MyImage.h"
#include "resource.h"
#include <vector>
#include <thread>

class FontEnumerator;
class TextParamsWindow;
class CustomEdit: public CWindowImpl<CustomEdit, CEdit,CControlWinTraits>{
public:
    CustomEdit(TextParamsWindow* textParamsWindow);
    ~CustomEdit();
    DECLARE_WND_SUPERCLASS(_T("CustomComboBox"), CEdit::GetWndClassName())

    BEGIN_MSG_MAP(CustomEdit)
        MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
        MESSAGE_HANDLER(WM_CHAR, OnChar)
    END_MSG_MAP()

    // Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
    TextParamsWindow* textParamsWindow_;
};
class TextParamsWindow : public CDialogImpl<TextParamsWindow>
{
    typedef CDialogImpl<TextParamsWindow> TBase;
    public:
        enum { IDD = IDD_TEXTPARAMSWINDOW, TPWM_FONTCHANGED = WM_USER + 123, IDC_BOLD = 1500, IDC_ITALIC, IDC_UNDERLINE};
        TextParamsWindow();
        ~TextParamsWindow();
        void setFont(LOGFONT logFont);
        LOGFONT getFont();
        /*HWND Create(HWND parent);*/
    protected:
        BEGIN_MSG_MAP(CTextParamsWindow)
            MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
            MESSAGE_HANDLER(WM_CLOSE, OnClose)
            COMMAND_HANDLER(IDC_FONTCOMBO, CBN_SELCHANGE, OnFontSelChange);
            COMMAND_HANDLER(IDC_FONTSIZECOMBO, CBN_SELCHANGE, OnFontSizeSelChange);
            COMMAND_ID_HANDLER(IDC_BOLD, OnBoldClick)
            COMMAND_ID_HANDLER(IDC_ITALIC, OnItalicClick)
            COMMAND_ID_HANDLER(IDC_UNDERLINE, OnUnderlineClick)
        END_MSG_MAP()

        // Handler prototypes (uncomment arguments if needed):
        //    LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        //    LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
        //    LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

        // Message handlers
        LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
        LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
        LRESULT OnFontSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
        LRESULT OnFontSizeSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
        LRESULT OnBoldClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
        LRESULT OnItalicClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
        LRESULT OnUnderlineClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

        CToolBarCtrl textToolbar_;
        void OnFontEnumerationFinished();
        std::thread fontEnumerationThread_;
        CComboBox fontComboBox_;
        CComboBox fontSizeComboBox_;
        CustomEdit fontSizeComboboxCustomEdit_;
        CString fontName_;
        LOGFONT font_;
        CImageList toolbarImageList_;
        void NotifyParent(DWORD changeMask);
        std::vector<LOGFONT> fonts_;
        CIcon iconBold_, iconItalic_, iconUnderline_;
        friend class CustomEdit;
};
