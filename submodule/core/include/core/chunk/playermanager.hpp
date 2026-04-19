#pragma once
#include "entitymanager.hpp"
#include "core/internal/internal.hpp"
#include"core/entity/player/player.hpp"
#include "core/internal/filemanager.hpp"
#include <chrono>

#include <optional>
class PlayerComponent;
class PlayerManager
{
private:
    /* data */
    using ID =unsigned long long;
    IDMaker<ID> idmaker;
    Room& room;
    PlayerComponent& component;
    fsmanager::db::LMDB internal_db;
    std::unordered_map<std::string,ID> used_name;
    std::unordered_map<std::string,unsigned int> name_count;
    std::unordered_map<XUID,ID> used_XUID;
    std::unordered_map<ID,PlayerData> playerlist;
    PlayerData loadData(XUID);
    void unloadData(PlayerData&,XUID);
    public:
    std::optional<ID> addPlayer(std::string name,const XUID& xuid);
    std::optional<Player> getPlayer(ID playerId){
        auto itc=playerlist.find(playerId);
        if(itc==playerlist.end())return std::nullopt;
        return Player(itc->second,playerId);
    }
    std::optional<Player> getPlayer(const std::string& playername){
        
        auto itc=used_name.find(playername);
        if(itc==used_name.end())return std::nullopt;
        return getPlayer(itc->second);
    }
    std::optional<Player> getPlayer(std::string&& playername){
        auto itc=used_name.find(playername);
        if(itc==used_name.end())return std::nullopt;
        return getPlayer(itc->second);
    }
    std::optional<Player> getPlayer(const XUID& playerXuid){
        auto itc=used_XUID.find(playerXuid);
        if(itc==used_XUID.end())return std::nullopt;
        return getPlayer(itc->second);
    }
    using time_point=std::chrono::steady_clock::time_point;
    void simulate(Registry& reg, time_point time_pivot);
    const PlayerComponent& getComponent()const{return component;}
    PlayerManager(Room& room,PlayerComponent& component,const std::string& path_to_storage):room(room),
    component(component),internal_db(path_to_storage,1024*1024*50){}
    ~PlayerManager();
};
