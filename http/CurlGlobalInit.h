#pragma once

#include <curl/curl.h>

class CurlGlobalInit
{
public:
    explicit CurlGlobalInit()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~CurlGlobalInit()
    {
        curl_global_cleanup();
    }
};
