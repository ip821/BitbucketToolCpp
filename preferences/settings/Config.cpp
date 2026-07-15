#include "Config.h"

#include <bitbucket_api/StructsSerialization.h>
#include <nlohmann/json.hpp>
#include <wx/config.h>
#include <wx/wx.h>

const wxString keyRepositories = "repositories5";

std::vector<Repository> Config::GetRepositories()
{
    const wxConfig config;

    if (const auto jsonString = config.Read(keyRepositories);
        !jsonString.IsEmpty())
    {
        try
        {
            const auto jObject = nlohmann::json::parse(jsonString.ToUTF8().data());
            return jObject.get<std::vector<Repository> >();
        }
        catch (nlohmann::json::exception&)
        {
            return {};
        }
    }

    return {};
}

void Config::SetRepositories(const std::vector<Repository>& repositories)
{
    wxConfig config;
    const nlohmann::json jObject = repositories;
    config.Write(keyRepositories, wxString(jObject.dump()));
}
