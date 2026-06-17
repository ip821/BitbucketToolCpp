#include "wx/base64.h"
#include "wx/wx.h"
#include "wx/log.h"
#include "wx/secretstore.h"

#include "Credentials.h"

const wxString SecretStoreAppName = "PRToolForBitbucketCpp";

wxString Credentials::GetCredentialsBase64()
{
    const auto credentials = GetCredentials();
    if (!credentials.has_value())
    {
        wxLogError("Failed to load credentials");
        return "";
    }

    const auto& [strEmail, strPassword] = credentials.value();

    const wxString auth = strEmail + ":" + strPassword;
    const wxCharBuffer utf8 = auth.ToUTF8();
    const wxString encoded = wxBase64Encode(utf8.data(), utf8.length());

    return encoded;
}

std::optional<CredentialsData> Credentials::GetCredentials()
{
    wxString email;
    wxSecretValue password;

    if (const auto store = wxSecretStore::GetDefault();
        store.IsOk())
    {
        if (store.Load(SecretStoreAppName, email, password))
            return CredentialsData{email, password.GetAsString()};
    }

    return std::nullopt;
}

void Credentials::SetCredentials(const wxString& userName, const wxString& password)
{
    if (auto store = wxSecretStore::GetDefault();
        store.IsOk())
    {
        wxSecretValue passwordSecretValue(password);
        store.Save(SecretStoreAppName, userName, passwordSecretValue);
    }
}
