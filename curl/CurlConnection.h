#pragma once
#include <expected>
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

using CurlResult = std::expected<Success, Error>;

class CurlConnection
{
    CurlHandle m_handle;

    wxString GetErrorMessage(CURLcode rc) const;

public:
    CurlConnection();
    ~CurlConnection();

    [[nodiscard]] CurlResult HttpGet(const wxString& url) const;
};
