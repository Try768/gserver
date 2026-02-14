#pragma once
class registry;
class TileData;
class EntityData;
class chunkmap;
class Entity;
class Tile;
class Player;
class EntityManager;
class PlayerManager;
namespace executionProses{
    void earlyExecution();
    void tickExecution();
    void randomTick();
    void cleanUpExecution();
};