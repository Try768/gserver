#pragma once 
#include "../../internal/datatype.hpp"
#include "../../common.hpp"
#include "entity/list.hpp"
class Player;
class PlayerData:public Coord_manager{
    friend class Player;
    private:
    
    Typein::Component dynamic_property;
    Var_component_Object runtime_property;
    velo2 velocity;
    unsigned char flag;
    public:
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
    bool getFlag(PlayerData::status_flag pFlag)const{
        if(flag&pFlag)return true;return false;
    }
    void changeFlag(PlayerData::status_flag pFlag){
        flag^=pFlag;
    }
    //this function can throw error
    void parse(const std::vector<unsigned char>& buffer,size_t& offset){
        //buffer_bigendian_to_string_short(buffer,offset,playerName);
        //debug_print("parsed playername:"<<playerName);
        this->co_parse(buffer,offset);
    }
    PlayerData(const std::vector<unsigned char>& buffer,size_t& offset){
        parse(buffer,offset);
    }
    PlayerData()=default;
    PlayerData(Coord<int16_t>lokal,
        Coord<long long> global):Coord_manager(global,lokal){
        this->flag&=(~status_flag::is_name_aproved);
    }
};
//std::unordered_set<std::string> PlayerData::usedPlayername={};