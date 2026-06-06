#pragma once

#include <optional>
#include <wx/string.h>

struct CredentialsData
{
    wxString userName;
    wxString password;
};

class Credentials {
public:
    static wxString GetCredentialsBase64();
    static std::optional<CredentialsData> GetCredentials();
    static void SetCredentials(const wxString& userName, const wxString& password);
};
