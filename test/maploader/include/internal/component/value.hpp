#pragma once
#include "const.hpp"
#include <vector>
#include "../internal.hpp"
//wip class do not include
//to do : ubah jadi value component
#define ZT_buffer std::vector<unsigned char>& buffer

class v_component;
class v_component_Array;
class v_component_Long;
class v_component_String;
class v_component_Bool;
class v_component_Object;
class v_component_Int;
class v_component_Char;
namespace v_component_type{
    enum class Type:unsigned char { None,Long,Int,Char,String,Object,Bool,Array};
    bool is_valid(Type ty){
        return ty<=Type::Array;
    }
        template<Type T> struct internal; // primary (kosong)
        template<>struct internal<Type::Long> {
            using type = v_component_Long*;
        };
        template<> struct internal<Type::Int> {
            using type = v_component_Int*;
        };
        template<> struct internal<Type::Char> {
            using type = v_component_Char*;
        };
        template<> struct internal<Type::String> {
            using type = v_component_String*;
        };
        template<> struct internal<Type::Bool> {
            using type = v_component_Bool*;
        };
        template<> struct internal<Type::Array> {
            using type = v_component_Array*;
        };
        template<> struct internal<Type::Object> {
            using type = v_component_Object*;
        };
        template<>struct internal<Type::None> {
            using type = void*;
        };
};
class v_component{
    protected:
    using cct=v_component_type::Type;
    const cct type;
    explicit v_component(cct type=cct::None):type(type){}
    public:
    inline static cct type=cct::None;
    constexpr cct get_type()const noexcept{return type;}
    template<cct T>
    typename v_component_type::internal<T>::type getCast(){
        if (T!=get_type())return nullptr;
        return static_cast<v_component_type::internal<T>::type>(this);
    }
};
class v_component_Long final:public v_component{
    private:
    long long value;
    public:
    const long long get()const{return value;}
    void dump(std::vector<unsigned char>& buffer){
        to_buffer_bigendian(value,buffer);
    }
    static bool is_valid(ZT_buffer,size_t& offset){
        if(!zt::Internal::parse::checkPrimitiveBigendian<long long>(buffer,offset))return false;
    }
    void parse(ZT_buffer,size_t& offset){
        buffer_bigendian_to(buffer,offset,value);
    }
    explicit v_component_Long(long long value):value(value),v_component(cct::Long){}
    inline const static cct type=cct::Long;
    void set(long long value){
        this->value=value;
    }
    void set_type(){}
};
class v_component_Int final:public v_component{
    private:
    public:
    my_property_(int,value,get)
    inline const static cct type=cct::Int;
    explicit v_component_Int(int value):value(value),v_component(cct::Int){}
    void set(int value){
        this->value=value;
    }
    void dump(std::vector<unsigned char>& buffer){
        to_buffer_bigendian(value,buffer);
    }
    static bool is_valid(ZT_buffer,size_t& offset){
        if(!zt::Internal::parse::checkPrimitiveBigendian<int>(buffer,offset))return false;
    }
    void parse(ZT_buffer,size_t& offset){
        buffer_bigendian_to(buffer,offset,value);
    }
};
class v_component_Char final:public v_component{
    private:
    public:
    my_property_(unsigned char,value,get)
    inline const static cct type=cct::Char;
    explicit v_component_Char(unsigned char value):value(value),v_component(cct::Char){}
    void set(unsigned char value){
        this->value=value;
    }
    void dump(std::vector<unsigned char>& buffer){
        to_buffer_bigendian(value,buffer);
    }
    static bool is_valid(ZT_buffer,size_t& offset){
        if(!zt::Internal::parse::checkPrimitiveBigendian<unsigned char>(buffer,offset))return false;
    }
    void parse(ZT_buffer,size_t& offset){
        buffer_bigendian_to(buffer,offset,value);
    }
};
class v_component_String final:public v_component{
    private:
    public:
    my_property_(std::string,value,get)
    inline const static cct type=cct::String;
    explicit v_component_String(std::string value):value(value),v_component(cct::String){}
    void set(std::string value){
        this->value=value;
    }
    void dump(std::vector<unsigned char>& buffer){
        string_short_to_buffer_bigendian(value,buffer);
    }
    static bool is_valid(ZT_buffer,size_t& offset){
        size_t len=0;
        if(!zt::Internal::parse::checkStringBigendian(buffer,offset,len))return false;
        offset+=len;
    }
    void parse(ZT_buffer,size_t& offset){
        buffer_bigendian_to_string_short(buffer,offset,value);
    }
};
class v_component_Bool final:public v_component{
    private:
    my_property_(bool,value,get)
    public:
    inline const static cct type=cct::Bool;
    explicit v_component_Bool(bool value):value(value),v_component(type){}
    void set(bool value){
        this->value=value;
    }
    void dump(std::vector<unsigned char>& buffer){
        to_buffer_bigendian<unsigned char>(value,buffer);
    }
    static bool is_valid(ZT_buffer,size_t& offset){
        if(!zt::Internal::parse::checkPrimitiveBigendian<unsigned char>(buffer,offset))return false;
    }
    void parse(ZT_buffer,size_t& offset){
        unsigned char tmp;
        buffer_bigendian_to<unsigned char>(buffer,offset,tmp);
        value=tmp;
    }
};
namespace{
    namespace vct=v_component_type;
}
class internal_type{
    protected:
    v_component_type::Type type;
    internal_type(vct::Type type,bool usekey):type(type),use_key(usekey){}
    public:
    const bool use_key;
    const auto get_type()noexcept{return type;};
    internal_type_patter* cast_up(){if(use_key)return static_cast<internal_type_patter*>(this);else return nullptr;}
    internal_type(vct::Type type):type(type),use_key(0){}
};
class internal_type_patter;
class internal_type_object;
class internal_type_patter_object;
class internal_type_patter:public internal_type{
    protected:
    std::string key;
    explicit internal_type_patter(std::string key,vct::Type type):key(key),internal_type(type,1){}
    public:
    const auto& get_key()noexcept{return key;};
    //explicit internal_type_patter(std::string key,vct::Type type):key(key),internal_type(type,1),primitif_object_array(0){}
};
/**class internal_type_patter_object final:public internal_type_patter{
    private:
    bool is_array;
    std::vector<internal_type*> inside;

    public:
    my__iterator(inside)
    const size_t size(){return inside.size();};
    const internal_type_patter* get(const size_t& key) const {
        return inside[key];
    }
    explicit internal_type_patter_object(const std::vector<internal_type_patter*>& inside){
    }
};**/
class TypePattern{
    private:
    std::string key;
    v_component_type::Type type;
    public:
    void dump(std::vector<unsigned char>& buffer){
        string_short_to_buffer_bigendian(key,buffer);
        to_buffer_bigendian<unsigned char>(static_cast<unsigned char>(type),buffer);
    }
    static bool is_valid(ZT_buffer,size_t& offset){
        size_t len=0;
        if(!zt::Internal::parse::checkStringBigendian(buffer,offset,len))return false;
        offset+=len;
        if(!zt::Internal::parse::checkPrimitiveBigendian<unsigned char>(buffer,offset)) return false;
        unsigned char tmp;
        buffer_bigendian_to(buffer,offset,tmp);
        if(v_component_type::is_valid(static_cast<v_component_type::Type>(tmp))){
            return true;
        }else{
            debug_print("tipe data tak terbaca type:"<<tmp);
            return false;
        }
    }
    void parse(ZT_buffer,size_t& offset){
        buffer_bigendian_to_string_short(buffer,offset,key);
        unsigned char tmp;
        buffer_bigendian_to(buffer,offset,tmp);
        if(v_component_type::is_valid(static_cast<v_component_type::Type>(tmp))){
            type=static_cast<v_component_type::Type>(tmp);
        }else{
            throw std::exception("tipe data tak terbaca saat parse type pattern");
        }
    }
};
class v_component_Object final:public v_component{
    private:
    std::unordered_map<std::string,v_component*>value;
    public:
    inline const static cct type=cct::Object;
    void dump(std::vector<unsigned char>& buffer,const std::vector<TypePattern>& pattern);
    static bool is_valid(ZT_buffer,size_t& offset);
    void parse(ZT_buffer,size_t& offset);
    using iterator = decltype(value)::iterator;
    using const_iterator = decltype(value)::const_iterator;
    iterator begin() noexcept { return value.begin(); }
    iterator end()   noexcept { return value.end(); }
    const_iterator begin() const noexcept { return value.begin(); }
    const_iterator end()   const noexcept { return value.end(); }
    const_iterator cbegin() const noexcept { return value.cbegin(); }
    const_iterator cend()   const noexcept { return value.cend(); }
    const v_component* get(const std::string& key) const {
        auto it = value.find(key);
        if (it == value.end()) return nullptr;
        return it->second;
    }
    void set(const std::string& key, v_component* v) {
        value[key] = v;
    }
    explicit v_component_Object(std::unordered_map<std::string,v_component*> value):value(value),v_component(type){}
    v_component*& operator[](const std::string key){
        return value[key];
    }
};

class v_component_Array final:public v_component{
    private:
    std::vector<v_component*>value;
    const cct internalType;
    public:
    inline const static cct type=cct::Array;
   
    using iterator = decltype(value)::iterator;
    using const_iterator = decltype(value)::const_iterator;
    iterator begin() noexcept { return value.begin(); }
    iterator end()   noexcept { return value.end(); }
    const_iterator begin() const noexcept { return value.begin(); }
    const_iterator end()   const noexcept { return value.end(); }
    const_iterator cbegin() const noexcept { return value.cbegin(); }
    const_iterator cend()   const noexcept { return value.cend(); }
    void dump(std::vector<unsigned char>& buffer){
        dynamic_to_buffer_bigendian(value.size(),buffer);
        to_buffer_bigendian<unsigned char>(static_cast<unsigned char>(internalType),buffer);
        for(auto a:value){
            if(a->get_type()!=internalType);
        }
    };
    static bool is_valid(ZT_buffer,size_t& offset);
    void parse(ZT_buffer,size_t& offset){

    };
    void set(const size_t& index, v_component* v) {
        value[index] = v;
    }
    explicit v_component_Array(std::vector<v_component*> value,cct internalType):value(value),internalType(internalType),v_component(type){}
    explicit v_component_Array(cct internalType):value(),internalType(internalType),v_component(type){}
    v_component*& operator[](const size_t key){
        return value[key];
    }
};