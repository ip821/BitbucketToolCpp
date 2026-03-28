#include "Config.h"

#include <wx/wx.h>
#include <wx/config.h>
#include <nlohmann/json.hpp>

#include "Repository.h"

const wxString keyRepositories = "repositories5";

std::vector<Repository> Config::GetRepositories()
{
    const wxConfig config;

    if (const auto jsonString = config.Read(keyRepositories);
        !jsonString.IsEmpty())
    {
        const auto jObject = nlohmann::json::parse(jsonString.ToUTF8().data());
        return jObject.get<std::vector<Repository> >();
    }

    return {};
}

void Config::SetRepositories(const std::vector<Repository>& repositories)
{
    wxConfig config;
    const nlohmann::json jObject = repositories;
    config.Write(keyRepositories, wxString(jObject.dump()));
}
