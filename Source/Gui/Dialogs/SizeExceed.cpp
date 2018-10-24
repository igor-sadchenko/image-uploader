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

#include "SizeExceed.h"

#include "Func/common.h"
#include "Core/Upload/UploadEngine.h"
#include "Core/Settings.h"
#include "Gui/GuiTools.h"
#include "Func/myutils.h"
#include "Func/WinUtils.h"
#include "Core/Upload/FileUploadTask.h"
#include <Func/IuCommonFunctions.h>
#include "Func/MyEngineList.h"

// CSizeExceed
CSizeExceed::CSizeExceed(FileUploadTask * fileTask, CUploadEngineList * EngineList, UploadEngineManager* uploadEngineManager):
        m_EngineList(EngineList)
{
    fileTask_ = fileTask;
    m_szFileName = U2W(fileTask->getFileName());
    uploadEngineManager_ = uploadEngineManager;
}

CSizeExceed::~CSizeExceed()
{
}

void CSizeExceed::Translate()
{
    TRC(IDC_WHATTODO, "Change image processing settings or choose another server, so your image can be uploaded.");

    TRC(IDOK, "OK");
    TRC(IDC_FORALL, "To all");
    TRC(IDCANCEL, "Ignore");
    SetWindowText(TR("File size exceeding"));
}

LRESULT CSizeExceed::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    RECT rc = {12, 30, 162, 144};
    img.Create(m_hWnd, rc);
    bool isImage = fileTask_->isImage() && IuCommonFunctions::IsImage(m_szFileName);
    if (isImage) {
        img.LoadImage(m_szFileName);
    }
    
    CenterWindow(GetParent());

    RECT serverSelectorRect = GuiTools::GetDialogItemRect(m_hWnd, IDC_SERVERPLACEHOLDER);
    imageServerSelector_.reset(new CServerSelectorControl(uploadEngineManager_, true));
    imageServerSelector_->Create(m_hWnd, serverSelectorRect);
    imageServerSelector_->setTitle(fileTask_->isImage() ? TR("Server for uploading images") : TR("Server for other file types"));
    if (!fileTask_->isImage()) {
        imageServerSelector_->setServersMask(CServerSelectorControl::smFileServers);
    }
    
    imageServerSelector_->ShowWindow(SW_SHOW);
    imageServerSelector_->SetWindowPos(0, serverSelectorRect.left, serverSelectorRect.top, serverSelectorRect.right - serverSelectorRect.left, 
                                        serverSelectorRect.bottom - serverSelectorRect.top, 0);
    imageServerSelector_->setServerProfile(Settings.imageServer);
    
    CString serverName = U2W(fileTask_->serverProfile().serverName());

    GuiTools::MakeLabelBold(GetDlgItem(IDC_FILEEXCEEDNAME));
    
    int64_t fileSize = IuCoreUtils::getFileSize(W2U(m_szFileName));
    WCHAR buf2[25];
    WinUtils::NewBytesToString(fileSize, buf2, 25);

    CString name;
    CString params; 
    if (isImage) {
        params.Format(_T(" %s (%dx%d, %s)"), static_cast<LPCTSTR>(myExtractFileName(m_szFileName)), img.ImageWidth, img.ImageHeight, static_cast<LPCTSTR>(buf2));
    } else {
        params.Format(_T(" %s (%s)"), static_cast<LPCTSTR>(myExtractFileName(m_szFileName)), static_cast<LPCTSTR>(buf2));
    }
   
    name = TR("File") + params;

    SetDlgItemText(IDC_FILEEXCEEDNAME, name);
    WinUtils::NewBytesToString(m_EngineList->byName(fileTask_->serverProfile().serverName())->MaxFileSize, buf2, 25);

    TCHAR szBuf[1000];
    wsprintf(szBuf, TR("File exceeds filesize limit of \"%s\" server (%s)."),
        static_cast<LPCTSTR>(U2W(fileTask_->serverProfile().serverName())), buf2);
    SetDlgItemText(IDC_FILEEXCEEDSIZE2, szBuf);
    Translate();

    return 1;  // Let the system set the focus
}

LRESULT CSizeExceed::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    std::string serverName = imageServerSelector_->serverProfile().serverName();
    if (serverName.empty()) {
        MessageBox(TR("You have not selected server!"), TR("Error"), MB_ICONERROR);
        return 0;
    } else if (!imageServerSelector_->isAccountChosen()) {
        CString message;
        message.Format(TR("You have not selected account for server \"%s\""), IuCoreUtils::Utf8ToWstring(imageServerSelector_->serverProfile().serverName()).c_str());
        MessageBox(message, TR("Error"), MB_ICONERROR);
        return 0;
    }

    fileTask_->setServerProfile(imageServerSelector_->serverProfile());
    EndDialog(wID);
    return 0;
}

LRESULT CSizeExceed::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    EndDialog(wID);
    return 0;
}

LRESULT CSizeExceed::OnBnClickedForall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    fileTask_->setServerProfile(imageServerSelector_->serverProfile());
    EndDialog(3);
    return 0;
}
