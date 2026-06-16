#include <wx/webrequest.h>

#include "HttpConnection.h"
#include "../preferences/Credentials.h"

HttpConnection::HttpConnection()
{
}

HttpConnection::~HttpConnection()
{
}

HttpResult HttpConnection::HttpGet(const wxString& url) const
{
    auto requestUrl = url;

    auto redirectCount = 3;
    while (redirectCount > 0)
    {
        wxWebRequestSync webRequest = wxWebSessionSync::GetDefault().CreateRequest(requestUrl);
        webRequest.SetMethod(wxS("GET"));

        const auto credentialsBase64 = Credentials::GetCredentialsBase64();
        webRequest.SetHeader(wxS("Accept"), wxS("application/json"));
        webRequest.SetHeader(wxS("Authorization"), wxS("Basic ") + wxString::FromUTF8(credentialsBase64));

        const auto result = webRequest.Execute();
        const auto response = webRequest.GetResponse();

        if (!result)
        {
            const auto responseUrl = response.GetURL();
            if (response.GetStatus() == 404 && responseUrl != requestUrl)
            {
                redirectCount--;
                requestUrl = responseUrl;
                continue;
            }
            return std::unexpected(Error{result.error});
        }

        const auto errorMessage = response.GetStatusText();
        if (const auto statusCode = response.GetStatus(); statusCode != 200)
        {
            return std::unexpected(Error{errorMessage});
        }

        return Success{response.AsString()};
    }

    return std::unexpected(Error{wxS("Redirect count is exceeded.")});
}
