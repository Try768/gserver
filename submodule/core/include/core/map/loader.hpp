#pragma once
#include "filemap.hpp"

class gameMap:public checksumparent{
    
private:
    /* data */
    IndeksArea2d internalUsedArea;
    std::string mapName;
    Area2d internalArea;
    constexpr static std::array<unsigned char,8> mapSignature ={4,5,6,255,43,57,72,12};
    public:
    MapOption mapOptions;
    MapFileParent* mapfile;
    const std::string& getMapName()const{
        return mapName;
    }
    bool getchunk(long long x,long long y,chunkmap& temp){
        if(internalUsedArea.is_found(x,y)){
            temp=internalArea.get(x,y);
            return true;
        }
        return false;
    }
    chunkmap* getchunkptr(long long x,long long y){
        if(internalUsedArea.is_found(x,y)){
            std::cout<<"Getting chunk pointer at ("<<x<<","<<y<<")\n";
            return internalArea.get_ptr(x,y);
        }
        return nullptr;
    }
    const IndeksArea2d& getUsedArea()const{
        return internalUsedArea;
    }
    void setMapDir(std::string dir){
        if(!fsmanager::directory::exists(dir)){
            fsmanager::directory::create(dir);
        }
        if(!fsmanager::file::exists(dir+"/option.json")){
            MapOption opsi;
            nlohmann::json jsopsi;
            jsopsi["map_name"]=opsi.mapName;
            jsopsi["author"]=opsi.author;
            jsopsi["version"]=opsi.version;
            jsopsi["max_player"]=opsi.maxPlayers;
            jsopsi["is_beta"]=opsi.beta;
            jsopsi["gen_opsi"]["seed"]=opsi.genOption.getSeed();
            jsopsi["gen_opsi"]["is_using_generator"]=opsi.genOption.getUseGenerator();
            fsmanager::file::write(dir+"/option.json",jsopsi.dump(4));
            mapOptions=opsi;
        }else{
            nlohmann::json jsopsi=nlohmann::json::parse(fsmanager::file::read(dir+"/option.json"));
            if(!mapOptions.getjson(jsopsi)){
                std::cout<<"error parsing map option\n";
                mapOptions=MapOption();
            }
        }
        mapfile =new MapFileParent(dir,mapOptions,&internalArea,&internalUsedArea);
    }
    /***
     * x adalah koordinat chunk bukan dalam chunk
     */
    chunkmap* chunkgenerator(long long x,long long y){
        std::cout<<"chunkgen("<<x<<","<<","<<y<<")\n";
        chunkmap* newchunk = new chunkmap();
        auto& tile=registry::getTiles();
        if(tile.size()<2){
            std::cout<<"tile kurang dari 2\n";
            //std::vector<tilelist> emptychunk(chunkmap::sizex*chunkmap::sizey,tilelist(0));
            //newchunk.tilesBuffer=emptychunk;
            return newchunk;
        }
        newchunk->addTileTypes(tile.begin(),tile.begin()+3);
        //auto& buffer =newchunk.tilesBuffer;
        //buffer.reserve(chunkmap::sizex*chunkmap::sizey);
        for (size_t i = 0; i < chunkmap::sizey; i++)
        {
            for (size_t j = 0; j < chunkmap::sizex; j++)
            {
                if(i<90){
                    tilelist air(0);
                    air.setupComponent(newchunk->getTileType(0).getData().c_size);
                    newchunk->setTile(j,i,air);
                }else if(i<100){
                    tilelist dirt(1);
                    dirt.setupComponent(newchunk->getTileType(1).getData().c_size);
                    newchunk->setTile(j,i,dirt);
                }else{
                    tilelist stone(2);
                    stone.setupComponent(newchunk->getTileType(2).getData().c_size);
                    newchunk->setTile(j,i,stone);
                }
            }
            
        }
        if(y>12){
            if(registry::getEntities().size()>0){
                int i =0;
                for (auto& entity : registry::getEntities())
                {
                    
                    auto& data=entity.second.getData();
                    auto& entitybuffer=newchunk->entitiesBuffer;
                    entitylist ent(data.id,entity.second.getDefaultComponent(),Coord<unsigned int>((i*3*24/2)%1600,i*x%1600));
                    entitybuffer.push_back(ent);
                }

            }
        }
        std::cout<<"chunk generated success at ("<<x<<","<<y<<")\n";
        return newchunk;
    }
    
    inline void storeAndGenerateChunk(long long x,long long y){
        
        if(!internalUsedArea.is_found(x,y)){
            std::cout<<"generatechunk("<<y<<","<<x<<")\n";
            internalArea.set_ptr(x,y,chunkgenerator(x,y));
            internalUsedArea.usedChunkIds[y].usedChunkIds.insert(x);
            mapfile->creatmapfile(x,y);
        }else{
            return;
        }
        return;
    }
    inline void storeGeneratedChunk(long long x,long long y,const chunkmap& chunk){
        if(!internalUsedArea.is_found(x,y)){
            internalArea.set(x,y,chunk);
            internalUsedArea.usedChunkIds[y].usedChunkIds.insert(x);
        }
    }
    void getareaIDforplayer(
        const playerlist& player,
        long long radius,
        IndeksArea2d& pointerArea)
    {
        auto center = player.getchunkcoord();
    
        for (long long y = center.y - radius; y <= center.y + radius; ++y)
        {
            long long dy = std::llabs(y - center.y);
            if (dy > radius) continue;
        
            long long beda = radius - dy;
        
            for (long long x = center.x - beda; x <= center.x + beda; ++x)
            {
                pointerArea.usedChunkIds[y].usedChunkIds.insert(x);
            }
        }
    }

    const bool getChunkByID(long long x,long long y,chunkmap& temp){
        if(internalUsedArea.is_found(x,y)){
            temp=internalArea.get(x,y);
            return true;
        }
        auto tempptr = chunkgenerator(x,y);
        temp=chunkmap(*tempptr);
        delete tempptr;
        return 0;
    }
    void getChunkRefDumpForPlayer(const playerlist& player,unsigned short radius,std::vector<unsigned char>& buffer){
        IndeksArea2d tempo;
        getareaIDforplayer(player,radius,tempo);
        internalArea.dump_ref(buffer,tempo);
    }
    //dont forget to store it first
    void savechunk(long long x,long long y){
        if(internalUsedArea.is_found(x,y)){
            mapfile->save_chunk(x,y);
            return;
        }
    }
    ~gameMap(){
      if(mapfile){
        delete mapfile;
        mapfile=nullptr;
      }  
    }
    gameMap(){}
};