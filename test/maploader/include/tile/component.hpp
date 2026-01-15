#pragma once
#include "../common.hpp"
#include "../forward.hpp"
class tilecomponent{
private:
    /* data */
    static unsigned long long idcount;
    std::string name;
    unsigned char c_size;
    
    unsigned long long id;
    tilecomponent(/* args */)=default;
    public:
    struct C_tileData{
        const std::string& name;
        const unsigned char& c_size;
        const unsigned long long id;
    };
    C_tileData getData()const{
        return{name,c_size,id};
    }
    std::vector<unsigned char> dump()const{
        std::vector<unsigned char> keluaran;
        ref_dump(keluaran);
        return keluaran;
    }
    inline void ref_dump(std::vector<unsigned char>& buffer)const{
        string_short_to_buffer_bigendian(name,buffer);
        to_buffer_bigendian<unsigned char>(c_size,buffer);
    }
    static bool is_buffer_valid(const std::vector<unsigned char>& data,size_t& offset){
        using namespace zt::Internal;
        size_t len;
        if(!parse::checkStringBigendian(data,offset,len))return false;
        offset+=len;
        if(!parse::checkPrimitiveBigendian<unsigned char>(data,offset))return false;
        return true;
    }
    //this may throw errors
    void parse(const std::vector<unsigned char>& data,size_t& offset){
        buffer_bigendian_to_string_short(data,offset,name);
        debug_print("component tile nama:"<<name);
        buffer_bigendian_to<unsigned char>(data,offset,c_size);
    }
    tilecomponent(const std::vector<unsigned char>& data,size_t& offset){
        parse(data,offset);
    }
    tilecomponent(std::string name,unsigned char sizeofComponent):name(name),c_size(sizeofComponent){
        this->id=idcount;
        idcount++;
    }
};

class IndeksTileComponent{
    friend class registry;
    friend class tilelist;
    private:
    unsigned long long TileType;
    IndeksTileComponent(const tilecomponent& component):TileType(component.getData().id){}
    public:
    const unsigned long long getId()const noexcept{return TileType;}
};