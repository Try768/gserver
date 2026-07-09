#pragma once
#include <chrono>
#include "../register/register.hpp"
#include "room.hpp"

class World:public Dynamic_Property_Parent{
    using ID=unsigned long long;
    IDMaker<ID> idmaker;
    std::unordered_map<ID,Room> rooms;
    Registry reg;
    using clock=std::chrono::steady_clock;
    std::string mainFolder;
    clock::time_point deltaTime;
    void resetClock(){deltaTime =clock::now();}
    public:
    double getDeltaClock(){
        auto now =clock::now();
        return std::chrono::duration<double,std::milli>(now-deltaTime).count();
    }
    using time_point =std::chrono::steady_clock::time_point;
    static double getStaticDeltaClock(time_point pivot,time_point now){
        return std::chrono::duration<double,std::milli>(now-pivot).count();
    }
    World(
        void (*entityEventAndComponentRegister)(zt::callback::EntityEventListener&),
        void (*tileEventAndComponentRegister)(zt::callback::TileEventListener&),
        void (*playerEventAndComponentRegister)(zt::callback::PlayerEventListener&),
        void (*objectRegister)(Registry&),const std::string& ParentDirectory
    ):mainFolder(ParentDirectory){
       entityEventAndComponentRegister(reg.eel);
       tileEventAndComponentRegister(reg.tel);
       playerEventAndComponentRegister(reg.pel);
       objectRegister(reg);
    }
    inline const Registry& getRegister()const{return reg;}
    ID createRoom(std::vector<std::pair<std::string,std::unique_ptr<Dimension>>> dimensions,
        World& world,bool is_admin=false){
        ID id;
        if(!this->idmaker.getID(id))throw std::runtime_error("too much");
        rooms.emplace(id,dimensions,world,is_admin);
    }
    void destroyRooms(ID id){
        idmaker.destroyID(id);
        auto itc =rooms.find(id);
        if(itc!=rooms.end())rooms.erase(itc);
    }
    const std::string& getParentDir()const{
        return this->mainFolder;
    }
    auto simulate(){
        resetClock();
        //do some thing
        auto pivot =clock::now();
        for(auto& room:rooms){
            room.second.simulate(reg,pivot);
        }
        //end do something
        auto nnow=clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(nnow-deltaTime);
    }
};