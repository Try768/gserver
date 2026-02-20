#pragma once
#include "core/chunk/chunk.hpp"
class Dimension{
    explicit Dimension(const std::string& dimensionName,Room& room);
    EntityManager entittManager;
    std::unordered_map<Coord<long long>,std::unique_ptr<chunkmap>> chunks;
    public:
    unsigned long long spawnEntity(const std::string&);
    Entity getEntity(unsigned long long);
    std::vector<Entity> getEntites(Coordinat begin,Coordinat end);
    bool despawnEntity(unsigned long long);
    bool fillTiles(Coordinat begin,Coordinat end,const std::string& TileName);
    Tile getTile(Coordinat);
    
};