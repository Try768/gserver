#pragma once
#include "filemap.hpp"

class gameMap:public checksumparent{
    
private:
    /* data */
    Area2d internalArea;
    UsedArea2d internalUsedArea;
    std::string mapName;
    constexpr static std::array<unsigned char,8> mapSignature ={4,5,6,255,43,57,72,12};
public:
    MapOption mapOptions;
    MapFileParent mapfile;
    const std::string& getMapName()const{
        return mapName;
    }
    const Area2d getarea()const{
        return internalArea;
    }
    const UsedArea2d& getUsedArea()const{
        return internalUsedArea;
    }
    void setMapDir(std::string dir){
        if(!fsmanager::directory::exists(dir)){
            fsmanager::directory::create(dir);
        }
        if(!fsmanager::directory::exists(dir+"/option.json")){
            MapOption opsi;
            nlohmann::json jsopsi;
            jsopsi["map_name"]=opsi.mapName;
            jsopsi["author"]=opsi.author;
            jsopsi["version"]=opsi.version;
            jsopsi["max_player"]=opsi.maxPlayers;
            jsopsi["is_beta"]=opsi.beta;
            jsopsi["gen_opsi"]["seed"]=opsi.genOption.getSeed();
            jsopsi["gen_opsi"]["is_using_generator"]=opsi.genOption.getUseGenerator();
            mapOptions=opsi;
        }else{
            nlohmann::json jsopsi=nlohmann::json::parse(fsmanager::file::read(dir+"/option.json"));
            if(!mapOptions.getjson(jsopsi)){
                std::cout<<"error parsing map option\n";
            }
        }
        if(!fsmanager::file::exists(dir+"/main.map")){
            std::vector<unsigned char> buffer;
            buffer.reserve(mapSignature.size()+mapName.size()+1);
            buffer.insert(buffer.end(),mapSignature.begin(),mapSignature.end());
            fsmanager::file::writebin(dir+"/main.map",buffer);
        }
    }
    /***
     * x adalah koordinat chunk bukan dalam chunk
     */
    chunkmap chunkgenerator(size_t x,size_t y){
        std::cout<<"chunkgen("<<x<<","<<","<<y<<")\n";
        chunkmap newchunk;
        auto& tile=registry::getTiles();
        if(tile.size()<2){
            std::cout<<"tile kurang dari 2\n";
            //std::vector<tilelist> emptychunk(chunkmap::sizex*chunkmap::sizey,tilelist(0));
            //newchunk.tilesBuffer=emptychunk;
            return newchunk;
        }
        newchunk.addTileTypes(tile.begin(),tile.begin()+2);
        //auto& buffer =newchunk.tilesBuffer;
        //buffer.reserve(chunkmap::sizex*chunkmap::sizey);
        for (size_t i = 0; i < chunkmap::sizey; i++)
        {
            for (size_t j = 0; j < chunkmap::sizex; j++)
            {
                if(i<90){
                    tilelist air(0);
                    air.setComponent(newchunk.getTileType(0).getData().c_size);
                    newchunk.setTile(j,i,air);
                }else if(i<100){
                    tilelist dirt(1);
                    dirt.setComponent(newchunk.getTileType(1).getData().c_size);
                    newchunk.setTile(j,i,dirt);
                }else{
                    tilelist stone(2);
                    stone.setComponent(newchunk.getTileType(2).getData().c_size);
                    newchunk.setTile(j,i,stone);
                }
            }
            
        }
        if(y>12){
            if(registry::getEntities().size()>0){
                int i =0;
                for (auto& entity : registry::getEntities())
                {
                    
                    auto& data=entity.second.getData();
                    auto& entitybuffer=newchunk.entitiesBuffer;
                    entitylist ent(data.id,entity.second.getDefaultComponent(),Coord<unsigned int>((i*3*24/2)%1600,i*x%1600));
                    entitybuffer.push_back(ent);
                }

            }
        }
        
        return newchunk;
    }
    
    inline void storeAndGenerateChunk(long long x,long long y){
        
       std::cout<<"generatechunk("<<y<<","<<x<<")\n";
        if(internalUsedArea.is_found(x,y)){
            internalArea.set(x,y,chunkgenerator(x,y));
            internalUsedArea.usedChunkIds[y].usedChunkIds.insert(x);
        }else{
            return;
        }
        return;
    }
    inline void storeGeneratedChunk(long long x,long long y,const chunkmap& chunk){
        if(internalUsedArea.is_found(x,y)){
            internalArea.set(x,y,chunk);
            internalUsedArea.usedChunkIds[y].usedChunkIds.insert(x);
        }
    }
    void getareaIDforplayer(const playerlist& player,unsigned short radius,UsedArea2d& pointerArea){
        std::cout<<"chunk,rad,sign"<<","<<radius<<","<<player.getchunkcoord().x<<","<<player.getchunkcoord().y<<std::endl;
        for (long long i = player.getchunkcoord().y-radius; i < player.getchunkcoord().y + radius; i++)
        {
            unsigned short beda=std::abs(radius-i);
            for (long long j = player.getchunkcoord().x - beda; j < player.getchunkcoord().x + beda; j++)
            {
                    pointerArea.usedChunkIds[i].usedChunkIds.insert(j);
            }
        }
        
    }
    const bool getChunkByID(long long x,long long y,chunkmap& temp){
        if(internalUsedArea.is_found(x,y)){
            temp=internalArea.get(x,y);
            return true;
        }
        temp = chunkgenerator(x,y);
        return 0;
    }
    void getChunkRefDumpForPlayer(const playerlist& player,unsigned short radius,std::vector<unsigned char>& buffer){
        UsedArea2d tempo;
        getareaIDforplayer(player,radius,tempo);
        internalArea.dump_ref(buffer,tempo);
    }
    gameMap(){}
};