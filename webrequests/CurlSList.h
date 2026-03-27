#pragma once
#include "curl/curl.h"

class CurlSList
{
private:
    struct curl_slist* m_pSlist{};

public:
    CurlSList()
    {
    }

    ~CurlSList()
    {
        if (m_pSlist)
            curl_slist_free_all(m_pSlist);
    }

    void Append(wxString str)
    {
        m_pSlist = curl_slist_append(m_pSlist, str);
    }

    curl_slist* Get() const { return m_pSlist; }
};
