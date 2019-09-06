#include "WebUtils.h"

#include "Core/3rdpart/pcreplusplus.h"
#include "Func/WinUtils.h"

namespace WebUtils {
    bool DoesTextLookLikeUrl(CString& text) {
        if ( text.Left(4) == _T("www.") ) {
            text = _T("http://") + text;
            return true;
        }
        
        std::string utf8Text = WCstringToUtf8(text);
        pcrepp::Pcre regexp2("^(http|https|ftp)://", "imcu");
        if ( regexp2.search(utf8Text, 0) ) { 
            if ( regexp2.get_match_start() == 0) {
                //text = "http://" + text;
                return true;
            }
        } 

        pcrepp::Pcre regexp("^[A-Za-z0-9-]+(\\.[A-Za-z0-9-]+)*(\\.[A-Za-z]{2,})", "imcu");

        if ( regexp.search(utf8Text, 0) ) { 
            if ( regexp.get_match_start() == 0) {
                text = "http://" + text;
                return true;
            }
        } 

        return false;
    }
}
