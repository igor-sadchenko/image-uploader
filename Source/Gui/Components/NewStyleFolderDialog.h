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

#ifndef IU_GUI_NEWSTYLEFOLDERDIALOG_H
#define IU_GUI_NEWSTYLEFOLDERDIALOG_H

#include <memory>
#include "atlheaders.h"
#include "Func/WinUtils.h"
#include "Func/Library.h"

typedef HRESULT(__stdcall *SHCreateItemFromParsingNameFunc)(PCWSTR, IBindCtx *, REFIID, void**);

class CNewStyleFolderDialog {
public:
    
    CNewStyleFolderDialog(HWND parent, const CString& initialFolder, const CString& title, bool onlyFsDirs = true) 
    {
        isVista_ = WinUtils::IsVistaOrLater();
 
        if (!isVista_ )
        {
            oldStyleDialog_ = std::make_unique<CFolderDialog>(parent, title.IsEmpty() ? nullptr : static_cast<LPCTSTR>(title), BIF_NEWDIALOGSTYLE | (onlyFsDirs ? BIF_RETURNONLYFSDIRS : 0));
            oldStyleDialog_->SetInitialFolder(initialFolder, true);
        } else {
            newStyleDialog_= std::make_unique<CShellFileOpenDialog>(/*WinUtils::myExtractFileName(initialFolder)*/nullptr, FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST | FOS_PICKFOLDERS | (onlyFsDirs ? FOS_FORCEFILESYSTEM : 0));
            
            ParseDisplayName_ = shellDll_.GetProcAddress<SHCreateItemFromParsingNameFunc>("SHCreateItemFromParsingName");

            if (!initialFolder.IsEmpty()) {
                // SHCreateItemFromParsingName function not available on Windows XP
                IShellItem *psi;
                
                HRESULT hresult = ParseDisplayName_(initialFolder, nullptr, IID_IShellItem, reinterpret_cast<void**>(&psi));
                if (SUCCEEDED(hresult)) {
                    newStyleDialog_->m_spFileDlg->SetDefaultFolder(psi);
                    psi->Release();
                }
            }

        }
    }

    ~CNewStyleFolderDialog() {
    }

    /*
       This function uses IFileDialog::SetFolder (not SetDefaultFolder) to force initial folder
    */
    void SetFolder(const CString& folder) {
        if (!isVista_) {
            oldStyleDialog_->SetInitialFolder(folder, true);
        } else {
            if (!folder.IsEmpty()) {
                // SHCreateItemFromParsingName function not available on Windows XP
                IShellItem *psi;
                HRESULT hresult = ParseDisplayName_(folder, nullptr, IID_IShellItem, reinterpret_cast<void**>(&psi));
                if (SUCCEEDED(hresult)) {
                    newStyleDialog_->m_spFileDlg->SetFolder(psi);
                    psi->Release();
                }
            }
        }
    }
    INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
    {
        if (!isVista_) {
            return oldStyleDialog_->DoModal(hWndParent);
        } else {
            return newStyleDialog_->DoModal(hWndParent);
        }
    }

    CString GetFolderPath()
    {
        if (!isVista_)
        {
            return oldStyleDialog_->GetFolderPath();
        } else {
            CString fileName;
            newStyleDialog_->GetFilePath(fileName);
            return fileName;
        }
    }

    void SetOkButtonLabel(const CString& label) {
        if (newStyleDialog_) {
            newStyleDialog_->m_spFileDlg->SetOkButtonLabel(label);
        }
    }
protected:
    std::unique_ptr<CFolderDialog> oldStyleDialog_;
    std::unique_ptr<CShellFileOpenDialog> newStyleDialog_;
    bool isVista_;
    Library shellDll_{ L"shell32.dll" };
    SHCreateItemFromParsingNameFunc ParseDisplayName_;

};
#endif