#include <format>
#include <wx/webrequest.h>
#include <cpp_utils/wx_string_format.h>

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
        const auto responseBody = response.AsString();

        if (!result)
        {
            const auto responseUrl = response.GetURL();
            if (response.GetStatus() == 404 && responseUrl != requestUrl)
            {
                // On macOS redirection occurs without attaching proper Authentication headers to the forthcoming requests
                // So I repeat the request to the redirected URL manually
                redirectCount--;
                requestUrl = responseUrl;
                continue;
            }
        }

        if (!result)
        {
            const auto httpStatus = response.GetStatus();
            const auto errorMessage = std::format(wxS("{}: {}"), result.error, HttpStatusToString(httpStatus));
            return std::unexpected(Error{errorMessage, responseBody});
        }

        return Success{responseBody};
    }

    return std::unexpected(Error{wxS("Redirect count is exceeded."), {}});
}

wxString HttpConnection::HttpStatusToString(int code)
{
    switch (code)
    {
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 409: return "Conflict";
        case 429: return "Too Many Requests";
        case 500: return "Internal Server Error";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        default:
            return wxString::Format("HTTP %d", code);
    }
}