#pragma once

#include <vector>
#include "bitbucket_api/Structs.h"

class Config
{
public:
    static std::vector<Repository> GetRepositories();
    static void SetRepositories(const std::vector<Repository>& repositories);
    static bool GetHideChangesRequestedPullRequests();
    static void SetHideChangesRequestedPullRequests(bool hide);
};
