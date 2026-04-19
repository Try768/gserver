#pragma once
#include "../common.hpp"
#include "core/eventListener/eventType.hpp"
#include "core/internal/component/var.hpp"
#include "../internal/datatype.hpp"
#include <vector>
#include "../forward.hpp"
#include <functional>
#define _MY_LAMBDA_(parameter) [](parameter)
#define _Custom_Component_Lambda(expression) [](EntityData*,Const_component)->bool{ expression };
struct Internal_c{
       unsigned char Type;
        std::string Name;
        Internal_c(unsigned char Type,std::string Name):Type(Type),Name(Name)
        {}
        Internal_c()=default;
    };
namespace zt::callback{
    template<zt::event::entity::Type T>
    class ComponentRegisterofEntity;
}
class IndeksEntityComponent;
class EntityComponent
{
private:
    friend class Registry;
    friend class EntityData;
    friend class Entity;
    friend class chunkmap;
    friend class IndeksEntityComponent;
    template<zt::event::entity::Type T>
    friend class zt::callback::ComponentRegisterofEntity;
    /* data */
    std::string idname;
    public:
    struct Internal{
        using optionalCC = zt::Internal::util::optionalRef<Var_component>;
        friend class Registry;
        friend class EntityData;
        friend class chunkmap;
        friend class EntityComponent;
        //std::vector<Internal_c> inComponent;
        using ID=size_t;
        using RunComponent=std::array<std::vector<ID>,(size_t)zt::event::entity::Type::COUNT>;
        using optionalRC = zt::Internal::util::optionalRef<RunComponent>;
        private:
        RunComponent runComponent;
        const Var_component_Object var_component;
        public:
        optionalCC getCC(std::string key)const{
            auto itc=var_component.find(key);
            if(itc==var_component.end()){
                return optionalCC(*itc->second);
            }return optionalCC();
        }
        const RunComponent& getRC()const{
           return runComponent;
        }
        private:
        Internal(Var_component_Object&& const_component,RunComponent runComponent):
        var_component(std::move(const_component)),runComponent(runComponent)
        {}
    };
    private:
    Internal& internal;
    EntityComponent(
        const std::string& idname,Internal& CCO):
        idname(idname),internal(CCO)
        {}
    public:
    const auto& getInternal(){return internal;}
    //inline static std::unordered_set<unsigned long long> usedId={1};
    //entitycomponent(/* args */){}
    struct C_entityData{
        const std::string& idname;
        const Var_component_Object& c_component;
        const EntityComponent::Internal::RunComponent& runComponent;
    };
    C_entityData getData()const{
        return{idname,internal.var_component,internal.runComponent};
    }
    //EntityComponent()=default;
};
class IndeksEntityComponent{
    friend class Entity;
    friend class Registry;
    friend class EntityData;
    friend class chunkmap;
    private:
    EntityComponent::Internal* entityType;
    const std::string* idname;
    IndeksEntityComponent(EntityComponent::Internal& component,const std::string& name):
     entityType(&component),idname(&name){}
    IndeksEntityComponent(){
        entityType=nullptr;
        idname=nullptr;
    }
    public:
    bool is_valid()const{
        if(entityType==nullptr||idname==nullptr)return false;
        return true;
    }
    const std::string& get_name()const{
        #if indebug==1
        if(!is_valid())throw std::exception("belum di cek valid wak");
        #endif
        return *idname;
    }
    EntityComponent getComponent()const{
        #if indebug==1
        if(!is_valid())throw std::exception("belum di cek valid wak");
        #endif
        return EntityComponent(*idname,*entityType);
    }
    IndeksEntityComponent(const IndeksEntityComponent&) = default;
    IndeksEntityComponent& operator=(const IndeksEntityComponent&) = default;
    IndeksEntityComponent(IndeksEntityComponent&&) = default;
    IndeksEntityComponent& operator=(IndeksEntityComponent&&) = default;
    //this may throw error
    const EntityComponent::Internal& get_id()const{
        #if indebug==1
        if(!is_valid())throw std::exception("belum di cek valid wak");
        #endif
        return *entityType;
    }
};
/*
    Typein::Component getDefaultComponent()const{
        Typein::Component keluaran;
        for (size_t i = 0; i < inComponent.size(); i++)
        {
            switch (inComponent[i].Type)
            {
            case t_type_id::integer:
                keluaran[inComponent[i].Name]=static_cast<unsigned int>(0);
                break;
            case t_type_id::string:
                keluaran[inComponent[i].Name]=std::string("");
                break;
            case t_type_id::boolean:
                keluaran[inComponent[i].Name]=false;
                break;
            case t_type_id::loong:
                keluaran[inComponent[i].Name]=static_cast<unsigned long long>(0);
                break;
            case t_type_id::character:
                keluaran[inComponent[i].Name]=static_cast<unsigned char>(0);
                break;
            default:
                break;
            }
        }
        return keluaran;
    }
*/
//not updated
    //void ref_dump(std::vector<unsigned char>& buffer)const{
    //     to_buffer_bigendian<unsigned long long>(id,buffer);
    //    string_short_to_buffer_bigendian(idname,buffer);
    //    to_buffer_bigendian<unsigned short>((unsigned short)inComponent.size(),buffer);
    //    for (size_t i = 0; i < inComponent.size(); i++)
    //    {
    //        to_buffer_bigendian<unsigned char>(inComponent[i].Type,buffer);
    //        string_short_to_buffer_bigendian(inComponent[i].Name,buffer);
    //    }
    //}
    //static bool is_buffer_valid(const std::vector<unsigned char>& data,size_t& offset){
    //    using namespace zt::Internal;
    //    size_t len;
    //    debug_print("checking entitycomponent buffer validity at offset:"<<offset);
    //    if(!parse::checkPrimitiveBigendian<unsigned long long>(data,offset))return false;
    //    if(!parse::checkStringBigendian(data,offset,len))return false;
    //    offset+=len;
    //    unsigned short compSize;
    //    if(!parse::checkPrimitiveBigendian<unsigned short>(data,offset))return false;
    //    offset-=sizeof(unsigned short);
    //    buffer_bigendian_to<unsigned short>(data,offset,compSize);
    //    for (size_t i = 0; i < compSize; i++)
    //    {
    //        if(!parse::checkPrimitiveBigendian<unsigned char>(data,offset))return false;
    //        offset-=sizeof(unsigned char);
    //        unsigned char type;
    //        buffer_bigendian_to<unsigned char>(data,offset,type);
    //        size_t length;
    //        if(!parse::checkStringBigendian(data,offset,length))return false;
    //        offset+=length;
    //    }
    //    return true;
    //}
    //void parse(const std::vector<unsigned char>& data,size_t& offset){
    //    buffer_bigendian_to(data,offset,id);
    //    buffer_bigendian_to_string_short(data,offset,idname);
    //    unsigned short compSize;
    //    buffer_bigendian_to<unsigned short>(data,offset,compSize);
    //    inComponent.clear();
    //    inComponent.reserve(compSize);
    //    Internal_c temp;
    //    for (size_t i = 0; i < compSize; i++)
    //    {
    //        buffer_bigendian_to<unsigned char>(data,offset,temp.Type);
    //        buffer_bigendian_to_string_short(data,offset,temp.Name);
    //        inComponent.push_back(temp);
    //    }
    //}
    //EntityComponent(const std::vector<unsigned char>& data,size_t& offset){
    //    parse(data,offset);
    //}
    //std::vector<unsigned char> dump()const{
    //    std::vector<unsigned char> keluaran;
    //    ref_dump(keluaran);
    //    return keluaran;
    //}