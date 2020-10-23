#ifndef IU_CORE_DEFAULTPROXYPROVIDER_H
#define IU_CORE_DEFAULTPROXYPROVIDER_H

#include "atlheaders.h"
#include "Core/Network/NetworkClient.h"
#include <Winhttp.h>

class DefaultProxyProvider : public NetworkClient::ProxyProvider {
public:
    DefaultProxyProvider();
    ~DefaultProxyProvider();
    bool provideProxyForUrl(INetworkClient* client, const std::string& url) override;
protected:
    HINTERNET hInternet_;
    WINHTTP_CURRENT_USER_IE_PROXY_CONFIG myProxyConfig_;
    bool configObtained_;
    bool openWinHttpSession();
    void closeWinHttpSession();
    std::string extractProxyForUrlFromList(const std::string& list, const std::string& url);
    bool obtainProxyConfig();
    CString proxyForUrlErrorToString(DWORD errorCode) const;
};
#endif