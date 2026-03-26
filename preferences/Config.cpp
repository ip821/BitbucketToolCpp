#include "Config.h"

#include <wx/config.h>
#include <nlohmann/json.hpp>

const wxString keyRepositories = "repositories";

wxArrayString Config::GetRepositories()
{
    wxConfig config;
    const auto strValue = config.Read(keyRepositories);
    const auto jObject = nlohmann::json::parse(strValue.ToUTF8().data());
    const auto values = jObject["values"];

    wxArrayString repositories;
    for (const auto& value : values)
    {
        repositories.Add(value.get<std::string>());
    }
    return repositories;
}

void Config::SetRepositories(const wxArrayString& repositories)
{
    wxConfig config;
    nlohmann::json jObject;
    jObject["values"] = repositories;
    config.Write(keyRepositories, wxString(jObject.dump()));
}
