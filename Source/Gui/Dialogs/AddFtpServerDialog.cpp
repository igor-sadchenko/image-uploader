// This file was generated by WTL Dialog wizard 
// AddFtpServerDialog.cpp : Implementation of CAddFtpServerDialog

#include "AddFtpServerDialog.h"
#include "Gui/GuiTools.h"
#include "Core/ServerListManager.h"
#include "Core/Settings/WtlGuiSettings.h"
#include "Func/IuCommonFunctions.h"

// CAddFtpServerDialog
CAddFtpServerDialog::CAddFtpServerDialog(CUploadEngineList* uploadEngineList)
{
    connectionNameEdited = false;
    downloadUrlEdited = false;
    serverNameEdited = false;
    uploadEngineList_ = uploadEngineList;
}

CAddFtpServerDialog::~CAddFtpServerDialog()
{
}

LRESULT CAddFtpServerDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    SetWindowText(TR("Add FTP server"));
    TRC(IDC_CONNECTIONNAMELABEL, "Connection  name:");
    TRC(IDC_SERVERSTATIC, "Server [:port]:");
    TRC(IDC_AUTHENTICATIONLABEL, "Authenticaton:");
    TRC(IDC_LOGINLABEL, "Login:");
    TRC(IDC_PASSWORDLABEL, "Password:");
    TRC(IDC_REMOTEDIRECTORYLABEL, "Remote directory:");
    TRC(IDC_DOWNLOADURLLABEL, "URL for downloading:");
    TRC(IDCANCEL, "Cancel");
    TRC(IDC_THEURLOFUPLOADEDLABEL, "URL for downloading will look like:");

    if (ServiceLocator::instance()->translator()->isRTL()) {
        // Removing WS_EX_RTLREADING style from some controls to look properly when RTL interface language is choosen
        HWND serverEditHwnd = GetDlgItem(IDC_SERVEREDIT);
        LONG styleEx = ::GetWindowLong(serverEditHwnd, GWL_EXSTYLE);
        ::SetWindowLong(serverEditHwnd, GWL_EXSTYLE, styleEx & ~WS_EX_RTLREADING);

        HWND remoteDirectoryEditHwnd = GetDlgItem(IDC_REMOTEDIRECTORYEDIT);
        styleEx = ::GetWindowLong(remoteDirectoryEditHwnd, GWL_EXSTYLE);
        ::SetWindowLong(remoteDirectoryEditHwnd, GWL_EXSTYLE, styleEx & ~WS_EX_RTLREADING);

        HWND downloadUrlEditHwnd = GetDlgItem(IDC_DOWNLOADURLEDIT);
        styleEx = ::GetWindowLong(downloadUrlEditHwnd, GWL_EXSTYLE);
        ::SetWindowLong(downloadUrlEditHwnd, GWL_EXSTYLE, styleEx & ~WS_EX_RTLREADING);

        HWND exampleUrlLabel = GetDlgItem(IDC_EXAMPLEURLLABEL);
        styleEx = ::GetWindowLong(exampleUrlLabel, GWL_EXSTYLE);
        ::SetWindowLong(exampleUrlLabel, GWL_EXSTYLE, styleEx & ~WS_EX_RTLREADING & ~WS_EX_LAYOUTRTL);
        LONG style = ::GetWindowLong(exampleUrlLabel, GWL_STYLE);
        ::SetWindowLong(exampleUrlLabel, GWL_STYLE, style | SS_RIGHT);
    }

    //TRC(IDC_CONNECTIONNAMEEDIT, "New FTP Connection");
    ::SetFocus(GetDlgItem(IDC_CONNECTIONNAMEEDIT));
    SetDlgItemText(IDC_REMOTEDIRECTORYEDIT, _T("/"));
    CenterWindow(GetParent());
    return 0;  // Let the system set the focus
}

LRESULT CAddFtpServerDialog::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    CString serverName = GuiTools::GetDlgItemText(m_hWnd, IDC_SERVEREDIT);
    serverName.TrimLeft(L" ");
    serverName.TrimRight(L" ");

    if ( serverName.IsEmpty() ) {
        MessageBox(TR("Server's name cannot be empty"),TR("Error"), MB_ICONERROR);
        return 0;
    }
    CString connectionName = GuiTools::GetDlgItemText(m_hWnd, IDC_CONNECTIONNAMEEDIT);
    connectionName.TrimLeft(L" ");
    connectionName.TrimRight(L" ");
    if ( connectionName.IsEmpty() ) {
        LocalizedMessageBox(TR("Connection name cannot be empty"), TR("Error"), MB_ICONERROR);
        return 0;
    }

    CString downloadUrl = GuiTools::GetDlgItemText(m_hWnd, IDC_DOWNLOADURLEDIT);
    downloadUrl.TrimLeft(L" ");
    downloadUrl.TrimRight(L" ");
    if ( downloadUrl.IsEmpty() ) {
        LocalizedMessageBox(TR("Download URL cannot be empty."), TR("Error"), MB_ICONERROR);
        return 0;
    }

    CString remoteDirectory = GuiTools::GetDlgItemText(m_hWnd, IDC_REMOTEDIRECTORYEDIT);
    if ( remoteDirectory.Left(1) != _T("/") ) {
        remoteDirectory = _T("/") + remoteDirectory;
    }
    if ( remoteDirectory.Right(1) != _T("/") ) {
        remoteDirectory += _T("/");

    }
    CString login = GuiTools::GetDlgItemText(m_hWnd, IDC_LOGINEDITBOX);
    CString password = GuiTools::GetDlgItemText(m_hWnd, IDC_PASSWORDEDITBOX);

    WtlGuiSettings& Settings = *ServiceLocator::instance()->settings<WtlGuiSettings>();

    ServerListManager slm(Settings.SettingsFolder + "\\Servers\\", uploadEngineList_, Settings.ServersSettings);
    if ( slm.addFtpServer(IuCoreUtils::WstringToUtf8((LPCTSTR)connectionName), IuCoreUtils::WstringToUtf8((LPCTSTR)serverName), IuCoreUtils::WstringToUtf8((LPCTSTR)login), 
        IuCoreUtils::WstringToUtf8((LPCTSTR)password), IuCoreUtils::WstringToUtf8((LPCTSTR)remoteDirectory), IuCoreUtils::WstringToUtf8((LPCTSTR)downloadUrl)) ) {
            createdServerName_ = IuCoreUtils::Utf8ToWstring(slm.createdServerName()).c_str();
            createdServerLogin_ = login;
            EndDialog(wID);
    } else {
        CString errorMessage = TR("Could not add server.");
        CString reason = IuCoreUtils::Utf8ToWstring(slm.errorMessage()).c_str();
        if ( !reason.IsEmpty() ) {
            errorMessage += CString(L"\r\n") + TR("Reason:") + L"\r\n" + reason;
        }
        LocalizedMessageBox(errorMessage, TR("Error"), MB_ICONERROR);

    }

    
    return 0;
}

LRESULT CAddFtpServerDialog::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    EndDialog(wID);
    return 0;
}


LRESULT CAddFtpServerDialog::OnConnectionNameEditChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    if ( GetFocus() == hWndCtl ) {
        connectionNameEdited = !GuiTools::GetDlgItemText(m_hWnd, IDC_CONNECTIONNAMEEDIT).IsEmpty();
    }
    if ( !serverNameEdited) {
        CString connectionName = GuiTools::GetDlgItemText(m_hWnd, IDC_CONNECTIONNAMEEDIT);
        if ( connectionName.Find(_T(".")) != -1 ) {
            SetDlgItemText(IDC_SERVEREDIT, connectionName);
        }
    }

    return 0;
}

LRESULT CAddFtpServerDialog::OnServerEditChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    if ( GetFocus() == hWndCtl ) {
        serverNameEdited = !GuiTools::GetDlgItemText(m_hWnd, IDC_SERVEREDIT).IsEmpty();
    }
    if ( !connectionNameEdited ) {
        CString serverName = GuiTools::GetDlgItemText(m_hWnd, IDC_SERVEREDIT);
        SetDlgItemText(IDC_CONNECTIONNAMEEDIT, serverName);
    }
    GenerateDownloadLink();

    return 0;
}

LRESULT CAddFtpServerDialog::OnRemoteDirectoryEditChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    GenerateDownloadLink();
    return 0;
}

LRESULT CAddFtpServerDialog::OnDownloadUrlEditChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
    if ( GetFocus() == hWndCtl ) {
        downloadUrlEdited = !GuiTools::GetDlgItemText(m_hWnd, IDC_DOWNLOADURLEDIT).IsEmpty();
        GenerateExampleUrl();
    }
    return 0;
}

CString CAddFtpServerDialog::createdServerName()
{
    return createdServerName_;
}

CString CAddFtpServerDialog::createdServerLogin()
{
    return createdServerLogin_;
}

void CAddFtpServerDialog::GenerateDownloadLink()
{
    if ( !downloadUrlEdited ) {
        CString serverName = GuiTools::GetDlgItemText(m_hWnd, IDC_SERVEREDIT);
        int pos = serverName.ReverseFind(L':');
        if ( pos != -1 ) {
            serverName = serverName.Left(pos);
        }

        CString remoteDirectory = GuiTools::GetDlgItemText(m_hWnd, IDC_REMOTEDIRECTORYEDIT);
        if ( remoteDirectory.Left(1) != _T("/") ) {
            remoteDirectory = _T("/") + remoteDirectory;
        }
        if ( remoteDirectory.Right(1) != _T("/") ) {
            remoteDirectory += _T("/");

        }
        CString generatedDownloadUrl = "http://" + serverName + remoteDirectory;
        
        if ( !serverName.IsEmpty() ) {
            SetDlgItemText(IDC_DOWNLOADURLEDIT, generatedDownloadUrl);
            
        }
    }
    GenerateExampleUrl();
}

void CAddFtpServerDialog::GenerateExampleUrl()
{
    CString downloadUrl = GuiTools::GetDlgItemText(m_hWnd, IDC_DOWNLOADURLEDIT);

    SetDlgItemText(IDC_EXAMPLEURLLABEL, downloadUrl + "example.png");
}
