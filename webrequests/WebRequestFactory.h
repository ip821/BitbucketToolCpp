#pragma once

#include <format>
#include <wx/webrequest.h>
#include "wx/base64.h"

#include "../Constants.h"

class WebRequestFactory
{
public:
    static wxWebRequest CreateWebRequest(wxEvtHandler* pEvtHandler, const wxString& relativeUrl, int id = wxID_ANY)
    {
        wxString strEmail;
        wxSecretValue strPassword;

        const auto store = wxSecretStore::GetDefault();
        if (store.IsOk())
        {
            if (!store.Load(SecretStoreAppName, strEmail, strPassword))
                wxLogError("Failed to load credentials from secret store");
        }

        const wxString auth = strEmail + ":" + strPassword.GetAsString();
        const wxCharBuffer utf8 = auth.ToUTF8();
        const wxString encoded = wxBase64Encode(utf8.data(), utf8.length());

        const std::string strFormattedUrl = std::format("https://api.bitbucket.org/2.0/{}", relativeUrl.ToStdString());

        wxWebRequest request = wxWebSession::GetDefault().CreateRequest(pEvtHandler, wxString::FromUTF8(strFormattedUrl), id);

        request.SetHeader("Authorization", "Basic " + encoded);
        request.SetHeader("Accept", "application/json");

        return request;
    }
};
