#pragma once

#include "Repository.h"
#include "wx/arrstr.h"

class Config
{
public:
    static std::vector<Repository> GetRepositories();
    static void SetRepositories(const std::vector<Repository>& repositories);
};
