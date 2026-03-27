#pragma once
#include <variant>
#include <wx/string.h>

#include "CurlHandle.h"

struct Success
{
    wxString body;
};

struct Error
{
    wxString message;
};

using CurlResult = std::variant<Success, Error>;

class CurlConnection
{
    CurlHandle m_handle;

    wxString GetErrorMessage(CURLcode rc) const;

public:
    CurlConnection();
    ~CurlConnection();

    CurlResult HttpGet(const wxString& url) const;
};
