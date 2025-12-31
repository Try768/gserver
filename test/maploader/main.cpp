#include "caster.h"
#include "json.hpp"
//#include "internal/datatype.hpp"
#include "map/loader.hpp"
#include "internal/internal.hpp"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <limits>

int main(int argc, char const *argv[])
{
    std::cout<<"Test Map Loader"<<std::endl;
    playerlist maincharacter(Coord<unsigned int>(1,5),Coord<long long>(0,0),"asep");
    if(maincharacter.setnametest()){
        if(registry::addplayer(maincharacter)){
            std::cout<<"Player Added: "<<registry::getplayerbyname("asep").getname()<<std::endl;
        }else std::cout<<"player not added\n";
    }else{
        std::cout<<"uhh\n";
        return 1;
    }

    entitycomponent entity(0,"nganu:npc",{});
    registry::entityregister(entity);
    std::cout<<"Entity Added: "<<entity.getData().idname<<std::endl;
    registry::tileregister(tilecomponent("air",0));
    registry::tileregister(tilecomponent("stone",0));
    registry::tileregister(tilecomponent("dirt",0));
    registry::tileregister(tilecomponent("grass",0));
    std::cout<<"4 tile registered"<<std::endl;
    gameMap mapg;
    UsedArea2d pointerArea;
    mapg.getareaIDforplayer(registry::getplayerbyname("asep"),3,pointerArea);
    
    std::cout<<"Jumlah y chunk: "<<pointerArea.usedChunkIds.size()<<std::endl;
    //if(cm.tilesBuffer.size()<chunkmap::sizex*chunkmap::sizey){
    //    return 1;
    //}
            std::cout<<std::endl;
    
    return 0;

}
