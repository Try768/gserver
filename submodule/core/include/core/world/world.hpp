#pragma once
#include "../register/register.hpp"
#include "room.hpp"
class World{
    using ID=unsigned long long;
    IDMaker<ID> id;
    std::unordered_map<ID,Room> rooms;
    World(void (*)()){

    }
};