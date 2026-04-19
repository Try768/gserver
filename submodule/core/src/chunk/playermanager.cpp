#include "core/chunk/playermanager.hpp"
#include "core/entity/player/player.hpp"
#include "world/room.hpp"
#include "world/world.hpp"
#include "core/eventListener/etPlayer.hpp"
#include "core/eventListener/eventTable.hpp"

PlayerData PlayerManager::loadData(XUID xuid){
    std::vector<uint8_t> buffer=internal_db.get(std::string("Xuid:")
        +std::to_string(xuid.id1)
        +std::to_string(xuid.id2)
        +std::to_string(xuid.id3)
    );
    if(buffer.size()==0)return PlayerData();
    return PlayerData(buffer);
}
void PlayerManager::unloadData(PlayerData&data,XUID xuid){
    std::vector<uint8_t> buffer;
    data.dump(buffer);
    internal_db.put(std::string("Xuid:")
        +std::to_string(xuid.id1)
        +std::to_string(xuid.id2)
        +std::to_string(xuid.id3),buffer
    );
}
std::optional<PlayerManager::ID> PlayerManager::addPlayer(std::string name,const XUID& xuid){
    bool aprove;
    zt::event::player::params<zt::event::player::Type::PlayerConnect> params{aprove,xuid};
    auto listener =this->room.getWorld().getRegister().getPEL();
    bool emitdefault=true;
    listener.getBeforeEvent<zt::event::player::Type::PlayerConnect>().emit(params,emitdefault);
    if(emitdefault)listener.getComponent<zt::event::player::Type::PlayerConnect>().emit(params,component);
    listener.getAfterEvent<zt::event::player::Type::PlayerConnect>().emit(params);
    if(!aprove)return std::optional<ID>();;
    auto xitc=used_XUID.find(xuid);
    if(xitc!=used_XUID.end())return std::optional<ID>();
    ID id;
    idmaker.getID(id);
    used_XUID.emplace(xuid,id);
    auto ntc=name_count.find(name);
    if(ntc==name_count.end()){
        name_count.emplace(name,1);
    }else{
        ntc->second++;
        name+=std::string(".")+std::to_string(ntc->second);
    }
    used_name.emplace(name,id);
    auto [itc,stat]=playerlist.emplace(id,loadData(xuid));
    
    zt::event::player::params<zt::event::player::Type::PlayerSpawn> spawnparams{Player(itc->second,itc->first)};//note
    auto listener =this->room.getWorld().getRegister().getPEL();
    bool emitdefault=true;
    listener.getBeforeEvent<zt::event::player::Type::PlayerSpawn>().emit(spawnparams,emitdefault);
    if(emitdefault)listener.getComponent<zt::event::player::Type::PlayerSpawn>().emit(spawnparams,component);
    listener.getAfterEvent<zt::event::player::Type::PlayerSpawn>().emit(spawnparams);
}