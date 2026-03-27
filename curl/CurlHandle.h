#pragma once

#include <curl/curl.h>

class CurlHandle
{
private:
    CURL* m_pCurl{};

public:
    CurlHandle()
    {
        m_pCurl = curl_easy_init();
    }

    ~CurlHandle()
    {
        if (m_pCurl)
            curl_easy_cleanup(m_pCurl);
    }

    CURL* GetHandle() const { return m_pCurl; }
};
