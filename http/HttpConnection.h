#pragma once
#include <expected>
#include <wx/string.h>

#include "CurlHandle.h"

struct Success
{
    wxString responseBody;
};

struct Error
{
    wxString message;
    wxString responseBody;
};

using HttpResult = std::expected<Success, Error>;

class HttpConnection
{
    CurlHandle m_handle;

public:
    HttpConnection() = default;
    ~HttpConnection() = default;

    [[nodiscard]] HttpResult HttpGet(const wxString& url) const;
    static wxString GetHttpStatusMessage(long code);
};
