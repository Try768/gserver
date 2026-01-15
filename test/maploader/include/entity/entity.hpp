#pragma once
#include "component.hpp"
#include "../forward.hpp"
#include "list.hpp"
#include "../register/register.hpp"
class Entity{
    friend class registry;
    friend class chunkmap;
    const IndeksEntityComponent indeks;
    const EntityData data;
    Coord<long long> chunk;
    Entity(EntityData data,IndeksEntityComponent indeks):indeks(indeks),data(data){}
    public:
    const Coordinat getCoordinat(){
        return Coordinat(data.getlocalcoord(),chunk);
    }
    //may throw error if not found
    const EntityComponent& getEntityComponent(){
        return registry::getEntities().at(indeks.get_id());
    }
};