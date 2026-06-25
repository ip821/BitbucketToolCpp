#include <format>

#include "HttpConnection.h"

#include "CurlSList.h"
#include "../preferences/Credentials.h"

HttpResult HttpConnection::HttpGet(const wxString& url) const
{
    const auto credentialsBase64 = Credentials::GetCredentialsBase64();

    CurlSList headersList;
    headersList.Append("Accept: application/json");
    headersList.Append("Authorization: Basic " + credentialsBase64);

    const CurlHandle curlHandle;
    const auto curl = curlHandle.GetHandle();

    const auto headers = headersList.Get();
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_URL, url.ToUTF8().data());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    std::string responseBody;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](const char* ptr, const size_t size, const size_t nmemb, void* userdata)
                     {
                     auto* out = static_cast<std::string*>(userdata);
                     out->append(ptr, size * nmemb);
                     return size * nmemb;
                     });

    if (const CURLcode rc = curl_easy_perform(curl);
        rc != CURLE_OK)
    {
        return CreateHttpResultFromCurlErrorCode(rc, responseBody);
    }

    long httpStatusCode{};
    if (const CURLcode infoRc = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);
        infoRc != CURLE_OK)
    {
        return CreateHttpResultFromCurlErrorCode(infoRc, responseBody);
    }

    if (httpStatusCode < 200 || httpStatusCode >= 300)
    {
        return std::unexpected(Error{GetHttpStatusMessage(httpStatusCode), responseBody});
    }

    return Success{wxString::FromUTF8(responseBody)};
}

HttpResult HttpConnection::CreateHttpResultFromCurlErrorCode(const CURLcode rc, const std::string& responseBody)
{
    const wxString strError = wxString::FromUTF8(curl_easy_strerror(rc));
    return std::unexpected(Error{strError, responseBody});
}

wxString HttpConnection::GetHttpStatusMessage(long code)
{
    switch (code)
    {
        case 400: return wxS("Bad Request");
        case 401: return wxS("Unauthorized");
        case 403: return wxS("Forbidden");
        case 404: return wxS("Not Found");
        case 409: return wxS("Conflict");
        case 429: return wxS("Too Many Requests");
        case 500: return wxS("Internal Server Error");
        case 502: return wxS("Bad Gateway");
        case 503: return wxS("Service Unavailable");
        case 504: return wxS("Gateway Timeout");
        default:
            return wxString::Format("HTTP status code %ld", code);
    }
}
