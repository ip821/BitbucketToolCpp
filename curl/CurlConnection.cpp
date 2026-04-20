#include "CurlConnection.h"

#include <curl/curl.h>

#include "CurlHandle.h"
#include "CurlSList.h"
#include "../preferences/Credentials.h"

CurlConnection::CurlConnection()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

CurlConnection::~CurlConnection()
{
    curl_global_cleanup();
}

CurlResult CurlConnection::HttpGet(const wxString& url) const
{
    const auto curl = m_handle.GetHandle();

    const auto credentialsBase64 = Credentials::GetCredentialsBase64();

    CurlSList headersList;
    headersList.Append("Accept: application/json");
    headersList.Append("Authorization: Basic " + credentialsBase64);

    const auto headers = headersList.Get();
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_URL, url.ToUTF8().data());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    std::string responseBody;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](const char* ptr, const size_t size, const size_t nmemb, void* userdata)
                     {
                     auto* out = static_cast<std::string*>(userdata);
                     out->append(ptr, size * nmemb);
                     return size * nmemb;
                     });

    if (const CURLcode rc = curl_easy_perform(curl);
        rc == CURLE_OK)
    {
        const wxString strBody = wxString::FromUTF8(responseBody);
        return Success{strBody};
    }
    else
    {
        const wxString strError = GetErrorMessage(rc);
        return std::unexpected(Error{strError});
    }
}

wxString CurlConnection::GetErrorMessage(const CURLcode rc) const
{
    if (rc == CURLE_HTTP_RETURNED_ERROR)
    {
        long httpCode = 0;
        curl_easy_getinfo(m_handle.GetHandle(), CURLINFO_RESPONSE_CODE, &httpCode);

        switch (httpCode)
        {
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 500:
            return "Internal Server Error";
        default:
            return wxString::Format("HTTP error: %ld", httpCode);
        }
    }
    return wxString::FromUTF8(curl_easy_strerror(rc));
}
