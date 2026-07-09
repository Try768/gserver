#pragma once
#include "shared/Util/string.hpp"
//shared folder didn`t use any submodule header
struct EntityMoveSnapshot{
    unsigned long long id;
    unsigned long long anim_id;
    unsigned int permstate;
    double x,y;//relative movement
    double time;
};
namespace Client{
    void sendEntityMove(const EntityMoveSnapshot& snapshot);
    
};
