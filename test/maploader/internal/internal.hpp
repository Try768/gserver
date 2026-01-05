#pragma once
#include <string>
#include <vector>
#include <array>
#include "../caster.h"
#include <cstdint>
#include <algorithm>
#include <cctype>
//todo:pisahin fungsi cek dan dumper nya 
//okey
#define internal_exeption__(logic,msg) if(! logic )throw std::exception(msg);
#define checking(x) if(! x ) return 0;
inline unsigned char ceklength(size_t value){
    if(value<=std::numeric_limits<unsigned char>::max()){
        return 1;
    }else if(value<=std::numeric_limits<uint16_t>::max()){
        return 2;
    }else if(value<=std::numeric_limits<uint32_t>::max()){
        return 4;
    }else {
        return 8;
    }
    //if(value<=std::numeric_limits<uint64_t>::max())
}
namespace zt::Internal{
    namespace parse{
        template<class T>
        inline bool checkPrimitiveBigendian(const std::vector<unsigned char>& buffer,size_t& offset){
            if(buffer.size()<sizeof(T)+offset)return false;
            offset+=sizeof(T);
            return 1;
        }
        inline bool checkDynamicBigendian(const std::vector<unsigned char>& buffer,size_t& offset,unsigned char& size){
            if((buffer.size())<(1+offset)){
                return false;
            }
            size=buffer[offset];
            if(size!=1&&size!=2&&size!=4&&size!=8){
                return false;
            }
            if(buffer.size()<size+offset){
                return false;
            }   
            offset+=1;

            return 1;
        }
        //next time
        bool checkArrayBigendian(const std::vector<unsigned char>& buffer,size_t& offset,size_t& arrlength,unsigned char& btl){
            if(buffer.size()<2+offset){
                return false;
            }
            if(buffer[offset]!=((unsigned char)10)){
                return false;
            }
            offset++;
            btl=buffer[offset];
            if(btl!=1&&btl!=2&&btl!=4&&btl!=8){
                return false;
            }
            offset++;
            if(buffer.size()<btl+offset){
                return false;
            }
            arrlength=0;
            for(int i=0;i<btl;++i){
                arrlength=(arrlength<<8)|buffer[offset];
                offset++;
            }
            if(buffer.size()<2+btl+arrlength){
                return false;
            }
            return true;
        }
        inline bool checkArrayBigendian(const std::vector<unsigned char>& buffer,size_t& offset,size_t& arrlength){
            unsigned char btl;
            return checkArrayBigendian(buffer,offset,arrlength,btl);
        }
        inline bool checkStringBigendian(const std::vector<unsigned char>& buffer,size_t& offset,size_t& length){
            if(buffer.size()<3+offset){
                return false;
            }
            if(buffer[offset] !=(unsigned char) 9){
                return false;
            }
            offset++;
            length=0;
            for(int i=1;i<3;++i){
                length=(length<<8)|buffer[offset];
                offset++;
            }
            if(buffer.size()<length+offset){
                return false;
            }
            return true;
        }
    }
}
template<typename T>
inline void to_buffer_bigendian(T value, std::vector<unsigned char>& buffer) {
    //buffer.push_back((unsigned char)sizeof(T));
    for (int i = sizeof(T) - 1; i >= 0; --i) {
        buffer.emplace_back(static_cast<unsigned char>((value >> (i * 8)) & 0xFF));
    }
}
//this function may throw error
template<typename T>
inline void buffer_bigendian_to(const std::vector<unsigned char>& buffer,size_t& offset, T& value){
    if(!zt::Internal::parse::checkPrimitiveBigendian<T>(buffer,offset))throw std::exception("error:primitif static parsing");
    offset-=sizeof(T);
    value=0;
    for(int i=0;i<sizeof(T);++i){
        value=(value<<8)|buffer[offset];
        offset++;
    }
    //buffer.erase(buffer.begin(),buffer.begin()+sizeof(T)+1);
}
inline void dynamic_to_buffer_bigendian(uint64_t value, std::vector<unsigned char>& buffer) {
    unsigned char size=ceklength(value);
    std::array<unsigned char,8> tmp;
    buffer.push_back(size);
    ll_to_buffer_bigendian(value,tmp);
    buffer.insert(buffer.end(),tmp.end()-size,tmp.end());
}
inline void swap_sp(unsigned char& a,unsigned char& b){
            a=a^b;b=b^a;a=a^b;
}
inline void swap(unsigned char& a,unsigned char& b){
            static unsigned char c;
            c=a;a=b;b=c;
}
inline void cssortchar(std::vector<unsigned char>& arr){
    for (size_t i = 0; i < arr.size(); i++)
    {
        for (size_t j = i; j < arr.size()-i; j++)
        {
            if(arr[i]>arr[j]){
                swap(arr[i],arr[j]);
            }
        }
    }
}
//this function may throw error
inline void buffer_bigendian_to_dynamic(const std::vector<unsigned char>& buffer,size_t& offset,uint64_t& value){
    unsigned char size=0;
    if(!zt::Internal::parse::checkDynamicBigendian(buffer,offset,size))throw std::exception("error:primitif dynamic parsing");
    value=0;
    for(int i=0;i<size;++i){
        value=(value<<8)|buffer[offset];
        offset++;
    }
    //buffer.erase(buffer.begin(),buffer.begin()+1+size);
    
}
inline bool is_number(const std::string& s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isdigit(c); });
}
std::string trimStringToShortSize(const std::string& str){
    std::string temp(str);
    if (temp.size() > std::numeric_limits<unsigned short>::max())
    {
        temp.erase(str.begin()+std::numeric_limits<unsigned short>::max(),str.end());
    }
    return temp;
}
//this function can auto trim any string that biger than unsigned short
inline void string_short_to_buffer_bigendian(const std::string& str, std::vector<unsigned char>& buffer) {
    buffer.push_back((unsigned char)9);
    std::string temp;
    if(str.size() > std::numeric_limits<unsigned short>::max())temp=trimStringToShortSize(str);else temp=str;
    unsigned short length = static_cast<unsigned short>(temp.size());
    for (int i = sizeof(length) - 1; i >= 0; --i) {
        buffer.emplace_back(static_cast<unsigned char>((length >> (i * 8)) & 0xFF));
    }
    buffer.insert(buffer.end(), str.begin(), str.end());
}
//this function can throw error
inline void buffer_bigendian_to_string_short(const std::vector<unsigned char>& buffer,size_t& offset,std::string& hasil){
    size_t length=0;
    hasil.reserve(128);
    if(!zt::Internal::parse::checkStringBigendian(buffer,offset,length))throw std::exception("error:str parsing");
    hasil.clear();
    for(int i=0;i<length;++i){
        hasil.push_back(static_cast<char>(buffer[offset]));offset++;
    }
    //buffer.erase(buffer.begin(),buffer.begin()+3+length);
}
inline void array_to_buffer_bigendian(const std::vector<unsigned char>& arr, std::vector<unsigned char>& buffer){
    buffer.push_back((unsigned char)10);
    dynamic_to_buffer_bigendian(arr.size(),buffer);
    buffer.insert(buffer.end(),arr.begin(),arr.end());
}
// this function can throw error
inline void buffer_bigendian_to_array(const std::vector<unsigned char>& buffer,size_t& offset,std::vector<unsigned char>& arr){
    size_t length;
    arr.clear();
    //if(!zt::Internal::parse::checkArrayBigendian(buffer,offset,length))throw std::exception("error:array parsing");
    internal_exeption__(zt::Internal::parse::checkArrayBigendian(buffer,offset,length),"error:array parsing");
    arr.insert(arr.end(),buffer.begin()+(offset),buffer.begin()+offset+length);
    offset+=length;
    //buffer.erase(buffer.begin(),buffer.begin()+2+btl+length);
}
class MultiValue {
public:
    enum class Type:unsigned char { Integer, String, Boolean, Loong, Character, Array, None };

private:
    Type type = Type::None;
    union {
        unsigned int intValue;
        bool boolValue;
        unsigned char charValue;
        unsigned long long loongValue;
    };
    std::string strValue;
    std::vector<unsigned char> arrValue;
    
public:
    bool setInt(unsigned int v) { if(type != Type::Integer) return false; intValue = v; return true; }
    bool setBool(bool v) { if(type != Type::Boolean) return false; boolValue = v; return true; }
    bool setChar(unsigned char v) { if(type != Type::Character) return false; charValue = v; return true; }
    bool setLoong(unsigned long long v) { if(type != Type::Loong) return false; loongValue = v; return true; }
    bool setString(const std::string& v) { if(type != Type::String) return false; strValue = v; return true; }
    bool setArray(const std::vector<unsigned char>& v) { if(type != Type::Array) return false; arrValue = v; return true; }
    MultiValue() : type(Type::None) {}
    MultiValue(unsigned int v) : type(Type::Integer), intValue(v) {}
    MultiValue(bool v) : type(Type::Boolean), boolValue(v) {}
    MultiValue(unsigned char v) : type(Type::Character), charValue(v) {}
    MultiValue(unsigned long long v) : type(Type::Loong), loongValue(v) {}
    MultiValue(const std::string& v) : type(Type::String), strValue(v) {}
    MultiValue(const std::vector<unsigned char>& v) : type(Type::Array), arrValue(v) {}

    Type getType() const { return type; }
    MultiValue& operator=(const MultiValue& lain){
        if(lain.type!=this->type){
            return *this;
        }
        switch (this->type)
        {
        case Type::Integer :{
            this->setInt(lain.getInt());
        }
            break;
        case Type::String :{
            this->setString(lain.getString());
        }
            break;
        case Type::Boolean :{
            this->setBool(lain.getBool());
        }
            break;
        case Type::Loong :{
            this->setLoong(lain.getLong());
        }
            break;
        case Type::Character :{
            this->setChar(lain.getChar());
        }
            break;
        case Type::Array :{
            this->setArray(lain.getArray());
        }
            break;
        default:
            break;
        }
        return *this;
    }
    unsigned int getInt() const { return intValue; }
    bool getBool() const { return boolValue; }
    unsigned char getChar() const { return charValue; }
    unsigned long long getLong() const { return loongValue; }
    const std::string& getString() const { return strValue; }
    const std::vector<unsigned char>& getArray() const { return arrValue; }
};