#pragma once 
#include "../../internal/datatype.hpp"
#include "../../common.hpp"
#include "core/forward.hpp"
#include "core/entity/list.hpp"
class Player;
class SimulatedPlayer;
class PlayerData:public Coord_manager{
    friend class Player;
    friend class SimulatedPlayer;
    private:
    std::string name;
    OriginWorld origin;
    Typein::Component dynamic_property;
    
    Var_component_Object runtime_property;
    velo2 velocity;
    XUID xuid;
    std::string name;
    unsigned long long id;
    unsigned char flag;
    public:
    //friend class zt::ManualObject<Player>;
    PlayerData(const OriginWorld& origin):origin(origin){};
    inline const Var_component_Object& getRuntimeproperty()const{return runtime_property;};
    enum status_flag:unsigned char{
            is_valid=0b01,
            is_loaded=0b10,
            is_name_aproved=0b100
    };
    static std::unordered_set<std::string> usedPlayername;
    void dump(std::vector<unsigned char>& keluaran){
        //string_short_to_buffer_bigendian(playerName,keluaran);
        this->co_dump(keluaran);
    }
    static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
        size_t length;
        if(!zt::Internal::parse::checkStringBigendian(buffer,offset,length))return false;
        offset+=length;
        if(!Coord_manager::is_co_valid(buffer,offset))return false;
        return true;
    }
    inline bool getFlag(PlayerData::status_flag pFlag)const{
        if(flag&pFlag)return true;return false;
    }
    inline void changeFlag(PlayerData::status_flag pFlag){
        flag^=pFlag;
    }
    inline void setname(const std::string& name){
        flag&=status_flag::is_name_aproved;
        this->name=name;
    }
    //this function can throw error
    inline static  bool parse(zt::ManualObject<PlayerData>& dat,const std::vector<unsigned char>& buffer,size_t& offset){
        //buffer_bigendian_to_string_short(buffer,offset,playerName);
        //debug_print("parsed playername:"<<playerName);
        dat.get()->dynamic_property.parse(buffer,offset);
        dat.get()->co_parse(buffer,offset);
    }

    inline static bool deserielise(zt::ManualObject<PlayerData>& dat,const std::vector<unsigned char>& buffer,size_t& offset,Dimension& dim){
        dat.ctor(dim);
        parse(dat,buffer,offset);
    }
    inline static bool deserielise(zt::ManualObject<PlayerData>& dat,const std::vector<unsigned char>& buffer,Dimension& dim){
        size_t offset=0;
        dat.ctor(dim);
        parse(dat,buffer,offset);
    }
    
    inline PlayerData(Coord<int16_t>lokal,
        Coord<long long> global,XUID xuid,const OriginWorld& origin):Coord_manager(global,lokal),xuid(xuid),origin(origin){
        this->flag&=(~status_flag::is_name_aproved);
    }
};
//std::unordered_set<std::string> PlayerData::usedPlayername={};