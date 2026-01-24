#pragma once
#include "component.hpp"
#include "../forward.hpp"
#include "list.hpp"
class Entity{
    friend class registry;
    friend class chunkmap;
    EntityData data;
    std::string name;
    Coord<long long> chunk;
    unsigned char flag;
    
    Entity(EntityData data,std::string name):data(data),name(name){}
    public:
    inline const Coordinat getCoordinat()const{
        return Coordinat(data.getlocalcoord(),chunk);
    }
    //may throw error if not found
    const EntityComponent& getEntityComponent();
    const long long getMaxHealt()const;
    const long long getHealt()const;
    const std::string& get_name()const{
        return name;
    }
    bool setHealt(const long long)const;
    //relative to entity
    bool applyImpuls(Coord<int> impuls);
    Coord<int> getVelocity();
    void clearVelocity();
    void teleport(Coordinat destination);
    void jump(unsigned int power);
    void walk(bool direction,unsigned int speed);
};