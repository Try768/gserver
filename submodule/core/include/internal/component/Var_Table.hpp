#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "../util.hpp"
#define my__iterator(x) using iterator = decltype(x)::iterator; \
    using const_iterator = decltype(x)::const_iterator;\
    iterator begin() noexcept { return x.begin(); }\
    iterator end()   noexcept { return x.end(); }\
    const_iterator begin() const noexcept { return x.begin(); }\
    const_iterator end()   const noexcept { return x.end(); }\
    const_iterator cbegin() const noexcept { return x.cbegin(); }\
    const_iterator cend()   const noexcept { return x.cend(); }
#define my_property_(type,name,geter) private: type name; public: const type geter()const{return name;} type geter(){return name;}
#define my_property_ptr(type,name,geter) private: type* name; public: const type geter(){return name}
class Var_component;
class Var_component_Number;
class Var_component_String;
class Var_component_Bool;
class Var_component_Object;
class Var_component_Desimal;
class Var_component_Array;
namespace Var_component_type{
    enum class Type:unsigned char { None,Number,String,Bool,Object,Desimal,Array};
    template<Type T> struct internal; // primary (kosong)
    template<Type T> struct Params; // primary (kosong)
    template<Type T> struct  inval;
    template<>struct internal<Type::Number> {
            using type = Var_component_Number;
        };
        template<> struct internal<Type::Desimal> {
            using type = Var_component_Desimal;
        };
        template<> struct internal<Type::String> {
            using type = Var_component_String;
        };
        template<> struct internal<Type::Bool> {
            using type = Var_component_Bool;
        };
        template<> struct internal<Type::Object> {
            using type = Var_component_Object;
        };
        template<>struct internal<Type::None> {
            using type = Var_component;
        };
        template<>struct internal<Type::Array> {
            using type = Var_component_Array;
        };
        template<>struct Params<Type::Number> {
            using type =long long&;
        };
        template<> struct Params<Type::Desimal> {
            using type = double&;
        };
        template<> struct Params<Type::String> {
            using type = std::string&;
        };
        template<> struct Params<Type::Bool> {
            using type = bool&;
        };
        template<> struct Params<Type::Object> {
            using type = std::unordered_map<std::string,Var_component*>&;
        };
        template<>struct Params<Type::Array> {
            using type = std::vector<Var_component*>&;
        };
        template<>struct inval<Type::Number> {
            using type =long long;
        };
        template<> struct  inval<Type::Desimal> {
            using type = double;
        };
        template<> struct inval<Type::String> {
            using type = std::string;
        };
        template<> struct inval<Type::Bool> {
            using type = bool;
        };
        template<> struct inval<Type::Object> {
            using type = std::unordered_map<std::string,Var_component*>;
        };
        template<>struct inval<Type::Array> {
            using type = std::vector<Var_component*>;
        };
};