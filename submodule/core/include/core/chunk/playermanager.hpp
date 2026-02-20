#pragma once
#include "entitymanager.hpp"
#include "core/entity/player/player.hpp"
class PlayerManager
{
private:
    /* data */
    using ID =unsigned long long;
    static std::unordered_map<std::string,ID> used_name;
    static std::unordered_map<ID,PlayerData> playerlist;
public:
    PlayerManager(/* args */);
    ~PlayerManager();
};

PlayerManager::PlayerManager(/* args */)
{
}

PlayerManager::~PlayerManager()
{
}
