#pragma once
class Registry;
class TileData;
class EntityData;
class chunkmap;
class Entity;
class Tile;
class Player;
class EntityManager;
class PlayerManager;
class Room;
class Dimension;
class DimensionAccses;
class World;
namespace executionProses{
    void earlyExecution();
    void tickExecution();
    void randomTick();
    void cleanUpExecution();
};
class OriginWorld{
    World* world;
    Dimension* dimension;
    Room* room;
    public:
    inline void setWorld(World &val){world=&val;}
    inline void setDimension(Dimension &val){dimension=&val;}
    inline void setRoom(Room &val){room=&val;}
    inline const World& getWorld()const{return *world;}
    inline const Dimension& getDimension()const{return *dimension;}
    inline const Room& getRoom()const{return *room;}
    inline World& getWorldRef(){return *world;}
    inline Dimension& getDimensionRef(){return *dimension;}
    inline Room& getRoomRef(){return *room;}
    OriginWorld(World& world,Dimension& dimension,Room& room):
    world(&world),dimension(&dimension),room(&room){}
};