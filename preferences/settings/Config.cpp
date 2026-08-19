#include "Config.h"

#include <bitbucket_api/StructsSerialization.h>
#include <nlohmann/json.hpp>
#include <wx/config.h>
#include <wx/wx.h>

const wxString keyRepositories = "repositories5";
const wxString keyHideChangesRequestedPullRequests = "hideChangesRequestedPullRequests";
const wxString keyUseSubmenusOnMenuOverflow = "useSubmenusOnMenuOverflow";
const wxString keyDisplayRepositoryNameLowercase = "displayRepositoryNameLowercase";

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

bool Config::GetHideChangesRequestedPullRequests()
{
    const wxConfig config;
    return config.ReadBool(keyHideChangesRequestedPullRequests, true);
}

void Config::SetHideChangesRequestedPullRequests(const bool hide)
{
    wxConfig config;
    config.Write(keyHideChangesRequestedPullRequests, hide);
}

bool Config::GetUseSubmenusOnMenuOverflow()
{
    const wxConfig config;
    return config.ReadBool(keyUseSubmenusOnMenuOverflow, true);
}

void Config::SetUseSubmenusOnMenuOverflow(const bool value)
{
    wxConfig config;
    config.Write(keyUseSubmenusOnMenuOverflow, value);
}

bool Config::GetDisplayRepositoryNameLowercase()
{
    const wxConfig config;
    return config.ReadBool(keyDisplayRepositoryNameLowercase, false);
}

void Config::SetDisplayRepositoryNameLowercase(bool value)
{
    wxConfig config;
    config.Write(keyDisplayRepositoryNameLowercase, value);
}
