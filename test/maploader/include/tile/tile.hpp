#pragma once
#include "./component.hpp"
#include "./list.hpp"
class Tile{
    friend class registry;
    friend class chunkmap;
    TileData data;
    const std::string name;
    public:
    inline const IndeksTileComponent& get_indeks()const noexcept{return data.getIndeks();}
    inline const std::string& get_name()const noexcept{return name;}
    inline const unsigned int get_permutation()const noexcept{return data.getpermutation();}
    inline void setpermutation(unsigned int permutation){data.setPermutation(permutation);}
    Tile(TileData data,
    const std::string name):data(data),name(name){}

};