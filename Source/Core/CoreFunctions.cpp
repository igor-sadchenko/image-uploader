#include "CoreFunctions.h"

#include "Core/Settings.h"
#include "Core/Network/NetworkClient.h"
#ifdef _WIN32
#include "DefaultProxyProvider.h"
#endif

namespace CoreFunctions {

std::unique_ptr<NetworkClient> createNetworkClient() {
    std::unique_ptr<NetworkClient> cl(new NetworkClient());
    ConfigureProxy(cl.get());
    return cl;
}

void ConfigureProxy(NetworkClient* nm)
{
#ifdef _WIN32
    if (Settings.ConnectionSettings.UseProxy == ConnectionSettingsStruct::kSystemProxy) {
        nm->setProxyProvider(new DefaultProxyProvider());
    } else
#endif
    if (Settings.ConnectionSettings.UseProxy == ConnectionSettingsStruct::kUserProxy) {
        int ProxyTypeList[5] = { CURLPROXY_HTTP, CURLPROXY_SOCKS4, CURLPROXY_SOCKS4A, CURLPROXY_SOCKS5, CURLPROXY_SOCKS5_HOSTNAME };
        nm->setProxy(Settings.ConnectionSettings.ServerAddress, Settings.ConnectionSettings.ProxyPort,
                        ProxyTypeList[Settings.ConnectionSettings.ProxyType]);

        if (Settings.ConnectionSettings.NeedsAuth) {
            nm->setProxyUserPassword(Settings.ConnectionSettings.ProxyUser,
                Settings.ConnectionSettings.ProxyPassword);
        }
    }
    nm->setUploadBufferSize(Settings.UploadBufferSize);
}

}
