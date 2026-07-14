#include "bitbucket_api/Structs.h"

#include <format>
#include <string>

std::string Repository::GetRepoKey() const
{
    return std::format("{0}/{1}", workspace.slug, slug);
}
