#pragma once
#include <unordered_map>
#include <string>
#define my__iterator(x) using iterator = decltype(x)::iterator; \
    using const_iterator = decltype(x)::const_iterator;\
    iterator begin() noexcept { return x.begin(); }\
    iterator end()   noexcept { return x.end(); }\
    const_iterator begin() const noexcept { return x.begin(); }\
    const_iterator end()   const noexcept { return x.end(); }\
    const_iterator cbegin() const noexcept { return x.cbegin(); }\
    const_iterator cend()   const noexcept { return x.cend(); }
#define my_property_(type,name,geter) private: type name; public: const type geter()const{return name;}
#define my_property_ptr(type,name,geter) private: type* name; public: const type geter(){return name}
class Const_component;
class Const_component_Number;
class Const_component_String;
class Const_component_Bool;
class Const_component_Object;
class Const_component_Desimal;
class Const_component_Array;
namespace Const_component_type{
    enum class Type:unsigned char { None,Number,String,Bool,Object,Desimal,Array};
        template<Type T> struct internal; // primary (kosong)
        template<>struct internal<Type::Number> {
            using type = Const_component_Number*;
        };
        template<> struct internal<Type::Desimal> {
            using type = Const_component_Desimal*;
        };
        template<> struct internal<Type::String> {
            using type = Const_component_String*;
        };
        template<> struct internal<Type::Bool> {
            using type = Const_component_Bool*;
        };
        template<> struct internal<Type::Object> {
            using type = Const_component_Object*;
        };
        template<>struct internal<Type::None> {
            using type = void*;
        };
        template<>struct internal<Type::Array> {
            using type = Const_component_Array*;
        };
};
class Const_component{
    protected:
    using cct=Const_component_type::Type;
    cct type=cct::None;
    explicit Const_component(cct Type=cct::None):type(type){}
    public:
    cct get_type()const{return type;}
    template<cct T>
    typename Const_component_type::internal<T>::type getCast(){
        if (T!=get_type())return nullptr;
        return static_cast<Const_component_type::internal<T>::type>(this);
    }
};
class Const_component_Number final:public Const_component{
    private:
    long long value;
    public:
    const long long get()const{return value;}
    explicit Const_component_Number(long long value):value(value),Const_component(cct::Number){}
    inline const static cct type=cct::Number;
};
class Const_component_Desimal final:public Const_component{
    private:
    public:
    my_property_(double,value,get)
    explicit Const_component_Desimal(double value):value(value),Const_component(cct::Desimal){}
};
class Const_component_String final:public Const_component{
    private:
    public:
    my_property_(std::string,value,get)
    explicit Const_component_String(std::string value):value(value),Const_component(cct::String){}
};
class Const_component_Bool final:public Const_component{
    private:
    my_property_(bool,value,get)
    public:
    explicit Const_component_Bool(bool value):value(value),Const_component(cct::Bool){}
};
class Const_component_Object final:public Const_component{
    private:
    std::unordered_map<std::string,Const_component*>value;
    public:
    
    using iterator = decltype(value)::iterator;
    using const_iterator = decltype(value)::const_iterator;
    //iterator begin() noexcept { return value.begin(); }
    //iterator end()   noexcept { return value.end(); }
    const_iterator begin() const noexcept { return value.begin(); }
    const_iterator end()   const noexcept { return value.end(); }
    const_iterator cbegin() const noexcept { return value.cbegin(); }
    const_iterator cend()   const noexcept { return value.cend(); }
    Const_component* get(const std::string& key) const {
        auto it = value.find(key);
        if (it == value.end()) return nullptr;
        return it->second;
    }
    const_iterator find(std::string key)const noexcept{return value.find(key);}
    //void set(const std::string& key, Const_component* v) {
    //    value[key] = v;
    //}
    explicit Const_component_Object(std::unordered_map<std::string,Const_component*> value):value(value),Const_component(cct::Object){}
    explicit Const_component_Object():value(),Const_component(cct::None){}
    const Const_component*& operator[](const std::string key){
        return value[key];
    }
};
class Const_component_Array final:public Const_component{
    private:
    std::vector<Const_component*>value;
    public:
    inline const static cct type=cct::Object;
    using iterator = decltype(value)::iterator;
    using const_iterator = decltype(value)::const_iterator;
    //iterator begin() noexcept { return value.begin(); }
    //iterator end()   noexcept { return value.end(); }
    const_iterator begin() const noexcept { return value.begin(); }
    const_iterator end()   const noexcept { return value.end(); }
    const_iterator cbegin() const noexcept { return value.cbegin(); }
    const_iterator cend()   const noexcept { return value.cend(); }
    Const_component* get(const size_t key) const {
        if(key>value.size())return nullptr;
        return value[key];
    }
    //void set(const std::string& key, Const_component* v) {
    //    value[key] = v;
    //}
    explicit Const_component_Array(std::vector<Const_component*>value):value(value),Const_component(cct::Array){}
    const Const_component*& operator[](const size_t key){
        return value[key];
    }
};