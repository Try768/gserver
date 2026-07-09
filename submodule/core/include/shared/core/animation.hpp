#pragma once
#include "shared/Util/string.hpp"
//shared folder didn`t use any submodule header
struct EntityAnimationRegisterSnapshot
{
    zt::string_view idname;
    unsigned long long id;
};
void sendEntityAnimationRegister(const EntityAnimationRegisterSnapshot& snapshot);
#ifdef __UNIMPLEMENTED_CORE
void sendEntityAnimationRegister(const EntityAnimationRegisterSnapshot& snapshot){}
#endif