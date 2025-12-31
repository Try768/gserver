#pragma once
#include "../common.hpp"
class tilecomponent{
private:
    /* data */
    std::string name;
    unsigned char c_size;
public:
    tilecomponent(/* args */)=default;
    struct C_tileData{
        const std::string& name;
        const unsigned char& c_size;
    };
    C_tileData getData()const{
        return{name,c_size};
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
        buffer_bigendian_to<unsigned char>(data,offset,c_size);
    }
    tilecomponent(std::vector<unsigned char>& data,size_t& offset){
        parse(data,offset);
    }
    tilecomponent(std::string name,unsigned char sizeofComponent):name(name),c_size(sizeofComponent){}
};