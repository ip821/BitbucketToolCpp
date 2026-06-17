#pragma once
#include <expected>
#include <wx/string.h>

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
public:
    HttpConnection();
    ~HttpConnection();

    [[nodiscard]] HttpResult HttpGet(const wxString& url) const;
    static wxString HttpStatusToString(int code);
};
