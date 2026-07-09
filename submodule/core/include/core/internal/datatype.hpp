#pragma once
#include "internal.hpp"
#include <sstream>
#include "miniz.h"
#include <vector>
#include "checksum.hpp"
#include "coordinat.hpp"
#include <string>
#include <variant>
#include <chrono>
#include <cmath>
#include <map>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
//constexpr unsigned char chunklen=16;
struct HealthCache{
    Var_component_Number* maxHealt=nullptr;
    long long healt;
};
namespace t_type_id{
    constexpr unsigned char integer=0x00;
    constexpr unsigned char string=0x01;
    constexpr unsigned char boolean=0x02;
    constexpr unsigned char array=0x03;
    constexpr unsigned char loong=0x04;
    constexpr unsigned char character=0x05;
}



class EntityComponent;
class EntityData;
class PlayerComponent;
class PlayerData;
namespace Typein{
    class Component {
        friend class EntityComponent;
        friend class EntityData;
        friend class PlayerComponent;
        friend class PlayerData;
    private:
    Component(std::vector<unsigned char>& buffer,size_t& offset){
        parse(buffer,offset);
    }
    Component()=default;
    public:
    std::unordered_map<std::string, MultiValue> data;
    inline MultiValue& operator[](const std::string& key) {
        return data[key];
    }
    static bool is_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
        using namespace zt::Internal;
        if(!parse::checkPrimitiveBigendian<unsigned long long>(buffer,offset))return false;
        unsigned long long banyak;
        buffer_bigendian_to<unsigned long long>(buffer,offset-=8,banyak);
        for(int i=0;i<banyak;i++){
            size_t length;
            if(!parse::checkStringBigendian(buffer,offset,length))return false;
            offset+=length;
            if(buffer.size()<offset+1)return false;
            switch (MultiValue::Type(buffer[offset]))
            {
            case MultiValue::Type::Integer:
                offset++;
                if(!parse::checkPrimitiveBigendian<unsigned int>(buffer,offset))return false;
                break;
            case MultiValue::Type::String:
                offset++;
                if(!parse::checkStringBigendian(buffer,offset,length))return false;
                offset+=length;
                break;
            case MultiValue::Type::Boolean:
                offset++;
                if(!parse::checkPrimitiveBigendian<bool>(buffer,offset))return false;
                break;
            case MultiValue::Type::Loong:
                offset++;
                if(!parse::checkPrimitiveBigendian<long long>(buffer,offset))return false;
                break;
            case MultiValue::Type::Character:
                offset++;
                if(!parse::checkPrimitiveBigendian<unsigned char>(buffer,offset))return false;
                break;
            case MultiValue::Type::Array:
                offset++;
                if(!parse::checkArrayBigendian(buffer,offset,length))return false;
                offset+=length;
            default:
                offset++;
                return false;
                break;
            }
        }
        return true;
    }
    //this may throw errors
    void parse(const std::vector<unsigned char>& buffer,size_t& offset){
        data.clear();
        unsigned long long size;
        buffer_bigendian_to(buffer,offset,size);
        std::string key;
        unsigned char type;
        for (size_t i = 0; i < size; i++)
        {
            buffer_bigendian_to_string_short(buffer,offset,key);
            buffer_bigendian_to<unsigned char>(buffer,offset,type);
            switch (MultiValue::Type(type))
            {
            case MultiValue::Type::Integer:
                {
                    unsigned int val;
                    buffer_bigendian_to<unsigned int>(buffer,offset,val);
                    data[key]=MultiValue(val);
                }
                break;
            case MultiValue::Type::String:
                {
                    std::string val;
                    buffer_bigendian_to_string_short(buffer,offset,val);
                    data[key]=MultiValue(val);
                }
                break;
            case MultiValue::Type::Boolean:
                {
                    bool val;
                    buffer_bigendian_to<bool>(buffer,offset,val);
                    data[key]=MultiValue(val);
                }
                break;
            case MultiValue::Type::Loong:
                {
                    unsigned long long val;
                    buffer_bigendian_to<unsigned long long>(buffer,offset,val);
                    data[key]=MultiValue(val);
                }
                break;
            case MultiValue::Type::Character:
                {
                    unsigned char val;
                    buffer_bigendian_to<unsigned char>(buffer,offset,val);
                    data[key]=MultiValue(val);
                }
                break;
            case MultiValue::Type::Array:
                {
                    std::vector<unsigned char> val;
                    buffer_bigendian_to_array(buffer,offset,val);
                    data[key]=MultiValue(val);
                }
                break;
            default:
                throw std::exception("error:unsupported type in component parse");
                break;
            }
        }
        
    }
    
    void dump(std::vector<unsigned char>& keluaran)const{
        to_buffer_bigendian<unsigned long long>(data.size(),keluaran);
        for (const auto& [key, value] : data) {
            string_short_to_buffer_bigendian(key,keluaran);
            to_buffer_bigendian<unsigned char>(static_cast<unsigned char>(value.getType()),keluaran);
            switch (value.getType())
            {
            case MultiValue::Type::Integer:
                to_buffer_bigendian<unsigned int>(value.getInt(),keluaran);
                break;
            case MultiValue::Type::String:
                string_short_to_buffer_bigendian(value.getString(),keluaran);
                break;
            case MultiValue::Type::Boolean:
                to_buffer_bigendian<bool>(value.getBool(),keluaran);
                break;
            case MultiValue::Type::Loong:
                to_buffer_bigendian<long long>(value.getLong(),keluaran);
                break;
            case MultiValue::Type::Character:
                to_buffer_bigendian<unsigned char>(value.getChar(),keluaran);
                break;
            case MultiValue::Type::Array:
                array_to_buffer_bigendian(value.getArray(),keluaran);
            default:
                break;
            }
        }
        
    }
};
}

//maks = maks int if posible
    //Coordinat operator+(const Coord<double>& other){
    //    long long ox=std::floor(other.x*100);
    //    long long oy=(std::floor(other.y*100));
    //    ox+=lokal.x;
    //    if(ox>makslokal){
    //        ox-=(makslokal -lokal.x);
    //        lokal.x=(ox%lenlokal)-makslokal;
    //        global.x=(ox-lokal.x)/lenlokal;
    //    }if(ox<minlokal){
    //        ox-=(minlokal -lokal.x);
    //        lokal.x=(ox%lenlokal)-minlokal;
    //        global.x=(ox-lokal.x)/lenlokal;
    //    }
    //};


