#pragma once
#include "../common.hpp"
#include "../tile/component.hpp"
#include "../entity/component.hpp"
#include "../entity/player/list.hpp"
class registry{
    private:
    static std::unordered_map<std::string,playerlist> players;
    static std::vector<tilecomponent> tiles;
    static std::unordered_map<unsigned long long,entitycomponent> entities;
    static inline const  std::array<unsigned char,8> sign ={10,20,80,90,40,20,222,0};
    public:
    //flag:1=dgn tile,2=tanpa;
    static void dump(std::vector<unsigned char>& keluaran){
        
        dynamic_to_buffer_bigendian(players.size(),keluaran);
        for (auto& player:players)
        {
            player.second.dump(keluaran);
        }
        to_buffer_bigendian<unsigned long long>(tiles.size(),keluaran);
        for(auto& tile:tiles){
            tile.ref_dump(keluaran);
        }
        to_buffer_bigendian<unsigned long long>(entities.size(),keluaran);
        for (auto& ent:entities)
        {
            dynamic_to_buffer_bigendian(ent.first,keluaran);
            ent.second.ref_dump(keluaran);
        }
        
    }
    static inline bool is_buffer_valid(const std::vector<unsigned char>& data,size_t& offset){
        using namespace zt::Internal;
        unsigned char len;
        unsigned long long arrsize;
        if(!parse::checkDynamicBigendian(data,offset,len))return false;
        offset--;buffer_bigendian_to_dynamic(data,offset,arrsize);
        for (size_t i = 0; i < arrsize; i++)
        {
            if(!playerlist::is_buffer_valid(data,offset))return false;
        }
        if(!parse::checkPrimitiveBigendian<unsigned long long>(data,offset))return false;
        buffer_bigendian_to<unsigned long long>(data,offset,arrsize);
        for (size_t i = 0; i < arrsize; i++)
        {
            if(!tilecomponent::is_buffer_valid(data,offset))return false;
        }
        if(!parse::checkPrimitiveBigendian<unsigned long long>(data,offset))return false;
        buffer_bigendian_to<unsigned long long>(data,offset,arrsize);
        for (size_t i = 0; i < arrsize; i++)
        {
            if(!parse::checkDynamicBigendian(data,offset,len))return false;
            offset--;
            unsigned long long id;
            buffer_bigendian_to_dynamic(data,offset,id);
            if(!entitycomponent::is_buffer_valid(data,offset))return false;
        }
        return true;
    }
    static const auto& getEntities(){
        return entities;
    }
    static const auto& getTiles(){
        return tiles;
    }
    //this may throw errors
    static void parse(const std::vector<unsigned char>& data,size_t& offset){
        using namespace zt::Internal;
        unsigned char len;
        unsigned long long arrsize;
        players.clear();
        buffer_bigendian_to_dynamic(data,offset,arrsize);
        for (size_t i = 0; i < arrsize; i++)
        {
            playerlist temp(data,offset);
            players[temp.getname()]=temp;
        }
        tiles.clear();
        buffer_bigendian_to<unsigned long long>(data,offset,arrsize);
        for (size_t i = 0; i < arrsize; i++)
        {
            tilecomponent temp(data,offset);
            tiles.push_back(temp);
        }
        entities.clear();
        buffer_bigendian_to<unsigned long long>(data,offset,arrsize);
        for (size_t i = 0; i < arrsize; i++)
        {
            unsigned long long id;
            buffer_bigendian_to_dynamic(data,offset,id);
            entitycomponent temp(data,offset);
            entities[id]=temp;
        }
    }
    static void fullparse(const std::vector<unsigned char>& data,size_t offset){
        using namespace zt::Internal;
        unsigned long long sum;
        unsigned long long arrsize;
        for (size_t i = 0; i < sign.size(); i++)
        {
            if(data[offset++]!=sign[i])throw std::runtime_error("Registry sign invalid");
        }
        buffer_bigendian_to<unsigned long long>(data,offset,sum);
        std::vector<unsigned char> temp;
        buffer_bigendian_to_array(data,offset,temp);

        if(checksumparent::verifycheksum(temp.begin(),temp.end(),sum))throw std::runtime_error("Registry checksum invalid");
        parse(temp,offset);
    }
    static bool is_fullbuffer_valid(const std::vector<unsigned char>& data,size_t& offset){
        using namespace zt::Internal;
        unsigned long long sum;
        for (size_t i = 0; i < sign.size(); i++)
        {
            if(data[offset++]!=sign[i])return false;
        }
        if(!parse::checkPrimitiveBigendian<unsigned long long>(data,offset))return false;
        buffer_bigendian_to<unsigned long long>(data,offset,sum);
        std::vector<unsigned char> temp;
        size_t leng;
        unsigned char btl;
        if(!parse::checkArrayBigendian(data,offset,leng,btl))return false;
        offset-=(btl+2);
        buffer_bigendian_to_array(data,offset,temp);

        if(!checksumparent::verifycheksum(temp.begin(),temp.end(),sum))return false;
        size_t tempoffset=0;
        if(!is_buffer_valid(temp,tempoffset))return false;
        return true;
    }
    static void fulldump(std::vector<unsigned char>& buffer){
        
        buffer.insert(buffer.end(),sign.begin(),sign.end());
        {
            std::vector<unsigned char> temp;
            dump(temp);
            CheckSumForStatic ast(temp);
            to_buffer_bigendian<unsigned long long>(ast.getcheksum(),buffer);
            array_to_buffer_bigendian(temp,buffer);
        }
        
    }
    static bool addplayer(const playerlist player){
        if(playerlist::usedPlayername.find(player.getname()) != playerlist::usedPlayername.end()){
            registry::players[player.getname()]=player;
            return true;
        }
        return false;
    }
    static const playerlist& getplayerbyname(const std::string& namaPlayer){
        return players.at(namaPlayer);
    }
    static const auto& getallplayer(){
        return players;
    }
    static bool delplayer(const std::string& namaPlayer){
        if(players.find(namaPlayer)!=players.end()){
            players.erase(namaPlayer);
            return true;
        }
        return false;
    }
    static bool entityregister(const entitycomponent entity){
        if(entities.find(entity.getData().id)!=entities.end()){
            return false;
        }
        entities[entity.getData().id]=entity;
        return true;
    }
    static bool delEntityregistered(unsigned long long entityId){
        if(registry::entities.find(entityId)!=registry::entities.end()){
            registry::entities.erase(entityId);
            return true;
        }
        return false;
        
    }
    static void tileregister(const tilecomponent tile){
        registry::tiles.push_back(tile);
    }
};
std::unordered_map<std::string,playerlist> registry::players={};
std::vector<tilecomponent> registry::tiles={};
std::unordered_map<unsigned long long,entitycomponent> registry::entities={};
