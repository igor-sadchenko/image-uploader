clientId <- "62e3f09c348d5696c9805ef1f200d5663b026283";
clientSecret <- "8acf539a452aae12f0d8156140f9cc3d05bd7cf6";
redirectUri <- "https://oauth.vk.com/blank.html";
redirectUrlEscaped <- "https:\\/\\/oauth\\.vk\\.com\\/blank\\.html";
code <- "";
token <- "";
login <- "";
apiKey <- "";
//Docs: https://dev.bitly.com/get_started.html


function BeginLogin() {
	try {
		return Sync.beginAuth();
	}
	catch ( ex ) {
	}
	return true;
}

function EndLogin() {
	try {
		return Sync.endAuth();
	} catch ( ex ) {
		
	}
	return true;
}


function OnUrlChangedCallback(data) {
	local reg = CRegExp("^" +redirectUrlEscaped, "");
	if ( reg.match(data.url) ) {
		local br = data.browser;
		local regError = CRegExp("error=([^&]+)", "");
		if ( regError.match(data.url) ) {
			WriteLog("warning", regError.getMatch(1));
		} else {
			local regToken = CRegExp("code=([^&]+)", "");
			if ( regToken.match(data.url) ) {
				code = regToken.getMatch(1);
			}	
		}
		br.close();
	}
}

function _DoLogin() {
    token = ServerParams.getParam("token");
	
	if ( token != ""){
		return 1;
	}
    
    local browser = CWebBrowser();
	browser.setTitle("Bit.ly authorization");
	browser.setOnUrlChangedCallback(OnUrlChangedCallback, null);
	
	local url = "https://bitly.com/oauth/authorize?" + 
			"client_id=" + clientId  + 
			"&redirect_uri=" + nm.urlEncode(redirectUri);

	browser.navigateToUrl(url);
	browser.showModal();
    
    if (code != ""){
        nm.setUrl("https://api-ssl.bitly.com/oauth/access_token");
        nm.addQueryParam("client_id", clientId);
        nm.addQueryParam("client_secret", clientSecret);
        nm.addQueryParam("code", code);
        nm.addQueryParam("redirect_uri", redirectUri);
        nm.addQueryHeader("Accept", "application/json");
        nm.doPost("");
        if (nm.responseCode() == 200){
            local t = ParseJSON(nm.responseBody());
            token = t.access_token ;
            ServerParams.setParam("token", token);
            login = t.login;
            ServerParams.setParam("actual_login", login);
            apiKey = t.apiKey;
            return 1;
        }
    }
    return 0;
}

function DoLogin() {
	if (!BeginLogin() ) {
		return false;
	}
	local res = _DoLogin();
	
	EndLogin();
	return res;
}

function ShortenUrl(url, options)
{	
    if(!DoLogin()) {
        return 0;
    }
    
    nm.setUrl("https://api-ssl.bitly.com/v4/shorten");
    nm.addQueryHeader("Content-Type", "application/json");
    nm.addQueryHeader("Authorization", "Bearer " + token );
    local req = {
        group_guid = "",
        domain = "bit.ly",
        long_url = url
    };
    nm.doPost(ToJSON(req));
    if (nm.responseCode() == 200 || nm.responseCode() == 201) {
        local t = ParseJSON(nm.responseBody());
        if ( "link" in t) {
             options.setDirectUrl(t.link);
             return 1;
        }
    }
    
	return 0;
}


function GetServerParamList()
{
	return {
        token = "Token"
	};
}