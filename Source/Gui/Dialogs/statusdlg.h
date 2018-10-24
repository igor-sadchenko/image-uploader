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
#ifndef STATUSDLG_H
#define STATUSDLG_H

#pragma once
#include "atlheaders.h"
#include "resource.h"       // main symbols
#include "3rdpart/thread.h"

// CStatusDlg

class CStatusDlg :
    public CDialogImpl<CStatusDlg>
{
    public:
        CStatusDlg();
        ~CStatusDlg();
        enum { IDD = IDD_STATUSDLG };
        CString m_Title, m_Text;
        bool m_bNeedStop;
        CAutoCriticalSection CriticalSection, Section2;
        BEGIN_MSG_MAP(CStatusDlg)
            MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
            MESSAGE_HANDLER(WM_TIMER, OnTimer)
            COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
        END_MSG_MAP()
        // Handler prototypes:
        //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
        //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
        LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
        void SetInfo(const CString& Title, const CString& Text);
        void SetWindowTitle(const CString& WindowTitle);
        bool NeedStop();
        void Hide();
protected:
    CFont titleFont_;
};

#endif // STATUSDLG_H
