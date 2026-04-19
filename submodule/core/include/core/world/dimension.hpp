#pragma once
#include "core/chunk/chunk.hpp"
#include "core/map/Area.hpp"
class Dimension:public Dynamic_Property_Parent{
    explicit Dimension(const std::string& dimensionName,Room& room):room(room),entityManager(*this),name(dimensionName){
    }
    Room& room;
    std::string name;
    EntityManager entityManager;
    Area2d area;
    public:
    const Room& getRoom()const{return room;}
    unsigned long long spawnEntity(const std::string&);
    Entity getEntity(unsigned long long);
    std::vector<Entity> getEntites(Coordinat begin,Coordinat end);
    bool despawnEntity(unsigned long long);
    bool fillTiles(Coordinat begin,Coordinat end,const std::string& TileName);
    Tile getTile(Coordinat);
    using time_point=std::chrono::steady_clock::time_point;
    void simulate(Registry& reg,time_point time_pivot){
        entityManager.simulate(reg,time_pivot);
        for(auto& chunk:area){
            if(chunk.second->is_load())chunk.second->simulate(reg,time_pivot);
        }
    }
};