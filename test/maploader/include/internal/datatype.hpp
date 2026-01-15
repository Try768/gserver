#pragma once
#include "internal.hpp"

#include "miniz.h"
#include <vector>
#include <string>
#include <variant>
#include <map>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
//constexpr unsigned char chunklen=16;
namespace t_type_id{
    constexpr unsigned char integer=0x00;
    constexpr unsigned char string=0x01;
    constexpr unsigned char boolean=0x02;
    constexpr unsigned char array=0x03;
    constexpr unsigned char loong=0x04;
    constexpr unsigned char character=0x05;
}

class checksumparent{
    protected:
    checksumparent()=default;
    public:
    virtual std::vector<unsigned char> bufferdump(){
        return {};
    }
    inline unsigned long long getchecksum(){
        std::vector<unsigned char> buffer=bufferdump();
        return mz_crc32(0,buffer.data(),buffer.size());
    }
    inline unsigned long long getchecksum(const std::vector<unsigned char>& buffer)const{
        return mz_crc32(0,buffer.data(),buffer.size());
    }
    inline bool verifychecksum(unsigned long long checksum){
        return getchecksum()==checksum;
    }
    inline bool verifychecksum(unsigned long long checksum,const std::vector<unsigned char>& buffer)const{
        return mz_crc32(0,buffer.data(),buffer.size())==checksum;
    }
    //static inline bool verifychecksum(const std::vector<unsigned char>::const_iterator awal,
    //    const std::vector<unsigned char>::const_iterator akhir,unsigned long long checksum){
    //    return mz_crc32(0,&(*awal),(size_t)(akhir-awal))==checksum;
    //}
    static inline bool verifychecksum(
        std::vector<unsigned char>::const_iterator awal,
        std::vector<unsigned char>::const_iterator akhir,
        unsigned long long checksum
    ){
        if (awal == akhir)
            return checksum == 0;

        return mz_crc32(
            0,
            std::addressof(*awal),
            static_cast<size_t>(std::distance(awal, akhir))
        ) == checksum;
}
};
class CheckSumForStatic{
    private:
    unsigned long long checksum;
    public:
    CheckSumForStatic(const std::vector<unsigned char>& buffer){
        checksum=mz_crc32(0,buffer.data(),buffer.size());
    }
    CheckSumForStatic(unsigned long long checksum_in):checksum(checksum_in){}
    bool verifychecksum(unsigned long long checksum_in){
        return checksum_in==checksum;
    }
    inline unsigned long long getchecksum()const{
        return checksum;
    }
    bool verifychecksum(const std::vector<unsigned char>& buffer){
        return mz_crc32(0,buffer.data(),buffer.size())==checksum;
    }
};
class EntityComponent;
class EntityData;
namespace Typein{
    class Component {
        friend class EntityComponent;
        friend class EntityData;
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

template<typename T>
struct Coord
{
    T x;
    T y;
    Coord(T x=0,T y=0):x(x),y(y){}
};
struct Coordinat{
    Coord<unsigned int> lokal;
    Coord<long long> global;
    Coordinat(Coord<unsigned int> lokal,Coord<long long> global):lokal(lokal),global(global){}
};
class Coord_manager_local{
    protected:
    Coord<unsigned int> lokal;
    Coord_manager_local()=default;
    void localdump(std::vector<unsigned char>& keluaran)const{
        to_buffer_bigendian<unsigned int>(lokal.x,keluaran);
        to_buffer_bigendian<unsigned int>(lokal.y,keluaran);
    }
    static bool is_local_coor_buffer_valid(const std::vector<unsigned char>& buffer,size_t& offset){
        using namespace zt::Internal;
        bool a=zt::Internal::parse::checkPrimitiveBigendian<unsigned int>(buffer,offset);
        return parse::checkPrimitiveBigendian<unsigned int>(buffer,offset)&&a;
    }
    void localCoorParse(const std::vector<unsigned char>& buffer,size_t& offset){
        buffer_bigendian_to<unsigned int>(buffer,offset,lokal.x);
        buffer_bigendian_to<unsigned int>(buffer,offset,lokal.y);
    }
    public:
    inline void setlocalcoord(Coord<unsigned int> lokal){
            this->lokal=lokal;
    }
    inline Coord<unsigned int> getlocalcoord()const{
            return lokal;
            
    }
};
class Coord_manager:public Coord_manager_local{
    protected:
        Coord<long long> global;
        void co_dump(std::vector<unsigned char>& keluaran)const{
            this->localdump(keluaran);
            to_buffer_bigendian<long long>(global.x,keluaran);
            to_buffer_bigendian<long long>(global.y,keluaran);
            //to_buffer_bigendian<unsigned char>((((unsigned char)global.signx)<<1)|((unsigned char)global.signy),keluaran);
        }
        Coord_manager(){
            global={0,0};
        };
        static bool is_co_valid(const std::vector<unsigned char>& buffer,size_t& offset){
            bool a=Coord_manager_local::is_local_coor_buffer_valid(buffer,offset);
            using namespace zt::Internal;
            a=parse::checkPrimitiveBigendian<long long>(buffer,offset)&&a;
            return parse::checkPrimitiveBigendian<long long>(buffer,offset)&&a;
        }
        void co_parse(const std::vector<unsigned char>& buffer,size_t& offset){
            this->localCoorParse(buffer,offset);
            buffer_bigendian_to<long long>(buffer,offset,global.x);
            buffer_bigendian_to<long long>(buffer,offset,global.y);
        }
        Coord_manager(const Coord<long long>& pos):global(pos)
        {
        }
    public:
        
        inline const Coord<long long>& getchunkcoord()const{
            return global;
        }
        inline void setchunkcoord(Coord<long long> chunk){
            this->global=chunk;
        }
};


