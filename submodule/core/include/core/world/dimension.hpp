#pragma once
#include "core/chunk/chunk.hpp"
#include "core/map/Area.hpp"

class Dimension:public Dynamic_Property_Parent{
    friend class EntityManager;
    explicit Dimension(const std::string& dimensionName,
        Room& room,World& world,const std::string& dimensionDir);
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
    void loadChunks(const std::vector<Coord<long long>>& chunksCoord);
    void loadChunk(const Coord<long long>& chunkCoord);
    void unLoadChunks(const std::vector<Coord<long long>>& chunksCoord);
    void unLoadChunk(const Coord<long long>& chunkCoord);
    bool isChunkLoaded(Coord<long long>& chunkCoord);
    Tile getTile(Coordinat);
    using time_point=std::chrono::steady_clock::time_point;
    void simulate(Registry& reg,time_point time_pivot){
        entityManager.simulate(time_pivot);
        for(auto& chunk:area){
            if(chunk.second->is_load())chunk.second->simulate(reg,time_pivot);
        }
    }
};

class Room;
class DimensionAccses{
    friend class Room;
    private:
    Dimension& dimension;
    DimensionPermision permision;
    using optDim = zt::Internal::util::OptionalRef<Dimension>;
    using CoptDim = zt::Internal::util::ConstOptionalRef<Dimension>;
    Dimension& getInternalAcsess(){return dimension;}
    public:
    explicit DimensionAccses(Dimension& dimension,DimensionPermision permision):dimension(dimension),permision(permision){}
    //can only accses if you get admin permision
    optDim get(){if(permision==DimensionPermision::readAndWrite) optDim(dimension);
        return optDim();
    }
    const Dimension& getConst()const{return dimension;}
};