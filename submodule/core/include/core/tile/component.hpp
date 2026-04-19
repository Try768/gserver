#pragma once
#include "../common.hpp"
#include "../forward.hpp"
#include "core/internal/component/var.hpp"
#include "core/eventListener/eventType.hpp"

class TileComponent{
    private:
    /* data */
    public:
    struct Internal{
        static unsigned long long idcount;
        friend class Registry;
        friend class TileData;
        friend class chunkmap;
        friend class TileComponent;
        friend class ComponentRegisterofTile;
        std::string name;
        unsigned char max_permutation;
        public:
        using ID=size_t;
        using RunComponent=std::array<std::vector<ID>,(size_t)zt::event::entity::Type::COUNT>;
        using optionalRC = zt::Internal::util::optionalRef<RunComponent>;
        private:
        RunComponent runComponent;
        unsigned long long id;
        Internal(
        Var_component_Object&& c_component,
        const std::string& name):name(name),
        c_component(std::move(c_component)){
            this->id=idcount;
            idcount++;
        }
        Internal()=default;
        
        const Var_component_Object c_component;
        public:
        unsigned char getMaxPermutation()const noexcept{return max_permutation;}
        const auto& getRunComponent()const {return runComponent;}
        auto& getRunComponent() {return runComponent;}
        const auto& get_CCO()const {return c_component;}
    };
    private:
    Internal& internal;
    TileComponent(/* args */)=default;
    friend class registry;
    friend class TileData;
    friend class chunkmap;
    const Internal& get_internal()const{
        return internal;
    }
    Internal& get_internal(){
        return internal;
    }
    public:
    struct C_tileData{
        const std::string& name;
        const unsigned long long& id;
    };
    C_tileData getData()const{
        return{internal.name,internal.id};
    }
    TileComponent(Internal& internal):internal(internal){
        
    }
};

class IndeksTileComponent{
    friend class Registry;
    friend class TileData;
    friend class Tile;
    friend class chunkmap;
    private:
    TileComponent::Internal* TileType;
    bool valid;
    IndeksTileComponent(TileComponent::Internal& component):valid(1){
        TileType=&component;
    }
    IndeksTileComponent(IndeksTileComponent& indeks):TileType(indeks.TileType),valid(1){
    }
    IndeksTileComponent(const IndeksTileComponent& indeks):TileType(indeks.TileType),valid(1){
    }
    IndeksTileComponent():TileType(0),valid(0){}
    public:
    bool is_valid()const noexcept{return valid;}
    const TileComponent::Internal& get()const noexcept{return *TileType;}
    TileComponent::Internal& get() noexcept{return *TileType;}
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