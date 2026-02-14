#pragma once
#include "forward.hpp"
#include "../common.hpp"
#include "../tile/component.hpp"
#include "../entity/component.hpp"
#include "../entity/entity.hpp"
#include "tile/tile.hpp"
#include "../entity/player/list.hpp"
constexpr size_t maxplayers=1024;
constexpr size_t maxtiles=1024*10;
constexpr size_t maxentities=1024*10;
class registry;
class registry{
    private:
    class Internal{
        friend class registry;
        static std::unordered_map<std::string,PlayerData> players;
        static std::unordered_map<std::string,tilecomponent::Internal> tiles;
        static std::unordered_map<std::string,EntityComponent::Internal> entities;
        //static std::unordered_map<std::string,bool(*)(EntityData&,const Const_component&)> customComponents;
        static inline const  std::array<unsigned char,8> sign ={10,20,80,90,40,20,222,0};
    };
    friend class Entity;
    //static std::unordered_map<std::string,unsigned long long> nametiles; 
    //static std::unordered_map<std::string,unsigned long long> nameentities; 
    static unsigned long long idcounte;
    static unsigned long long idcountt;
    friend void executionProses::earlyExecution();
    static bool entityregister(const std::string& idname,const Const_component_Object& CCO);
    static bool tileregister(std::string name,unsigned char sizeofComponent){
        if(!Internal::entities.count(name))return false;
       Internal::entities.try_emplace(name,sizeofComponent);
        idcountt++;
    }
    bool addEntityCustomComponent(bool(*componentFunction)(EntityData&,const Const_component&),const std::string& componentName);
    bool addTileCustomComponent(bool(*componentFunction)(EntityData&,const Const_component&),const std::string& componentName);
    public:
    bool callEntityCustomComponent(const std::string&,EntityData& data,const Const_component& const_component);
    bool callTileCustomComponent(const std::string&,EntityData& data,const Const_component& const_component);
    static const IndeksEntityComponent getEntityComponentByName(const std::string& name){
        auto itc=Internal::entities.find(name);
        if(itc!=Internal::entities.end()){
            return IndeksEntityComponent((itc->second),name);
        }
        return IndeksEntityComponent();
    }
    static const IndeksTileComponent getTileComponentByName(std::string name){
         auto itc=Internal::tiles.find(name);
        if(itc!=Internal::tiles.end()){
            return IndeksTileComponent((itc->second));
        }
        return IndeksTileComponent();
    }
    //flag:1=dgn tile,2=tanpa;
    struct dumping{
        static void players(std::vector<unsigned char>& keluaran);
        //static void tiles(std::vector<unsigned char>& keluaran);
        //static void entity(std::vector<unsigned char>& keluaran);
    };
    static void dump(std::vector<unsigned char>& keluaran);
    struct check{
        private:
        public:
        //static bool entities(const std::vector<unsigned char>& data,size_t& offset);
        //static bool tiles(const std::vector<unsigned char>& data,size_t& offset);
        static bool players(const std::vector<unsigned char>& data,size_t& offset);
    };
    static inline bool is_buffer_valid(const std::vector<unsigned char>& data,size_t& offset){
        using namespace zt::Internal;
        unsigned char len;
        unsigned long long arrsize;
        check::players(data,offset);
        return true;
    }
    static const auto& getEntities(){
        return Internal::entities;
    }
    using InTileComponent=zt::Internal::util::optional<tilecomponent>;
    static const InTileComponent getTilebyname(std::string name){
        namespace utility=zt::Internal::util;
        auto itc = Internal::tiles.find(name);
        if(itc==Internal::tiles.end())return utility::optional<tilecomponent>();
        return utility::optional<tilecomponent>(tilecomponent(name,itc->second));
    }
    using InEntityComponent=zt::Internal::util::optional<EntityComponent>;
    static const InEntityComponent getentitybyname(std::string name){
        namespace utility=zt::Internal::util;
        auto itc = Internal::entities.find(name);
        if(itc==Internal::entities.end())return InEntityComponent();
        return InEntityComponent(EntityComponent(name,itc->second));
    }
    static const auto& getTiles(){
        return Internal::tiles;
    }
    //this may throw errors
    struct parsing{
        static void players(const std::vector<unsigned char>& data,size_t& offset);
        //static void tiles(const std::vector<unsigned char>& data,size_t& offset);
        //static void entities(const std::vector<unsigned char>& data,size_t& offset);
    };
    static void parse(const std::vector<unsigned char>& data,size_t& offset){
        parsing::players(data,offset);
    }
    static void fullparse(const std::vector<unsigned char>& data,size_t offset);
    static bool is_fullbuffer_valid(const std::vector<unsigned char>& data,size_t& offset);
    static void fulldump(std::vector<unsigned char>& buffer);
    static bool addplayer(const Player& player){
        if(PlayerData::usedPlayername.find(player.getname()) != PlayerData::usedPlayername.end()){
            Internal::players[player.getname()]=player.data;
            return true;
        }
        return false;
    }
    static const PlayerData& getplayerbyname(const std::string& namaPlayer){
        return Internal::players.at(namaPlayer);
    }
    static const auto& getallplayer(){
        return Internal::players;
    }
    static bool delplayer(const std::string& namaPlayer){
        if(Internal::players.find(namaPlayer)!=Internal::players.end()){
            Internal::players.erase(namaPlayer);
            return true;
        }
        return false;
    }
    
};

