#pragma once
#include "forward.hpp"
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
class Registry{
    private:
    static    std::unordered_map<std::string,tilecomponent::Internal> tiles;
    static    std::unordered_map<std::string,EntityComponent::Internal> entities;
        const  std::array<unsigned char,8> sign ={10,20,80,90,40,20,222,0};
    public:
    friend class Entity;
   
    
    friend void executionProses::earlyExecution();
    static bool entityregister(const std::string& idname,const Var_component_Object&& CCO);
    static bool tileregister(std::string name,unsigned char sizeofComponent){
        if(!entities.count(name))return false;
       entities.try_emplace(name,sizeofComponent);
    }
    static bool addEntityCustomComponent(bool(*componentFunction)(EntityData&,const Var_component&),const std::string& componentName);
    static bool addTileCustomComponent(bool(*componentFunction)(EntityData&,const Var_component&),const std::string& componentName);
    public:
    static bool callEntityCustomComponent(const std::string&,EntityData& data,const Var_component& const_component);
    static bool callTileCustomComponent(const std::string&,EntityData& data,const Var_component& const_component);
    static const IndeksEntityComponent getEntityComponentByName(const std::string& name){
        auto itc=entities.find(name);
        if(itc!=entities.end()){
            return IndeksEntityComponent((itc->second),name);
        }
        return IndeksEntityComponent();
    }
    static const IndeksTileComponent getTileComponentByName(std::string name){
         auto itc=tiles.find(name);
        if(itc!=tiles.end()){
            return IndeksTileComponent((itc->second));
        }
        return IndeksTileComponent();
    }
    static const auto& getEntities(){
        return entities;
    }
    using InTileComponent=zt::Internal::util::optional<tilecomponent>;
    static const InTileComponent getTilebyname(std::string name){
        namespace utility=zt::Internal::util;
        auto itc = tiles.find(name);
        if(itc==tiles.end())return utility::optional<tilecomponent>();
        return utility::optional<tilecomponent>(tilecomponent(name,itc->second));
    }
    using InEntityComponent=zt::Internal::util::optional<EntityComponent>;
    static const InEntityComponent getentitybyname(std::string name){
        namespace utility=zt::Internal::util;
        auto itc = entities.find(name);
        if(itc==entities.end())return InEntityComponent();
        return InEntityComponent(EntityComponent(name,itc->second));
    }
    const auto& getTiles(){
        return tiles;
    }
};

