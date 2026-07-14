#include <format>

#include "CurlConnection.h"
#include "CurlSList.h"

HttpResult CurlConnection::HttpGet(const std::string& url, const std::optional<std::string>& authorizationHeaderValue) const
{
    CurlSList headersList;
    headersList.Append("Accept: application/json");

    if (authorizationHeaderValue.has_value())
        headersList.Append("Authorization: Basic " + authorizationHeaderValue.value());

    const CurlHandle curlHandle;
    const auto pCurl = curlHandle.Get();

    const auto pHeaders = headersList.Get();
    curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pHeaders);

    curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 10L);

    std::string responseBody;
    curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, +[](const char* ptr, const size_t size, const size_t nmemb, void* userdata)
                     {
                     auto* out = static_cast<std::string*>(userdata);
                     out->append(ptr, size * nmemb);
                     return size * nmemb;
                     });

    if (const CURLcode rc = curl_easy_perform(pCurl);
        rc != CURLE_OK)
    {
        return CreateHttpResultFromCurlErrorCode(rc, responseBody);
    }

    long httpStatusCode{};
    if (const CURLcode infoRc = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &httpStatusCode);
        infoRc != CURLE_OK)
    {
        return CreateHttpResultFromCurlErrorCode(infoRc, responseBody);
    }

    if (httpStatusCode < 200 || httpStatusCode >= 300)
    {
        return std::unexpected(Error{GetHttpStatusMessage(httpStatusCode), responseBody});
    }

    return Success{responseBody};
}

HttpResult CurlConnection::CreateHttpResultFromCurlErrorCode(const CURLcode rc, const std::string& responseBody)
{
    const auto strError = std::string(curl_easy_strerror(rc));
    return std::unexpected(Error{strError, responseBody});
}

std::string CurlConnection::GetHttpStatusMessage(long code)
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
            return std::format("HTTP status code {}", code);
    }
}
