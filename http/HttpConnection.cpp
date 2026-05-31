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
    wxWebRequestSync webRequest = wxWebSessionSync::GetDefault().CreateRequest(url);
    webRequest.SetMethod(wxS("GET"));

    const auto credentialsBase64 = Credentials::GetCredentialsBase64();
    webRequest.SetHeader(wxS("Accept"), wxS("application/json"));
    webRequest.SetHeader(wxS("Authorization"), wxS("Basic ") + wxString::FromUTF8(credentialsBase64));

    const auto result = webRequest.Execute();
    if (!result)
        return std::unexpected(Error{result.error});

    const auto response = webRequest.GetResponse();
    const auto errorMessage = response.GetStatusText();
    if (const auto statusCode = response.GetStatus(); statusCode != 200)
    {
        return std::unexpected(Error{errorMessage});
    }

    return Success{webRequest.GetResponse().AsString()};
}
