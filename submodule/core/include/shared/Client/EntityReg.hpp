#pragma once
#include "shared/Util/string.hpp"
//shared folder didn`t use any submodule header
struct EntityRegisterSnapshot
{
    zt::string_view idname;
    unsigned long long id;
};void sendEntityRegister(const EntityRegisterSnapshot& snapshot);