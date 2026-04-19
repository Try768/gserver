#pragma once
#include "core/forward.hpp"
#include "../common.hpp"
#include "../tile/component.hpp"
#include "../entity/component.hpp"
#include "../entity/entity.hpp"
#include "core/tile/tile.hpp"
#include "../entity/player/list.hpp"
#include "core/eventListener/eventTable.hpp"
constexpr size_t maxplayers=1024;
constexpr size_t maxtiles=1024*10;
constexpr size_t maxentities=1024*10;
class World;
class Registry{
    private:
    friend class chunkmap;
    friend class EntityManager;
    friend class World;
        std::unordered_map<std::string,TileComponent::Internal> tiles;
        std::unordered_map<std::string,EntityComponent::Internal> entities;
        zt::callback::EntityEventListener eel;
        zt::callback::PlayerEventListener pel;
        zt::callback::TileEventListener tel;
    const  std::array<unsigned char,8> sign ={10,20,80,90,40,20,222,0};
    public:
    friend class Entity;
    zt::callback::EntityEventListener& getEEL(){return eel;}
    zt::callback::TileEventListener& getTEL(){return tel;}
    zt::callback::PlayerEventListener& getPEL(){return pel;}
    const zt::callback::EntityEventListener& getEEL()const {return eel;}
    const zt::callback::TileEventListener& getTEL()const {return tel;}
    const zt::callback::PlayerEventListener& getPEL()const {return pel;}
    //friend void executionProses::earlyExecution();
    Registry(){};
    public:
    bool entityregister(const std::string& name,Var_component_Object&& cco){
        if(!entities.count(name))return false;
        entities.try_emplace(name,std::move(cco));
    }
    bool tileregister(std::string name,Var_component_Object&& cco){
        if(!tiles.count(name))return false;
        tiles.try_emplace(name,std::move(cco));
    }
    
    const IndeksEntityComponent getEntityComponentByName(const std::string& name){
        auto itc=entities.find(name);
        if(itc!=entities.end()){
            return IndeksEntityComponent((itc->second),name);
        }
        return IndeksEntityComponent();
    }
     const IndeksTileComponent getTileComponentByName(std::string name){
         auto itc=tiles.find(name);
        if(itc!=tiles.end()){
            return IndeksTileComponent((itc->second));
        }
        return IndeksTileComponent();
    }
    const auto& getEntities(){
        return entities;
    }
    using InTileComponent=zt::Internal::util::optionalRef<TileComponent>;
    InTileComponent getTilebyname(std::string name){
        namespace utility=zt::Internal::util;
        auto itc = tiles.find(name);
        if(itc==tiles.end())return utility::optionalRef<TileComponent>();
        return utility::optionalRef<TileComponent>(TileComponent(itc->second));
    }
    using InEntityComponent=zt::Internal::util::optionalRef<EntityComponent>;
    InEntityComponent getentitybyname(std::string name){
        namespace utility=zt::Internal::util;
        auto itc = entities.find(name);
        if(itc==entities.end())return InEntityComponent();
        return InEntityComponent(EntityComponent(name,itc->second));
    }
    const auto& getTiles(){
        return tiles;
    }
};

