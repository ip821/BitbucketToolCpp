#pragma once

#include <vector>
#include "bitbucket_api/Structs.h"
#include "wx/event.h"

class Config
{
public:
    static std::vector<Repository> GetRepositories();
    static void SetRepositories(const std::vector<Repository>& repositories);
    static bool GetHideChangesRequestedPullRequests();
    static void SetHideChangesRequestedPullRequests(bool hide);
    static bool GetUseTwoColumnLayout();
    static void SetUseTwoColumnLayout(bool value);
    static bool GetDisplayRepositoryNameLowercase();
    static void SetDisplayRepositoryNameLowercase(bool value);
};
