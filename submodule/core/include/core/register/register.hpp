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
        zt::ManualObject<PlayerComponent::Internal> playerComponent;
    const  std::array<unsigned char,8> sign ={10,20,80,90,40,20,222,0};
    public:
    friend class Entity;
    inline zt::callback::EntityEventListener& getEEL(){return eel;}
    inline zt::callback::TileEventListener& getTEL(){return tel;}
    inline zt::callback::PlayerEventListener& getPEL(){return pel;}
    inline const zt::callback::EntityEventListener& getEEL()const {return eel;}
    inline const zt::callback::TileEventListener& getTEL()const {return tel;}
    inline const zt::callback::PlayerEventListener& getPEL()const {return pel;}
    //friend void executionProses::earlyExecution();
    Registry();
    public:
    ~Registry(){
        playerComponent.dtor();
    }
    inline void initPlayerComponent(Var_component_Object&& cco){

    }
    inline bool entityregister(const std::string& name,Var_component_Object&& cco){
        if(!entities.count(name))return false;
        entities.try_emplace(name,std::move(cco));
    }
    inline bool tileregister(std::string name,Var_component_Object&& cco){
        if(!tiles.count(name))return false;
        tiles.try_emplace(name,std::move(cco));
    }
    
    inline const IndeksEntityComponent getEntityComponentByName(const std::string& name)const{
        auto itc=entities.find(name);
        if(itc!=entities.end()){
            return IndeksEntityComponent((itc->second),name);
        }
        return IndeksEntityComponent();
    }
    inline const IndeksTileComponent getTileComponentByName(std::string name)const{
         auto itc=tiles.find(name);
        if(itc!=tiles.end()){
            return IndeksTileComponent((itc->second));
        }
        return IndeksTileComponent();
    }
    inline const auto& getEntities()const{
        return entities;
    }
    using InTileComponent=zt::Internal::util::OptionalRef<TileComponent>;
    InTileComponent getTilebyname(std::string name)const;
    using InEntityComponent=zt::Internal::util::OptionalRef<EntityComponent>;
    InEntityComponent getEntitybyname(std::string name)const;
    PlayerComponent getPlayerComponent()const;
    inline const auto& getTiles(){
        return tiles;
    }
};


