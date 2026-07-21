#pragma once
#include <stdexcept>
#include "idmaker.hpp"
#include <string>
#include "chunkpos.hpp"
#include <vector>
#include <array>
#include "../caster.h"
#include <cstdint>
#include <algorithm>
#include <cctype>
#include "util.hpp"
#define __my__iterator(x)                                   \
    using iterator = decltype(x)::iterator;               \
    using const_iterator = decltype(x)::const_iterator;   \
    iterator begin() noexcept { return x.begin(); }       \
    iterator end()   noexcept { return x.end(); }         \
    const_iterator begin() const noexcept { return x.begin(); } \
    const_iterator end()   const noexcept { return x.end(); }   \
    const_iterator cbegin() const noexcept { return x.cbegin(); } \
    const_iterator cend()   const noexcept { return x.cend(); }

//todo:pisahin fungsi cek dan dumper nya 
//todo:79 checker
//okey
#define indebug 1
#if indebug==1
#include<iostream>
#define debug_print(...) std::cout<<__VA_ARGS__<<std::endl;
#else
#define debug_print(...)
#endif

#define internal_exeption__(logic,msg) if(! logic )throw std::exception(msg);
#define checking(x) if(! x ) return 0;
inline unsigned char ceklength(unsigned long long value){
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
namespace zt{
    template<class T>
    class ManualObject{
        private:
        alignas(T) unsigned char storage[sizeof(T)];
        public:
        template<typename... Args>
        T* ctor(Args&&... args){
            return new (storage) T(std::forward<Args>(args)...);
        }
        void dtor(){
            reinterpret_cast<T*>(storage)->~T();
        }
        T& operator*(){return *reinterpret_cast<T*>(storage);}
        T* get(){return reinterpret_cast<T*>(storage);}
        const T* get()const {return reinterpret_cast<const T*>(storage);}
    };
}
namespace zt::Internal{
    //return 0 if fail
    namespace parse{
        //return len
        template<class T>
        inline size_t checkPrimitiveBigendian(const std::vector<unsigned char>& buffer,size_t offset){
            if(buffer.size()<sizeof(T)+offset)return false;
            return sizeof(T)+offset;
        }
        //return len
        inline size_t checkDynamicBigendian(const std::vector<unsigned char>& buffer,size_t offset,unsigned char& size){
            if((buffer.size())<(1+offset)){
                return 0;
            }
            size=buffer[offset];
            if(size!=1&&size!=2&&size!=4&&size!=8){
                return 0;
            }
            if(buffer.size()<size+offset){
                return 0;
            }
            return 1+size+offset;
        }
        //return len
        size_t checkArrayBigendian(const std::vector<unsigned char>& buffer,size_t offset,size_t& arrlength,unsigned char& btl){
            
            if(buffer.size()<2+offset){
                return 0;
            }
            if(buffer[offset]!=((unsigned char)10)){
                return 0;
            }
            offset++;
            btl=buffer[offset];
            if(btl!=1&&btl!=2&&btl!=4&&btl!=8){
                return 0;
            }
            offset++;
            if(buffer.size()<btl+offset){
                return 0;
            }
            arrlength=0;
            for(int i=0;i<btl;++i){
                arrlength=(arrlength<<8)|buffer[offset];
                offset++;
            }
            if(buffer.size()<2+btl+arrlength+offset){
                return 0;
            }
            return offset;
        }
        inline size_t checkArrayBigendian(const std::vector<unsigned char>& buffer,size_t offset,size_t& arrlength){
            unsigned char btl;
            return checkArrayBigendian(buffer,offset,arrlength,btl);
        }
        inline size_t checkStringBigendian(const std::vector<unsigned char>& buffer,size_t offset,size_t& length){
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
            return offset;
        }
        inline size_t checkStringBigendian(const std::vector<unsigned char>& buffer,size_t offset){
            if(buffer.size()<3+offset){
                return false;
            }
            if(buffer[offset] !=(unsigned char) 9){
                return false;
            }
            offset++;
            size_t length=0;
            for(int i=1;i<3;++i){
                length=(length<<8)|buffer[offset];
                offset++;
            }
            if(buffer.size()<length+offset){
                return false;
            }
            return offset;
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
    enum class Type:unsigned char {Integer, String, Boolean, Loong, Character, Array, None };

private:
    Type type = Type::None;
    union {
        unsigned int intValue;
        bool boolValue;
        unsigned char charValue;
        long long loongValue;
    };
    std::string strValue;
    std::vector<unsigned char> arrValue;
    
public:
    void setInt(unsigned int v) { type=Type::Integer;  intValue = v; }
    void setBool(bool v) { type=Type::Boolean; boolValue = v; }
    void setChar(unsigned char v) {  type=Type::Character; charValue = v; }
    void setLoong(long long v) { type=Type::Loong;  loongValue = v; }
    void setString(const std::string& v) { type=Type::String;  strValue = v;  }
    void setArray(const std::vector<unsigned char>& v) { type=Type::Array; arrValue = v;  }
    MultiValue() : type(Type::None) {}
    MultiValue(unsigned int v) : type(Type::Integer), intValue(v) {}
    MultiValue(bool v) : type(Type::Boolean), boolValue(v) {}
    MultiValue(unsigned char v) : type(Type::Character), charValue(v) {}
    MultiValue(unsigned long long v) : type(Type::Loong), loongValue(v) {}
    MultiValue(const std::string& v) : type(Type::String), strValue(v) {}
    MultiValue(const std::vector<unsigned char>& v) : type(Type::Array), arrValue(v) {}

    Type getType() const { return type; }
    unsigned int getInt() const { return intValue; }
    bool getBool() const { return boolValue; }
    unsigned char getChar() const { return charValue; }
    long long getLong() const { return loongValue; }
    const std::string& getString() const { return strValue; }
    const std::vector<unsigned char>& getArray() const { return arrValue; }
    MultiValue& operator=(const MultiValue& lain){
        switch (type)
        {
        case Type::Array:
            arrValue.clear();
            break;
        case Type::String:
            strValue.clear();
            break;
        default:
            break;
        }
        switch (lain.type)
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
    MultiValue(const MultiValue& lain){
        
        switch (lain.type)
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
    }
};
class Dynamic_Property_Parent{
    protected:
    Dynamic_Property_Parent()=default;
    template<class T>
    using opt=zt::Internal::util::OptionalRef<T>;
    std::unordered_map<std::string,MultiValue> dynamic_property;
    public:
    const opt<const MultiValue> get_dynamic_property(const std::string& key)const{
        auto itc=dynamic_property.find(key);
        if(itc!=dynamic_property.end())return opt<const MultiValue>(itc->second);
        return opt<const MultiValue>();
    }
     opt< MultiValue> get_dynamic_property(const std::string& key){
        auto itc=dynamic_property.find(key);
        if(itc!=dynamic_property.end())return opt<MultiValue>(itc->second);
        return opt<MultiValue>();
    }
    bool create_dynamic_property(const std::string& key,const MultiValue& value){
        auto [itc,stat]=dynamic_property.try_emplace(key,value);
        return stat;
    }
    void clear_dynamic_property(const std::string& key){
        dynamic_property.erase(key);
    }
    __my__iterator(dynamic_property)

};