#pragma once

#include "wx/arrstr.h"

class Config
{
public:
    static wxArrayString GetRepositories();
    static void SetRepositories(const wxArrayString& repositories);
};
