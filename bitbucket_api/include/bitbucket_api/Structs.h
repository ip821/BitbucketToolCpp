#pragma once

#include <string>

struct DiffStat
{
    int size{};
};

struct Link
{
    std::string href{};
};

struct User
{
    std::string display_name{};
    std::string uuid{};
};

struct Workspace
{
    std::string slug{};
};

struct WorkspaceAccess
{
    Workspace workspace{};
};

struct Repository
{
    Workspace workspace{};
    std::string full_name{};
    std::string slug{};

    std::string GetRepoKey() const;
};
