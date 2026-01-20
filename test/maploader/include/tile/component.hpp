#pragma once
#include "../common.hpp"
#include "../forward.hpp"
#include "internal/component/const.hpp"

class tilecomponent{
    private:
    /* data */
    static unsigned long long idcount;
    std::string name;
    public:
    struct Internal{
        friend class registry;
        friend class TileData;
        friend class chunkmap;
        friend class tilecomponent;
        private:
        Internal(bool has_permutation,
        Const_component_Object c_component):has_permutation(has_permutation),
        c_component(c_component){}
        Internal()=default;
        bool has_permutation;
        Const_component_Object c_component;
        public:
        const auto& is_has_permutation(){return has_permutation;}
        const auto& get_CCO(){return c_component;}
    };
    private:
    const Internal& internal;
    unsigned long long id;
    tilecomponent(/* args */)=default;
    friend class registry;
    friend class TileData;
    friend class chunkmap;
    const Internal& get_internal(){
        return internal;
    }
    public:
    struct C_tileData{
        const std::string& name;
        const bool& c_size;
        const unsigned long long id;
    };
    C_tileData getData()const{
        return{name,internal.has_permutation,id};
    }
    tilecomponent(std::string name,const Internal& internal):name(name),internal(internal){
        this->id=idcount;
        idcount++;
    }
};

class IndeksTileComponent{
    friend class registry;
    friend class TileData;
    friend class chunkmap;
    private:
    const tilecomponent::Internal* TileType;
    bool valid;
    IndeksTileComponent(const tilecomponent::Internal& component):valid(1){
        TileType=&component;
    }
    IndeksTileComponent(const IndeksTileComponent& indeks):TileType(indeks.TileType),valid(1){
    }
    IndeksTileComponent():TileType(0),valid(0){}
    public:
    bool is_valid()const noexcept{return valid;}
    const tilecomponent::Internal& get()const noexcept{return *TileType;}
};
//std::vector<unsigned char> dump()const{
    //    std::vector<unsigned char> keluaran;
    //    ref_dump(keluaran);
    //    return keluaran;
    //}
    //inline void ref_dump(std::vector<unsigned char>& buffer)const{
    //    string_short_to_buffer_bigendian(name,buffer);
    //    to_buffer_bigendian<unsigned char>(internal.c_size,buffer);
    //}
    //static bool is_buffer_valid(const std::vector<unsigned char>& data,size_t& offset){
    //    using namespace zt::Internal;
    //    size_t len;
    //    if(!parse::checkStringBigendian(data,offset,len))return false;
    //    offset+=len;
    //    if(!parse::checkPrimitiveBigendian<unsigned char>(data,offset))return false;
    //    return true;
    //}
    ////this may throw errors
    //void parse(const std::vector<unsigned char>& data,size_t& offset){
    //    buffer_bigendian_to_string_short(data,offset,name);
    //    debug_print("component tile nama:"<<name);
    //    buffer_bigendian_to<unsigned char>(data,offset,internal.c_size);
    //}
    //tilecomponent(const std::vector<unsigned char>& data,size_t& offset){
    //    parse(data,offset);
    //}