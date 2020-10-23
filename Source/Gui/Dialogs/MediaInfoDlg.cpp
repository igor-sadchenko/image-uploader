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

#include "MediaInfoDlg.h"

#include "Func/MediaInfoHelper.h"
#include "Gui/GuiTools.h"
#include "Func/WinUtils.h"
#include "Func/myutils.h"

// CMediaInfoDlg
CMediaInfoDlg::CMediaInfoDlg(InfoType type)
{

}

CMediaInfoDlg::~CMediaInfoDlg()
{
    
}

LRESULT CMediaInfoDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CenterWindow(GetParent());
    GuiTools::MakeLabelBold(GetDlgItem(IDC_FILEINFOLABEL));
    DlgResize_Init(false, true, 0); // resizable dialog without "griper"
    LOGFONT logFont;
    StringToFont(_T("Courier New,8,,204"), &logFont);
    
    editFont_.CreateFontIndirect(&logFont);
    SendDlgItemMessage(IDC_FILEINFOEDIT, WM_SETFONT, (WPARAM)(HFONT)editFont_, MAKELPARAM(false, 0));
    // Translating controls' text
    TRC(IDOK, "OK");
    SetWindowText(TR("Information about file"));
    TRC(IDC_COPYALL, "Copy to clipboard");
    TRC(IDC_SUMMARYRADIOBUTTON, "Summary");
    TRC(IDC_FULLINFORADIOBUTTON, "Full information");

    SetDlgItemText(IDC_FILEINFOEDIT, TR("Loading..."));

    
    ::SetFocus(GetDlgItem(IDOK));

    Start(); // Starting thread which will load in background
                // information about file m_FileName
    return 0; 
}

LRESULT CMediaInfoDlg::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{    
    if(!IsRunning())  EndDialog(wID); // Don't allow user to close dialog before thread finishes
    return 0;
}

LRESULT CMediaInfoDlg::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    if(!IsRunning())  EndDialog(wID);
    return 0;
}

void CMediaInfoDlg::ShowInfo(LPCTSTR FileName)
{
    m_FileName = FileName;
    DoModal();
}

DWORD CMediaInfoDlg::Run()
{
    CString  ShortFileName = WinUtils::TrimString(WinUtils::myExtractFileName(m_FileName), 40);
    if(!WinUtils::FileExists(m_FileName))
    { 
        SetDlgItemText(IDC_FILEINFOLABEL, CString(TR("Error:")));
        SetDlgItemText(IDC_FILEINFOEDIT, CString(TR("File \"")) + ShortFileName + TR("\" not found!"));
        return 0;
    }

    SetDlgItemText(IDC_FILEINFOLABEL,CString(TR("Information about file"))+_T(" \"")+ ShortFileName+_T("\" :"));
    
    MediaInfoHelper::GetMediaFileInfo(m_FileName, summary_, fullInfo_);
    bool fullInfo = GuiTools::GetCheck(m_hWnd, IDC_FULLINFORADIOBUTTON);
    SetDlgItemText(IDC_FILEINFOEDIT, fullInfo ? fullInfo_ : summary_);
    return 0;
}

LRESULT CMediaInfoDlg::OnBnClickedCopyall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // Copying text to clipboard
    SendDlgItemMessage(IDC_FILEINFOEDIT, EM_SETSEL, 0, -1);
    SendDlgItemMessage(IDC_FILEINFOEDIT, WM_COPY, 0, 0);
    SendDlgItemMessage(IDC_FILEINFOEDIT, EM_SETSEL, static_cast<WPARAM>(-1), 0);
    return 0;
}