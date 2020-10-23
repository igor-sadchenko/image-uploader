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

#include "AboutDlg.h"

#include <curl/curl.h>
#include "Core/Settings.h"
#include "Gui/GuiTools.h"
#include "Func/WinUtils.h"
#include <libavutil/ffversion.h>
#include <boost/config.hpp>
#include <boost/version.hpp>
#include <webp/decode.h>
#include "Core/AppParams.h"
#include "Func/MediaInfoHelper.h"

LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    thanksToLabelFont_ = GuiTools::MakeLabelBold(GetDlgItem(IDC_THANKSTOLABEL));
    LogoImage.SubclassWindow(GetDlgItem(IDC_STATICLOGO));
    LogoImage.SetWindowPos(0, 0,0, 48, 48, SWP_NOMOVE );
    LogoImage.LoadImage(0, 0, IDR_ICONMAINNEW, false, GetSysColor(COLOR_BTNFACE));
    
    m_WebSiteLink.SubclassWindow(GetDlgItem(IDC_SITELINK));
    m_WebSiteLink.m_dwExtendedStyle |= HLINK_UNDERLINEHOVER; 

    m_GoogleCodeLink.SubclassWindow(GetDlgItem(IDC_GOOGLECODELINK));
    m_GoogleCodeLink.m_dwExtendedStyle |= HLINK_UNDERLINEHOVER; 

    m_ReportBugLink.SubclassWindow(GetDlgItem(IDC_FOUNDABUG));
    m_ReportBugLink.m_dwExtendedStyle |= HLINK_UNDERLINEHOVER; 
    m_ReportBugLink.SetLabel(TR("Found a bug? Send it to the author"));
    m_ReportBugLink.SetHyperLink(_T("https://github.com/zenden2k/image-uploader/issues"));

    m_EmailLink.SubclassWindow(GetDlgItem(IDC_AUTHORNAMELABEL));
    m_EmailLink.m_dwExtendedStyle |= HLINK_UNDERLINEHOVER;
    m_EmailLink.SetLabel((Lang.GetLanguageName() == _T("Russian") ? U2W("\xD0\xA1\xD0\xB5\xD1\x80\xD0\xB3\xD0\xB5\xD0\xB9\x20\xD0\xA1\xD0\xB2\xD0\xB8\xD1\x81\xD1\x82\xD1\x83\xD0\xBD\xD0\xBE\xD0\xB2")
        : _T("Sergey Svistunov")) + CString(_T(" (zenden2k@yandex.ru)")));
    m_EmailLink.SetHyperLink(_T("mailto:zenden2k@yandex.ru"));

    m_Documentation.SubclassWindow(GetDlgItem(IDC_DOCUMENTATION));
    m_Documentation.m_dwExtendedStyle |= HLINK_UNDERLINEHOVER | HLINK_COMMANDBUTTON; 
    m_Documentation.SetLabel(TR("Documentation"));

    auto ver = AppParams::instance()->GetAppVersion();

    CString memoText;
    
    memoText += TR("Settings file path:") + CString(_T("\r\n"))+ Settings.getSettingsFileName() + _T("\r\n\r\n");
    memoText += TR("Thanks to:") + CString("\r\n\r\n");
    memoText += TR("Contributors:") + CString("\r\n");
    memoText += L"arhangelsoft\thttps://github.com/arhangelsoft\r\nTahir Yilmaz\thttps://github.com/thrylmz\r\nAlex_Qwerty\r\n\r\n";

   
    CString translatorName = TR("translator_name");
    if ( translatorName == "translator_name") {
        translatorName.Empty();
    }
    if ( !translatorName.IsEmpty() ) {
        memoText += TR("Translators:") + CString("\r\n");

        CString trans;
        trans.Format(TR("%s translation:"),Lang.GetLanguageName());
        memoText += L"\r\n"+ trans + L"\r\n"+ translatorName +L"\r\n\r\n";
    } 

    memoText += TR("Beta-testers:")+ CString("\r\n");
    memoText += L"Graf, CKA3O4H1K, Geslot, Alex_Qwerty\r\n\r\n";

    //memoText += CString(T_R("Thanks to the authors of the following open-source libraries:"))+L"\r\n\r\n";
    memoText += CString(TR("Thanks to the authors of the following open-source libraries:"))+L"\r\n\r\n";
    
    memoText += CString(L"WTL")+ "\t\thttp://sourceforge.net/projects/wtl/\r\n";
    memoText += CString(L"libcurl")+ "\t\thttp://curl.haxx.se/libcurl/\r\n";
//#ifdef USE_OPENSSL
    memoText += CString(L"openssl") +"\t\thttps://www.openssl.org\r\n";
//#endif
    memoText += CString(L"zlib") +"\t\thttp://www.zlib.net\r\n";
    memoText += CString(L"squirrel") +"\t\thttp://squirrel-lang.org\r\n";
    memoText += CString(L"sqrat") +"\t\thttp://scrat.sourceforge.net\r\n";
    memoText += CString(L"ffmpeg") +"\t\thttps://www.ffmpeg.org\r\n";    
    memoText += CString(L"MediaInfo") +"\thttps://mediaarea.net/\r\n";
    memoText += CString(L"pcre") +"\t\thttp://www.pcre.org\r\n";
    memoText += CString(L"pcre++") +"\t\thttp://www.daemon.de/PCRE\r\n";
    
    memoText += CString(L"tinyxml") +"\t\thttp://sourceforge.net/projects/tinyxml/\r\n";
    memoText += CString(L"gumbo parser") +"\thttps://github.com/google/gumbo-parser\r\n";
    memoText += CString(L"gumbo-query") +"\thttps://github.com/lazytiger/gumbo-query\r\n";
    memoText += CString(L"glog") +"\t\thttps://github.com/google/glog\r\n";
    memoText += CString(L"libwebp") +"\t\thttps://github.com/webmproject/libwebp\r\n";


    memoText += CString(L"minizip") +"\t\thttp://www.winimage.com/zLibDll/minizip.html\r\n";
    memoText += CString(L"jsoncpp") +"\t\thttps://github.com/open-source-parsers/jsoncpp\r\n";
    memoText += CString(L"Boost") +"\t\thttp://www.boost.org\r\n";
    memoText += CString(L"FastDelegate") +"\thttp://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible\r\n";
    memoText += CString(L"QColorQuantizer") +"\thttp://www.codeguru.com/cpp/g-m/gdi/gdi/article.php/c3677/Better-GIFs-with-Octrees.htm\r\n";
    memoText += CString(L"WTL Browser") +"\thttp://www.codeproject.com/Articles/7147/WTL-Browser\r\n";
    memoText += CString(L"CRegistry") +"\t\thttp://www.codeproject.com/Articles/19/Registry-Class\r\n";
    memoText += CString(L"TParser") +"\t\thttps://rsdn.ru/article/files/Classes/tparser.xml\r\n";
    memoText += CString(L"CWinHotkeyCtrl") +"\thttps://rsdn.ru/article/controls/WinHotkeyCtrl.xml\r\n";
    memoText += CString(L"UTF-8 CPP") +"\thttp://sourceforge.net/projects/utfcpp/\r\n";
    memoText += CString(L"CUnzipper") +"\thttp://www.codeproject.com/Articles/4288/Win-Wrapper-classes-for-Gilles-Volant-s-Zip-Unzi\r\n";
    memoText += CString(L"CThread") +"\t\thttp://www.viksoe.dk/code/thread.htm\r\n";
    memoText += CString(L"CPropertyList") +"\thttp://www.viksoe.dk/code/propertylist.htm\r\n";
    memoText += CString(L"uriparser") + "\t\thttp://uriparser.sourceforge.net/\r\n";
    memoText += CString(L"GDI+ helper") +"\thttp://www.codeproject.com/Articles/4969/GDI-and-MFC-memory-leak-detection\r\n";
    memoText += CString(L"xbbcode") +"\t\thttps://github.com/patorjk/Extendible-BBCode-Parser\r\n";
    memoText += CString(L"entities.c") + "\t\thttps://bitbucket.org/cggaertner/cstuff/\r\n";
    memoText += CString(L"base64") + "\t\thttps://github.com/aklomp/base64/\r\n";
    memoText += CString(L"Mega SDK") + "\thttps://github.com/meganz/sdk\r\n";
    memoText += CString(L"Crypto++") + "\t\thttps://www.cryptopp.com/\r\n";
    memoText += CString(L"c-ares") + "\t\thttps://c-ares.haxx.se/\r\n";
    memoText += CString(L"libuv") + "\t\thttps://github.com/libuv/libuv\r\n";
            
    memoText += CString(_T("Resources:\r\n")) +
        _T("famfamfam icons\thttp://www.famfamfam.com/lab/icons/\r\n\r\n");
    memoText += CString(L"Build date: ") + CString(ver->BuildDate.c_str()) + _T("\r\n");
    memoText +=  CString(L"Built with: \r\n") + CString(BOOST_COMPILER) +  _T("\r\n");
    memoText +=  CString(L"Target platform: ") + BOOST_PLATFORM + _T(" (") + WinUtils::IntToStr(sizeof(void*) * CHAR_BIT) + _T(" bit)\r\n");
    memoText += TR("Libcurl version:")+ CString("\r\n");
    memoText +=  IuCoreUtils::Utf8ToWstring( curl_version()).c_str() + CString("\r\n\r\n");
    CString versionLabel;
    versionLabel.Format(_T("%s version:"), _T("Boost"));
    memoText += versionLabel + CString("\r\n");
    CString boostVersion = CString(BOOST_LIB_VERSION);
    boostVersion.Replace(L'_', L'.');
    memoText += boostVersion  + L"\r\n\r\n";
    int webpVersion = WebPGetDecoderVersion();
    CString webpVersionStr;
    webpVersionStr.Format(_T("%u.%u.%u"), (webpVersion >> 16) & 0xff, (webpVersion >> 8) & 0xff, webpVersion & 0xff);

    memoText += CString(L"Webp: v") + webpVersionStr + L"\r\n\r\n";
    /*if ( Settings.IsFFmpegAvailable() ) { // Can't determine actual ffmpeg version
        memoText += TR("FFmpeg version:")+ CString("\r\n");
        memoText += FFMPEG_VERSION + CString("\r\n");
    }*/

    if (MediaInfoHelper::IsMediaInfoAvailable()) {
        memoText += CString(L"MediaInfo.DLL path:\r\n") + MediaInfoHelper::GetLibraryPath() + _T("\r\n")+
            MediaInfoHelper::GetLibraryVersion() + L"\r\n\r\n";
    }
    SetDlgItemText(IDC_MEMO, memoText);
   
    CString buildInfo;
    buildInfo.Format(_T("Build %d"), ver->Build);
#ifdef USE_OPENSSL
    buildInfo += _T(" (with OpenSSL) ");
#endif
    buildInfo  +=  CString(_T("\r\n(")) + ver->BuildDate.c_str() + _T(")");

    CString text = CString(TR("v")) + ver->FullVersion.c_str();

    SetDlgItemText(IDC_CURLINFOLABEL, text);
    SetDlgItemText(IDC_IMAGEUPLOADERLABEL, buildInfo);
    CenterWindow(GetParent());

    TRC(IDC_AUTHORLABEL, "Author:");
    TRC(IDC_WEBSITELABEL, "Website:");
    
    SetWindowText(TR("About Image Uploader"));
    return TRUE;
}

LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    EndDialog(wID);
    return 0;
}

LRESULT CAboutDlg::OnDocumentationClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ShellExecute(0,L"open",WinUtils::GetAppFolder()+"Docs\\index.html",0,WinUtils::GetAppFolder()+"Docs\\",SW_SHOWNORMAL);
    return 0;
}

LRESULT CAboutDlg::OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
    HDC hdcStatic = (HDC) wParam;
    HWND hwndStatic = WindowFromDC(hdcStatic);

    if ( hwndStatic != GetDlgItem(IDC_IMAGEUPLOADERLABEL) ) {
        return 0;
    }
    COLORREF textColor = GetSysColor(COLOR_WINDOWTEXT);
    if ( textColor == 0 ) {
        SetTextColor(hdcStatic, RGB(100,100,100));
        SetBkColor(hdcStatic, GetSysColor(COLOR_BTNFACE));
    }
    

    return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_BTNFACE));
}
