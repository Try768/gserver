#include <caster.h>
#include <json.hpp>
//#include "internal/datatype.hpp"
#include <map/loader.hpp>
#include <internal/internal.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <limits>

int main(int argc, char const *argv[])
{
    std::cout<<"Test Map Loader"<<std::endl;
    try{
    playerlist maincharacter(Coord<unsigned int>(1,5),Coord<long long>(0,0),"asep");
    if(maincharacter.setnametest()){
        if(registry::addplayer(maincharacter)){
            std::cout<<"Player Added: "<<registry::getplayerbyname("asep").getname()<<std::endl;
        }else std::cout<<"player not added\n";
    }else{
        std::cout<<"uhh\n";
        return 1;
    }

    EntityComponent entity("nganu:npc",{});
    registry::entityregister(entity);
    std::cout<<"Entity Added: "<<entity.getData().idname<<std::endl;
    registry::tileregister(tilecomponent("air",0));
    registry::tileregister(tilecomponent("stone",0));
    registry::tileregister(tilecomponent("dirt",0));
    registry::tileregister(tilecomponent("grass",0));
    std::cout<<"4 tile registered"<<std::endl;
    gameMap mapg;
    mapg.setMapDir("testmap");
    std::cout<<"Map Directory Set"<<std::endl;
    IndeksArea2d pointerArea;
    mapg.getareaIDforplayer(registry::getplayerbyname("asep"),3,pointerArea);
    
    std::cout<<"Jumlah y chunk: "<<pointerArea.usedChunkIds.size()<<std::endl;
    bool runing=true;
    while(runing){
        std::string command;
        std::cout<<">>";
        std::getline(std::cin,command);
        if(command=="exit"){
            runing=false;
        }else if(command.rfind("setTile ",0)==0){
            size_t x,y,id;
            sscanf_s(command.c_str(),"setTile %zu %zu %zu",&x,&y,&id);
            auto chunkptr=mapg.getchunkptr(x/16,y/16);
            if(chunkptr){
                if(id>=chunkptr->getTileTypeCount()){
                    std::cout<<"tile id "<<id<<" not found\n";
                    continue;
                }
                tilelist tile(id);
                tile.setupComponent(chunkptr->getTileType(id).getData().c_size);
                chunkptr->setTile(x%16,y%16,tile);
            }else{
                std::cout<<"chunk ("<<x/16<<","<<y/16<<") not found\n";
                std::cout<<"you can try to render it first\n";
            }
        }else if(command.rfind("getTileTypeList ",0)==0){
            size_t x,y;
            sscanf_s(command.c_str(),"getTileTypeList %zu %zu",&x,&y);
            auto chunkptr=mapg.getchunkptr(x,y);
            if(chunkptr){
                std::cout<<"Tile Type List:\n";
                for(size_t i=0;i<chunkptr->getTileTypeCount();i++){
                    auto tiletype=chunkptr->getTileType(i).getData();
                    std::cout<<"ID: "<<i<<", Name: "<<tiletype.name<<", Component Size: "<<(unsigned int)tiletype.c_size<<std::endl;
                }
            }
        }else if(command.rfind("getplayerspos",0)==0){
            auto& players=registry::getallplayer();
            for(auto& player:players){
                auto pos=player.second.getchunkcoord();
                auto loc=player.second.getlocalcoord();
                std::cout<<"player: "<<player.first<<", chunk coord: ("<<pos.x<<","<<pos.y<<"), local coord: ("<<loc.x<<","<<loc.y<<")\n";
            }
        }else if(command.rfind("renderplayerchunk ",0)==0){
            size_t radius=5;
            std::string playername;
            //sscanf_s(command.c_str(),"renderplayerchunk %s %zu",&playername,&radius);
            std::string temp;
            command.erase(0,18);
            size_t pos=0;
            size_t endpos=command.size();
            while(pos<endpos){
                size_t nextpos=command.find(' ',pos);
                if(nextpos==std::string::npos)nextpos=endpos;
                temp=command.substr(pos,nextpos-pos);
                if(temp=="renderplayerchunk")continue;
                else if(temp.find_first_not_of("0123456789")==std::string::npos){
                    radius=std::stoull(temp);
                }else if(temp.rfind("\"")==0){
                    temp=command.substr(pos+1);
                    size_t quoteend=temp.find('"');
                    playername=temp.substr(0,quoteend);
                    nextpos=pos+1+quoteend+1;
                }else{
                    playername=temp;
                }
                pos=nextpos+1;
            }
            IndeksArea2d pointerArea;
            mapg.getareaIDforplayer(registry::getplayerbyname(playername),radius,pointerArea);
            for(auto& row: pointerArea.usedChunkIds){
                for(auto& col: row.second.usedChunkIds){
                   mapg.storeAndGenerateChunk(col,row.first);
                }
            }
        }else if(command.rfind("showchunk ",0)==0){
            size_t x,y;
            sscanf_s(command.c_str(),"showchunk %zu %zu",&x,&y);
            auto chunkptr=mapg.getchunkptr(x,y);
            if(chunkptr){
                std::cout<<"Chunk ("<<x<<","<<y<<") loaded.\n";
                for(size_t j=0;j<chunkmap::sizey;j++){
                    for(size_t i=0;i<chunkmap::sizex;i++){
                        auto tile=chunkptr->getTile(i,j);
                        std::cout<<(unsigned int)tile.getIdinChunk()<<" ";
                    }
                    std::cout<<std::endl;
                }
            }else{
                std::cout<<"Chunk ("<<x<<","<<y<<") not found.\n";
            }
        }else if(command.rfind("saveall",0)==0){
            mapg.mapfile->save_all();
            std::cout<<"All modified chunk saved.\n";

        }
        else{
            std::cout<<"Unknown Command\n";
        }

            
    }
    }catch(const std::exception& e){
        std::cout<<"Error caught: "<<e.what()<<std::endl;
    }
    //if(cm.tilesBuffer.size()<chunkmap::sizex*chunkmap::sizey){
    //    return 1;
    //}
            std::cout<<std::endl;
    
    return 0;

}
