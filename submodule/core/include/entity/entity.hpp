#pragma once
#include "component.hpp"
#include "../forward.hpp"
#include "list.hpp"
class SimulatedEntity;
class Entity{
    protected:
    friend class registry;
    friend class chunkmap;
    friend class EntityManager;
    friend class SimulatedEntity;
    template<class T>
    using opt=zt::Internal::util::optional<T>;
    unsigned long long ID;
    EntityData& data;
    Entity(EntityData* data,unsigned long long ID):data(*data),ID(ID){
        assert(data != nullptr);
    }
    public:
    inline const Coordinat getCoordinat()const{
        return Coordinat(data.getlocalcoord(),data.getchunkcoord());
    }
    //may throw error if not found
    EntityComponent getEntityComponent();
    const long long getMaxHealt()const;
    const long long getHealt()const;
    opt<std::string> get_name()const{
        if(!data.indeks.is_valid())return opt<std::string>();
        return opt<std::string>(data.indeks.get_name());
    }
    bool setHealt(const long long)const;
    const bool is_valid(){
        return data.flag&EntityData::status_flag::is_valid;
    }
    
    //relative to entity
    bool applyImpuls(Coord<double> impuls);
    Coord<int> getVelocity();
    void clearVelocity();
    void teleport(Coordinat destination);
    void jump(double power);
    void walk(bool direction,double speed);
    
    Coordinat checkTileAhead();
    void setDynamicProperty(const std::string& key,const MultiValue& value){
        data.dynamic_property.data[key]=value;
    }
    opt<MultiValue> getDynamicProperty(const std::string& key)const{
        auto it=data.dynamic_property.data.find(key);
        if(it==data.dynamic_property.data.end()){
            return opt<MultiValue>();
        }
        return opt<MultiValue>(it->second);
    }
    bool clearDynamicProperty(const std::string& key){
        auto it=data.dynamic_property.data.find(key);
        if(it==data.dynamic_property.data.end()){
            return false;
        }
        data.dynamic_property.data.erase(it);
        return true;
    }
    template<Var_component_type::Type T>
    opt<Var_component> getRuntimePropertyRef(const std::string& key){
        auto itc=data.runtime_property.find(key);
        if(itc==data.runtime_property.end()){
            return opt<Var_component>();
        }
        Var_component_type::internal<T>::type ptr=itc->second->getCast<T>();
        if(ptr==nullptr){
            return opt<Var_component>();
        }
        return opt<Var_component>(ptr);
    }
    template<Var_component_type::Type T>
    typename opt<Var_component_type::Params<T>::type> getRuntimePropertyVal(const std::string& key)const{
        return data.runtime_property.getAs<T>(key);
    }
    template<Var_component_type::Type T>
    bool setRuntimeProperty(const std::string& key,Var_component_type::Params<T>::type value){
        return data.runtime_property.recreate<T>(key,value);
    }
    bool clearRuntimeProperty(const std::string& key){
        auto it=data.runtime_property.find(key);
        if(it==data.runtime_property.end()){
            return false;
        }
        delete it->second;
        data.runtime_property.erase(it);
        return true;
    }
};
class SimulatedEntity:public Entity{
    friend class registry;
    friend class chunkmap;
    friend class EntityManager;
    SimulatedEntity(EntityData* data,unsigned long long ID):Entity(data,ID){
    }
    SimulatedEntity(const Entity& data):Entity(data){
    }
    public:
    Entity& getEntity(){
        return *this;
    }
    const Entity& getEntity()const{
        return *this;
    }
    //this will call listener
    void damageOther(Entity& other,unsigned int damage);
    //this will call listener
    void damageOther(Player& other,unsigned int damage);
    //this will call listener
    void hurt(unsigned int damage,std::string&& reason);
    //this will call listener
    void hurt(unsigned int damage,const std::string& reason){
        hurt(damage,std::string(reason));
    }
    //this will call listener
    void hurt(unsigned int damage){
        hurt(damage,"");
    }
};